#define IMGUI_DEFINE_MATH_OPERATORS

#include "overlay.hpp"
#include "imgui/TextEditor.h"
#include <dwmapi.h>
#include "../../misc/output/output.hpp"
#include "../../cheats/features.h"

#include "../../misc/globals/globals.hpp"
#include "../../misc/configs/configs.hpp"
#include "../../library/json/json.hpp"

#include "ckey/keybind.hpp"
#include "../overlay/XorStr/xorstr.hpp"
#include "../overlay/XorStr/json.hpp"

#include <filesystem>
#include <thread>
#include <bitset>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include "../../cheats/esp/fonts.h"
#ifdef min
#undef min
#endif
#include <stack>

#ifdef max
#undef max
#endif
#include <Psapi.h>
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/misc/freetype/imgui_freetype.h"
#include "imgui/addons/imgui_addons.h"
#include "fonts/IconsFontAwesome6.h"
#include "fonts/roboto_medium.h"

template<typename T>
T custom_clamp(T value, T min, T max) {
	return value < min ? min : (value > max ? max : value);
}

std::string GetConfigFolderPath()
{
	std::string configFolderPath = RBX::appdata_path() + XorStr("\\shadow\\configs");

	if (!std::filesystem::exists(configFolderPath))
	{
		std::filesystem::create_directory(configFolderPath);
	}

	return configFolderPath;
}

ID3D11Device* overlay::d3d11Device = nullptr;
ID3D11DeviceContext* overlay::d3d11DeviceContext = nullptr;
IDXGISwapChain* overlay::dxgiSwapChain = nullptr;
ID3D11RenderTargetView* overlay::d3d11RenderTargetView = nullptr;

enum ImFonts_ : int
{
	ImFont_Main = 0,
	ImFont_Icons
};

enum MenuPages_ : int
{
	MenuPage_Home,
	MenuPage_Self,
	MenuPage_Players,
	MenuPage_Misc,
	MenuPage_Scripts,
	MenuPage_Appearance,
	MenuPage_Updates,

	MenuPages_COUNT
};

bool m_bDrawPage[MenuPages_COUNT] = { true, true, true, true, true, true, true };

static bool                     g_ShowMenu = false;
static float                    g_menu_alpha = 0.0f;
static const float              g_fade_speed = 6.5f;
std::vector<std::string> outputLines;
bool overlay::full_screen(HWND windowHandle)
{
	MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
	if (GetMonitorInfo(MonitorFromWindow(windowHandle, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
	{
		RECT windowRect;
		if (GetWindowRect(windowHandle, &windowRect))
		{
			return windowRect.left == monitorInfo.rcMonitor.left
				&& windowRect.right == monitorInfo.rcMonitor.right
				&& windowRect.top == monitorInfo.rcMonitor.top
				&& windowRect.bottom == monitorInfo.rcMonitor.bottom;
		}
	}
}

void SetPropertiesPanelStyle() {

	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.4f, 0.6f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.5f, 0.7f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.4f, 0.6f, 0.8f, 1.0f));


	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));


	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));


	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
}

void ResetPropertiesPanelStyle() {
	ImGui::PopStyleColor(6);
	ImGui::PopFont();
}


void RenderExplorer() {
	static RBX::Instance selectedPart;
	static std::unordered_set<uint64_t> spectatedParts;
	static std::unordered_map<uint64_t, std::vector<RBX::Instance>> nodeCache;
	static std::unordered_map<uint64_t, bool> nodeExpandedCache;
	static std::unordered_map<uint64_t, std::string> nodeNameCache;
	static std::unordered_map<uint64_t, std::string> nodeClassNameCache;
	static std::unordered_map<uint64_t, std::string> nodeTeamCache;
	static std::unordered_map<uint64_t, std::string> nodeUniqueIdCache;
	static std::unordered_map<uint64_t, std::string> nodeModel;
	static char searchQuery[128] = "";
	static std::vector<RBX::Instance> searchResults;
	static bool showSearchResults = false;
	static bool isCacheInitialized = false;
	static auto lastCacheRefresh = std::chrono::steady_clock::now();

	static std::unordered_map<uint64_t, std::string> nodePath;

	auto cacheNode = [&](RBX::Instance& node) {
		if (nodeCache.find(node.address) == nodeCache.end()) {
			nodeCache[node.address] = node.GetChildren(false);
			nodeNameCache[node.address] = node.GetName();
			nodeTeamCache[node.address] = node.GetTeam().GetName();
			nodeClassNameCache[node.address] = node.GetClass();
			nodeModel[node.address] = node.GetModelInstance().GetName();
			nodeUniqueIdCache[node.address] = std::to_string(node.address);

			std::string path = node.GetName();
			RBX::Instance parent = node.GetParent();
			while (parent.address != 0) {
				if (nodeCache.find(parent.address) == nodeCache.end()) {
					nodeCache[parent.address] = parent.GetChildren(false);
					nodeNameCache[parent.address] = parent.GetName();
				}
				std::string parentName = nodeNameCache[parent.address];
				if (!parentName.empty()) {
					path = parentName + "." + path;
				}
				parent = parent.GetParent();
			}
			nodePath[node.address] = path;
		}
		};

	try {
		auto& datamodel = globals::game;
		RBX::Instance root_instance(datamodel.address);

		auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCacheRefresh).count() >= 2) {
			nodeCache.clear();
			nodeNameCache.clear();
			nodeClassNameCache.clear();
			nodeUniqueIdCache.clear();
			isCacheInitialized = false;
			lastCacheRefresh = now;
		}

		if (!isCacheInitialized) {
			cacheNode(root_instance);
			isCacheInitialized = true;
		}

		ImGui::BeginChild("Explorer");

		ImGui::PushItemWidth(ImGui::GetWindowWidth() - 50);
		if (ImGui::InputText("##Search", searchQuery, IM_ARRAYSIZE(searchQuery))) {
			searchResults.clear();
			showSearchResults = strlen(searchQuery) > 0;

			if (showSearchResults && strlen(searchQuery) >= 1) {
				std::string query = searchQuery;
				std::transform(query.begin(), query.end(), query.begin(), ::tolower);

				std::function<void(RBX::Instance&)> searchInstance = [&](RBX::Instance& instance) {
					if (searchResults.size() >= 100) return;

					cacheNode(instance);

					std::string name = nodeNameCache[instance.address];
					std::transform(name.begin(), name.end(), name.begin(), ::tolower);

					if (name.find(query) != std::string::npos) {
						searchResults.push_back(instance);
					}

					for (auto& child : instance.GetChildren(false)) {
						searchInstance(child);
					}
					};

				auto& datamodel = globals::game;
				RBX::Instance root(datamodel.address);

				if (auto workspace = root.FindFirstChild("Workspace", false, {}); workspace.address != 0) {
					searchInstance(workspace);
				}

				if (globals::players.address != 0) {
					for (auto& player : globals::players.GetChildren(false)) {
						searchInstance(player);
					}
				}

				if (auto teams = root.FindFirstChild("Teams", false, {}); teams.address != 0) {
					searchInstance(teams);
				}
			}
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImAdd::Button("x", ImVec2(24, 24))) {
			memset(searchQuery, 0, sizeof(searchQuery));
			searchResults.clear();
			showSearchResults = false;
		}
		ImGui::PopStyleVar();

		ImAdd::BeginChild("Explorer", ImVec2(0, ImGui::GetContentRegionAvail().y - 205));

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 child_pos = ImGui::GetWindowPos();
		ImVec2 child_size = ImGui::GetWindowSize();
		ImU32 color_bottom = ImGui::ColorConvertFloat4ToU32(ImVec4(0.035f, 0.035f, 0.035f, 0.54f));
		ImU32 color_top = ImGui::ColorConvertFloat4ToU32(ImVec4(0.12f, 0.12f, 0.12f, 0.54f));
		draw_list->AddRectFilledMultiColor(child_pos,
			ImVec2(child_pos.x + child_size.x, child_pos.y + child_size.y),
			color_top, color_top, color_bottom, color_bottom);

		if (showSearchResults && strlen(searchQuery) >= 1) {
			if (!searchResults.empty()) {
				for (auto& node : searchResults) {
					if (!node.address) continue;

					if (nodeCache.find(node.address) == nodeCache.end()) {
						cacheNode(node);
					}

					ImGui::PushID(nodeUniqueIdCache[node.address].c_str());

					std::string displayText = nodeNameCache[node.address] + " [" + nodeClassNameCache[node.address] + "]";
					if (ImGui::TreeNodeEx(displayText.c_str(), ImGuiTreeNodeFlags_Leaf)) {
						ImGui::TreePop();
					}

					if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
						selectedPart = node;
					}

					if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
						ImGui::OpenPopup("NodeContextMenu");
						selectedPart = node;
					}

					if (ImGui::BeginPopup("NodeContextMenu")) {
						if (ImGui::MenuItem("Copy Hex Addy")) {
							ImGui::SetClipboardText(nodeUniqueIdCache[node.address].c_str());
						}
						if (ImAdd::Button("Set Part Collision true", ImVec2(-0.1f, 0))) {
							selectedPart.setCanCollide(true);
						}
						if (ImAdd::Button("Set Part Collision false", ImVec2(-0.1f, 0))) {
							selectedPart.setCanCollide(false);
						}
						if (ImGui::MenuItem("Teleport To Part")) {
							RBX::Vector3 partPosition = node.GetPosition();
							const float verticalOffset = 5.0f;
							partPosition.y += verticalOffset;
							globals::players.GetLocalPlayer().GetModelInstance().FindFirstChild("HumanoidRootPart", false, {}).SetPartPos(partPosition);
						}
						if (spectatedParts.count(node.address)) {
							if (ImGui::MenuItem("Unspectate Part")) {
								selectedPart.UnSpectate();
								spectatedParts.erase(node.address);
							}
						}
						else {
							if (ImGui::MenuItem("Spectate Part")) {
								selectedPart = node;
								selectedPart.Spectate(node);
								spectatedParts.insert(node.address);
							}
						}
						ImGui::EndPopup();
					}

					ImGui::PopID();
				}
			}
			else {
				ImAdd::Text(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "mo results found");
			}
		}

		else {
			for (auto& child : nodeCache[root_instance.address]) {
				std::stack<std::pair<RBX::Instance, int>> stack;
				stack.push({ child, 0 });

				while (!stack.empty()) {
					auto [node, indentLevel] = stack.top();
					stack.pop();

					cacheNode(node);

					ImGui::SetCursorPosX(20.0f * indentLevel);
					ImGui::PushID(nodeUniqueIdCache[node.address].c_str());

					const std::vector<RBX::Instance>& children = nodeCache[node.address];
					bool hasChildren = !children.empty();

					std::string displayText;
					if (nodeClassNameCache[node.address] == "Workspace") {

					}
					else if (nodeClassNameCache[node.address] == "Players") {

					}
					else if (nodeClassNameCache[node.address] == "Folder") {

					}
					else if (nodeClassNameCache[node.address] == "Part" ||
						nodeClassNameCache[node.address] == "BasePart") {

					}
					else if (nodeClassNameCache[node.address].find("Script") != std::string::npos) {

					}
					else {
						displayText = "";
					}

					displayText += " " + nodeNameCache[node.address] + " [" + nodeClassNameCache[node.address] + "]";

					bool isExpanded = ImGui::TreeNodeEx(displayText.c_str(),
						hasChildren ? 0 : ImGuiTreeNodeFlags_Leaf);

					if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
						selectedPart = node;
					}

					if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
						ImGui::OpenPopup("NodeContextMenu");
					}

					if (ImGui::BeginPopup("NodeContextMenu")) {
						if (ImGui::MenuItem("Copy Hex Addy")) {
							ImGui::SetClipboardText(nodeUniqueIdCache[node.address].c_str());
						}
						if (ImAdd::Button("Set Part Collision true", ImVec2(-0.1f, 0))) {
							selectedPart.setCanCollide(true);
						}
						if (ImAdd::Button("Set Part Collision false", ImVec2(-0.1f, 0))) {
							selectedPart.setCanCollide(false);
						}
						if (ImGui::MenuItem("Teleport To Part")) {
							RBX::Vector3 partPosition = node.GetPosition();
							const float verticalOffset = 5.0f;
							partPosition.y += verticalOffset;
							globals::players.GetLocalPlayer().GetModelInstance().FindFirstChild("HumanoidRootPart", false, {}).SetPartPos(partPosition);
						}
						if (spectatedParts.count(node.address)) {
							if (ImGui::MenuItem("Unspectate Part")) {
								selectedPart.UnSpectate();
								spectatedParts.erase(node.address);
							}
						}
						else {
							if (ImGui::MenuItem("Spectate Part")) {
								selectedPart = node;
								selectedPart.Spectate(node);
								spectatedParts.insert(node.address);
							}
						}
						ImGui::EndPopup();
					}

					if (isExpanded) {
						for (auto it = children.rbegin(); it != children.rend(); ++it) {
							stack.push({ *it, indentLevel + 1 });
						}
						ImGui::TreePop();
					}

					ImGui::PopID();
				}
			}
		}
		ImAdd::EndChild();

		ImGui::BeginChild("Properties", ImVec2(0, 200), true);
		SetPropertiesPanelStyle();

		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Properties");
		ImGui::Separator();

		if (selectedPart.address != 0) {

			if (ImGui::TreeNodeEx("Basic Information", ImGuiTreeNodeFlags_DefaultOpen)) {
				const std::string& nodeName = nodeNameCache[selectedPart.address];
				const std::string& className = nodeClassNameCache[selectedPart.address];
				const std::string& model = nodeModel[selectedPart.address];
				RBX::Instance parent = selectedPart.GetParent();
				std::string parentName = nodeNameCache[parent.address];

				ImGui::Text("Path:"); ImGui::SameLine(120);
				ImGui::TextWrapped("%s", nodePath[selectedPart.address].c_str());

				ImGui::Text("Name:"); ImGui::SameLine(120);
				ImGui::Text("%s", nodeName.c_str());

				ImGui::Text("Class:"); ImGui::SameLine(120);
				ImGui::Text("%s", className.c_str());

				ImGui::Text("Parent:"); ImGui::SameLine(120);
				ImGui::Text("%s", parentName.c_str());

				ImGui::Text("Model:"); ImGui::SameLine(120);
				ImGui::Text("%s", model.c_str());

				ImGui::TreePop();
			}


			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				RBX::Vector3 position = selectedPart.GetPosition();
				ImGui::Text("Position:"); ImGui::SameLine(120);
				ImGui::Text("X: %.1f  Y: %.1f  Z: %.1f", position.x, position.y, position.z);

				RBX::Vector3 size = selectedPart.GetSize();
				ImGui::Text("Size:"); ImGui::SameLine(120);
				ImGui::Text("X: %.1f  Y: %.1f  Z: %.1f", size.x, size.y, size.z);

				RBX::Matrix3x3 rotation = selectedPart.GetRotation();
				ImGui::Text("Raw Matrix:"); ImGui::SameLine(120);
				ImGui::Text("[Matrix3x3]");

				ImGui::TreePop();
			}


			if (ImGui::TreeNodeEx("Appearance", ImGuiTreeNodeFlags_DefaultOpen)) {
				int color = selectedPart.GetColor3();
				ImGui::Text("Color:"); ImGui::SameLine(120);
				ImGui::Text("0x%X", color);

				ImGui::TreePop();
			}


			if (ImGui::TreeNodeEx("Part Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
				static bool canCollide = false;
				if (ImGui::Checkbox("CanCollide", &canCollide)) {
					selectedPart.setCanCollide(canCollide);
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Instance Value", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Value:"); ImGui::SameLine(120);
				const std::string& className = nodeClassNameCache[selectedPart.address];

				if (className == "BoolValue") {
					bool boolValue = selectedPart.getBoolFromValue();
					ImGui::Text("%s", boolValue ? "true" : "false");
				}
				else if (className == "IntValue") {
					int intValue = selectedPart.getIntFromValue();
					ImGui::Text("%d", intValue);
				}
				else if (className == "StringValue") {
					std::string stringValue = selectedPart.getStringFromValue();
					ImGui::Text("%s", stringValue.c_str());
				}
				else if (className == "NumberValue") {
					float floatValue = selectedPart.GetFloatFromValue();
					ImGui::Text("%.2f", floatValue);
				}
				else {
					ImGui::Text("No specific value");
				}

				ImGui::TreePop();
			}


			if (ImGui::TreeNodeEx("Network", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Address:"); ImGui::SameLine(120);
				ImGui::Text("0x%llX", selectedPart.address);

				ImGui::TreePop();
			}
		}
		ImGui::EndChild();
	}

	catch (...) {

	}
}

static ImVec4 ui_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


bool overlay::init = false;
void DrawMultiCombo(const char* label, const std::vector<std::string>& items, std::vector<bool>& selections) {

	std::string preview;
	for (size_t i = 0; i < items.size(); ++i) {
		if (selections[i]) {
			if (!preview.empty()) {
				preview += ", ";
			}
			preview += items[i];
		}
	}
	if (preview.empty()) {
		preview = "Select items";
	}

	if (ImGui::BeginCombo(label, preview.c_str())) {
		for (size_t i = 0; i < items.size(); ++i) {

			bool selected = selections[i];
			if (ImGui::Selectable(items[i].c_str(), selected)) {
				selections[i] = !selections[i];
			}

			if (selections[i]) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void applyBlur(const ImVec2& displaySize, float blurStrength) {
	ImDrawList* draw = ImGui::GetBackgroundDrawList();

	const int sampleCount = 8;

	const float offsets[sampleCount][2] = {
		{ -1.0f, -1.0f },
		{  1.0f, -1.0f },
		{ -1.0f,  1.0f },
		{  1.0f,  1.0f },
		{ -1.5f,  0.0f },
		{  1.5f,  0.0f },
		{  0.0f, -1.5f },
		{  0.0f,  1.5f }
	};

	ImU32 blurColor = ImGui::ColorConvertFloat4ToU32({ 0.0f, 0.0f, 0.0f, 0.15f });
	for (int i = 0; i < sampleCount; i++) {
		ImVec2 offset(offsets[i][0] * blurStrength, offsets[i][1] * blurStrength);
		draw->AddRectFilled(offset, ImVec2(displaySize.x + offset.x, displaySize.y + offset.y), blurColor);
	}
}
bool Keybind(CKeybind* keybind, const ImVec2& size_arg = ImVec2(0, 0), bool clicked = false, ImGuiButtonFlags flags = 0)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(keybind->get_name().c_str());
	const ImVec2 label_size = ImGui::CalcTextSize(keybind->get_name().c_str(), NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) &&
		style.FramePadding.y <
		window->DC.CurrLineTextBaseOffset)

		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = ImGui::CalcItemSize(
		size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (g.CurrentItemFlags & ImGuiItemFlags_ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	bool value_changed = false;
	int key = keybind->key;

	auto io = ImGui::GetIO();

	std::string name = keybind->get_key_name();

	if (keybind->waiting_for_input)
		name = "..";

	if (ImGui::GetIO().MouseClicked[0] && hovered)
	{
		if (g.ActiveId == id)
		{
			keybind->waiting_for_input = true;
		}
	}
	else if (ImGui::GetIO().MouseClicked[1] && hovered)
	{
		ImGui::OpenPopup(keybind->get_name().c_str());
	}
	else if (ImGui::GetIO().MouseClicked[0] && !hovered)
	{
		if (g.ActiveId == id)
			ImGui::ClearActiveID();
	}

	if (keybind->waiting_for_input)
	{
		if (ImGui::GetIO().MouseClicked[0] && !hovered)
		{
			keybind->key = VK_LBUTTON;

			ImGui::ClearActiveID();
			keybind->waiting_for_input = false;
		}
		else
		{
			if (keybind->set_key())
			{
				ImGui::ClearActiveID();
				keybind->waiting_for_input = false;
			}
		}
	}

	ImVec4 textcolor = ImLerp(ImVec4(201 / 255.f, 204 / 255.f, 210 / 255.f, 1.f), ImVec4(1.0f, 1.0f, 1.0f, 1.f), 1.f);

	window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(33 / 255.0f, 33 / 255.0f, 33 / 255.0f, 1.f), 2.f);

	window->DrawList->AddText(
		bb.Min +
		ImVec2(
			size_arg.x / 2 - ImGui::CalcTextSize(name.c_str()).x / 2,
			size_arg.y / 2 - ImGui::CalcTextSize(name.c_str()).y / 2),
		ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
		name.c_str());

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::SetNextWindowPos(pos + ImVec2(0, size_arg.y - 1));
	ImGui::SetNextWindowSize(ImVec2(size_arg.x, 47 * 1.f));

	{
		if (ImGui::BeginPopup(keybind->get_name().c_str(), 0))
		{

			{
				{
					ImGui::BeginGroup();
					{
						if (ImGui::Selectable(XorStr("Toggle"), keybind->type == CKeybind::TOGGLE))
							keybind->type = CKeybind::TOGGLE;
						if (ImGui::Selectable(XorStr("Hold"), keybind->type == CKeybind::HOLD))
							keybind->type = CKeybind::HOLD;
					}
					ImGui::EndGroup();
				}
			}

			ImGui::EndPopup();
		}
	}

	return pressed;
}








void render_fullscreen_background(float alpha, float blurAmount) {

	applyBlur(ImGui::GetIO().DisplaySize, blurAmount);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, alpha));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin("background", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings);
	ImGui::End();
	ImGui::PopStyleColor();
}

TextEditor editor;
void RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
	const float border_size = g.Style.FrameBorderSize;
	if (border && border_size > 0.0f)
	{
		window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), rounding, 0, border_size);
		window->DrawList->AddRect(p_min, p_max, ImGui::GetColorU32(ImGuiCol_Border), rounding, 0, border_size);
	}
}

void RenderFrameBorder(ImVec2 p_min, ImVec2 p_max, float rounding)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	const float border_size = g.Style.FrameBorderSize;
	if (border_size > 0.0f)
	{
		window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), rounding, 0, border_size);
		window->DrawList->AddRect(p_min, p_max, ImGui::GetColorU32(ImGuiCol_Border), rounding, 0, border_size);
	}
}

bool overlay::render() {
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	ImFontConfig font_config;
	font_config.PixelSnapH = false;
	font_config.OversampleH = 5;
	font_config.OversampleV = 5;
	font_config.RasterizerMultiply = 1.2f;

	static const ImWchar ranges[] = {
		0x0020, 0x00FF,
		0x0400, 0x052F,
		0x2DE0, 0x2DFF,
		0xA640, 0xA69F,
		0xE000, 0xE226,
		0,
	};

	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wc.lpszMenuName = nullptr;

	wc.lpszClassName = TEXT("quiet_overlay_class");
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		DWORD error = GetLastError();
		char errMsg[128];
		sprintf_s(errMsg, "RegisterClassEx failed: GetLastError = %lu", error);
		MessageBoxA(nullptr, errMsg, "error", MB_OK | MB_ICONERROR);
		return false;
	}

	const HWND hw = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
		wc.lpszClassName,
		TEXT("shadow"),
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	if (!hw) {
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return false;
	}

	SetLayeredWindowAttributes(hw, 0, 255, LWA_ALPHA);
	const MARGINS margin = { -1 };
	if (FAILED(DwmExtendFrameIntoClientArea(hw, &margin))) {
		DestroyWindow(hw);
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return false;
	}

	if (!create_device_d3d(hw)) {
		cleanup_device_d3d();
		DestroyWindow(hw);
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return false;
	}

	ShowWindow(hw, SW_SHOW);
	UpdateWindow(hw);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui::GetIO().IniFilename = nullptr;
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(hw);
	ImGui_ImplDX11_Init(d3d11Device, d3d11DeviceContext);

	const ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	init = true;
	static ImFont* pixelatedFont{ NULL };
	bool SpotifyBar = true;
	bool draw = false;
	bool WatermarkNshit = true;
	bool done = false;
	int tab = 0;
	bool teamcheck = false;
	auto last_time = std::chrono::high_resolution_clock::now();
	bool watermark = false;
	static float rot_angle = 0.0f;
	static const float rot_speed = 10.0f;
	static std::map<std::string, bool> strafe_map;
	float alpha = g_menu_alpha;
	bool gamelol = false;
	char buffer[256];
	std::string wmtext = "Shadow.hook";
	char textInput[256] = "Shadow.hook";
	ImFont* font = io.Fonts->AddFontFromMemoryTTF(font_Bytes, sizeof(font_Bytes), 14.3f, &font_config, io.Fonts->GetGlyphRangesJapanese());

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig cfg;
	cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;
	cfg.SizePixels = 12;
	ImFontConfig fa_config; fa_config.MergeMode = true; fa_config.PixelSnapH = true;
	fa_config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;
	io.Fonts->AddFontFromMemoryCompressedTTF(roboto_medium_compressed_data, roboto_medium_compressed_size, 12, &cfg, io.Fonts->GetGlyphRangesDefault());
	io.Fonts->AddFontDefault(&cfg);
	io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 14, &cfg, icons_ranges);

	std::time_t t = std::time(nullptr);
	std::tm local_tm = *std::localtime(&t);
	while (!done) {
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				done = true;
			}
		}

		if (done)
			break;

		move_window(hw);
		if (GetAsyncKeyState(VK_F1) & 1 || GetAsyncKeyState(VK_INSERT) & 1)
			draw = !draw;

		auto current_time = std::chrono::high_resolution_clock::now();
		float delta_time = std::chrono::duration<float>(current_time - last_time).count();
		last_time = current_time;

		if (g_ShowMenu) {
			g_menu_alpha += g_fade_speed * delta_time;
			if (g_menu_alpha > 1.0f) g_menu_alpha = 1.0f;
		}
		else {
			g_menu_alpha -= g_fade_speed * delta_time;
			if (g_menu_alpha < 0.0f) g_menu_alpha = 0.0f;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (GetForegroundWindow() == FindWindowA(0, XorStr("Roblox")) || GetForegroundWindow() == hw) {



			ImVec4 babyBlue = ImVec4(0.537f, 0.811f, 0.941f, 1.0f);
			ImVec4 darkBlue = ImVec4(0.0f, 0.0f, 0.5f, 1.0f);

			ImU32 accent_color_u32 = ImGui::ColorConvertFloat4ToU32(babyBlue);
			ImU32 border_accent_color_u32 = ImGui::ColorConvertFloat4ToU32(darkBlue);

			ImGui::Begin(XorStr("ESP"), nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
			{

				ImGui::PushFont(font);

				RBX::hook_esp();

				ImGui::PopFont();

				ImGui::End();

			}
			if (WatermarkNshit) {
				auto now = std::chrono::system_clock::now();
				std::time_t now_time = std::chrono::system_clock::to_time_t(now);
				std::tm local_tm = *std::localtime(&now_time);

				char buffer[128];
				std::strftime(buffer, sizeof(buffer), "Shadow.Hook - %B %d %Y - %H:%M:%S", &local_tm);

				const char* prefix = "Shadow";
				const char* hookPart = ".Hook";

				const char* rest = buffer + strlen("Shadow.Hook");

				std::stringstream ss;
				ss << rest << " - " << static_cast<int>(io.Framerate) << " FPS";

				ImVec2 textSizePrefix = ImGui::CalcTextSize(prefix);
				ImVec2 textSizeHook = ImGui::CalcTextSize(hookPart);
				ImVec2 textSizeRest = ImGui::CalcTextSize(ss.str().c_str());
				float totalWidth = textSizePrefix.x + textSizeHook.x + textSizeRest.x;
				float maxHeight = std::max({ textSizePrefix.y, textSizeHook.y, textSizeRest.y });
				ImVec2 padding = ImGui::GetStyle().WindowPadding;

				static ImVec2 watermark_pos = ImVec2(10, 10);
				ImGui::SetNextWindowPos(watermark_pos, ImGuiCond_Once);
				ImGui::SetNextWindowSize(ImVec2(totalWidth + padding.x * 2, maxHeight + padding.y * 2), ImGuiCond_Once);

				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
				ImGui::Begin("watermark", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
				{
					ImVec2 window_pos = ImGui::GetWindowPos();
					ImVec2 window_size = ImGui::GetWindowSize();
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					ImU32 color_top = ImGui::ColorConvertFloat4ToU32(ImVec4(0.25f, 0.25f, 0.25f, 0.55f));
					ImU32 color_bottom = ImGui::ColorConvertFloat4ToU32(ImVec4(0.035f, 0.035f, 0.035f, 0.478f));
					draw_list->AddRectFilledMultiColor(window_pos, ImVec2(window_pos.x + window_size.x, window_pos.y + window_size.y), color_top, color_top, color_bottom, color_bottom);
					draw_list->AddRect(window_pos, ImVec2(window_pos.x + window_size.x, window_pos.y + window_size.y), accent_color_u32, 0.0f, ImDrawFlags_RoundCornersAll, 3.0f);

					ImGui::PushStyleColor(ImGuiCol_Text, babyBlue);
					ImGui::TextUnformatted(prefix);
					ImGui::PopStyleColor();
					ImGui::SameLine(0.0f, 0.0f);
					ImGui::PushStyleColor(ImGuiCol_Text, darkBlue);
					ImGui::TextUnformatted(hookPart);
					ImGui::PopStyleColor();
					ImGui::SameLine(0.0f, 0.0f);
					ImGui::TextUnformatted(ss.str().c_str());
				}
				ImGui::End();
				ImGui::PopStyleVar();
			}
		}
		if (draw) {
			render_fullscreen_background(0.75f, 10.0f);

			RenderExplorer();
			if (m_bDrawPage[MenuPage_Home])
			{
				ImGuiStyle& style = ImGui::GetStyle();
				ImGui::StyleColorsDark();
				style.WindowRounding = 0;
				style.ChildRounding = 0;
				style.FrameRounding = 0;
				style.PopupRounding = 0;
				style.ScrollbarRounding = 0;
				style.TabRounding = 0;
				style.ButtonTextAlign = { 0.5f, 0.5f };
				style.WindowTitleAlign = { 0.5f, 0.5f };
				style.FramePadding = { 3.0f, 3.0f };
				style.WindowPadding = { 6.0f, 6.0f };
				style.ItemSpacing = style.WindowPadding;
				style.ItemInnerSpacing = { style.ItemSpacing.x, 5 };
				style.WindowBorderSize = 1;
				style.FrameBorderSize = 1;
				style.PopupBorderSize = 1;
				style.ScrollbarSize = 4.f;
				style.GrabMinSize = 4;
				style.WindowShadowSize = 50;
				style.Colors[ImGuiCol_WindowBg] = ImAdd::HexToColorVec4(0x232323, 1.00f);
				style.Colors[ImGuiCol_ChildBg] = ImAdd::HexToColorVec4(0x232323, 1.00f);
				style.Colors[ImGuiCol_PopupBg] = ImAdd::HexToColorVec4(0x1C1C1C, 1.00f);
				style.Colors[ImGuiCol_Text] = ImAdd::HexToColorVec4(0xC8C8C8, 1.00f);
				style.Colors[ImGuiCol_TextDisabled] = ImAdd::HexToColorVec4(0x888888, 1.00f);
				style.Colors[ImGuiCol_TextSelectedBg] = ImAdd::HexToColorVec4(0xA050E0, 1.00f);
				style.Colors[ImGuiCol_TitleBg] = ImAdd::HexToColorVec4(0x1B1B1B, 1.00f);
				style.Colors[ImGuiCol_TitleBgActive] = ImAdd::HexToColorVec4(0x1D1D1D, 1.00f);
				style.Colors[ImGuiCol_TitleBgCollapsed] = ImAdd::HexToColorVec4(0x1E1E1E, 0.75f);
				style.Colors[ImGuiCol_Border] = ImAdd::HexToColorVec4(0x353543, 1.00f);
				style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
				style.Colors[ImGuiCol_SliderGrab] = ImAdd::HexToColorVec4(0xA050E0, 1.00f);
				style.Colors[ImGuiCol_SliderGrabActive] = ImAdd::HexToColorVec4(0x7035A0, 1.00f);
				style.Colors[ImGuiCol_Button] = ImAdd::HexToColorVec4(0x1D1D1D, 1.00f);
				style.Colors[ImGuiCol_ButtonHovered] = ImAdd::HexToColorVec4(0x222222, 1.00f);
				style.Colors[ImGuiCol_ButtonActive] = ImAdd::HexToColorVec4(0x282828, 1.00f);
				style.Colors[ImGuiCol_FrameBg] = style.Colors[ImGuiCol_Button];
				style.Colors[ImGuiCol_FrameBgHovered] = style.Colors[ImGuiCol_ButtonHovered];
				style.Colors[ImGuiCol_FrameBgActive] = style.Colors[ImGuiCol_ButtonActive];
				style.Colors[ImGuiCol_Header] = ImAdd::HexToColorVec4(0x1E1E1D, 1.00f);
				style.Colors[ImGuiCol_HeaderHovered] = ImAdd::HexToColorVec4(0x232323, 1.00f);
				style.Colors[ImGuiCol_HeaderActive] = ImAdd::HexToColorVec4(0x282828, 1.00f);
				style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0);
				style.Colors[ImGuiCol_ScrollbarGrab] = style.Colors[ImGuiCol_SliderGrab];
				style.Colors[ImGuiCol_ScrollbarGrabHovered] = style.Colors[ImGuiCol_SliderGrab];
				style.Colors[ImGuiCol_ScrollbarGrabActive] = style.Colors[ImGuiCol_SliderGrab];
				style.Colors[ImGuiCol_WindowShadow] = ImAdd::HexToColorVec4(0x000000, 0.20f);
				style.Colors[ImGuiCol_ResizeGrip] = ImVec4();
				style.Colors[ImGuiCol_ResizeGripActive] = ImVec4();
				style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4();

				ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
				ImGui::SetNextWindowSize(ImVec2(390, 430), ImGuiCond_Once);
				ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrab]);
				ImGui::PushStyleColor(ImGuiCol_WindowShadow, style.Colors[ImGuiCol_SliderGrab]);
				ImGui::Begin("Shadow", nullptr, ImGuiWindowFlags_NoTitleBar);
				ImGui::PopStyleColor(2);
				{
					ImAdd::Text(style.Colors[ImGuiCol_SliderGrab], "Shadow.hook");
					ImGui::BeginChild("SecondLayer", ImVec2(), ImGuiChildFlags_Border);
					{
						static int iTabID = 0;
						float fRadioWidth = (ImGui::GetWindowWidth() - style.WindowPadding.x * 2) / 5;
						ImGui::BeginGroup();
						{
							ImAdd::FrameRadio("Legit", &iTabID, 0, ImVec2(fRadioWidth, ImGui::GetFontSize() + style.WindowPadding.y * 2)); ImGui::SameLine(0, 0);
							ImAdd::FrameRadio("Visuals", &iTabID, 1, ImVec2(fRadioWidth, ImGui::GetFontSize() + style.WindowPadding.y * 2)); ImGui::SameLine(0, 0);
							ImAdd::FrameRadio("Players", &iTabID, 2, ImVec2(fRadioWidth, ImGui::GetFontSize() + style.WindowPadding.y * 2)); ImGui::SameLine(0, 0);
							ImAdd::FrameRadio("Misc", &iTabID, 3, ImVec2(fRadioWidth, ImGui::GetFontSize() + style.WindowPadding.y * 2)); ImGui::SameLine(0, 0);
							ImAdd::FrameRadio("Lighting", &iTabID, 4, ImVec2(fRadioWidth, ImGui::GetFontSize() + style.WindowPadding.y * 2)); ImGui::SameLine(0, 0);


						}

						ImGui::EndGroup();
						ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_WindowBg]);
						ImGui::BeginChild("TabsLayer", ImVec2(0, 0), ImGuiChildFlags_Border);
						ImGui::PopStyleColor();
						{
							float fGroupWidth = (ImGui::GetWindowWidth() - style.ItemSpacing.x - style.WindowPadding.x * 2) / 2;
							float fGroupHeight = (ImGui::GetWindowHeight() - style.ItemSpacing.y - style.WindowPadding.y * 2) / 2;

							ImGui::BeginGroup();
							{
								if (iTabID == 0) {

									ImGui::BeginGroup();
									{
										ImAdd::BeginChild("Aim Assit", ImVec2(fGroupWidth, 0));
										{
											ImAdd::CheckBox("Aimbot", &globals::aimbot);

											if (globals::aimbot)
											{




												ImGui::Spacing();
												ImGui::Spacing();

												const char* aimbot_type_options[] = { "Camera", "Mouse" };
												ImAdd::Combo("Type", &globals::aimbot_type, aimbot_type_options, IM_ARRAYSIZE(aimbot_type_options));
											}



										}
										ImAdd::EndChild();
									}
									ImGui::EndGroup();
									ImGui::SameLine();
									ImGui::BeginGroup();
									{
										ImAdd::BeginChild("Aim Settings", ImVec2(0, fGroupHeight));
										{
											const char* aimbot_hitpart_options[] = { "Head", "RootPart", "UpperRoot", "LowerRoot" };
											ImAdd::Combo("Hitpart", &globals::aimbot_part, aimbot_hitpart_options, IM_ARRAYSIZE(aimbot_hitpart_options));

											if (globals::aimbot_type == 0) {
												ImAdd::SliderFloat("Smoothness", &globals::smoothness_camera, 1.0f, 100.0f);
											}
											else if (globals::aimbot_type == 1) {
												ImAdd::SliderFloat("Smoothness", &globals::mouse_smoothness, 1.0f, 100.0f);
												ImAdd::SliderFloat("Strength", &globals::mouse_sensitivity, 1.0f, 100.0f);
											}

											Keybind(&globals::aimbot_bind, ImVec2(40, 15));


										}
										ImAdd::EndChild();
										ImAdd::BeginChild("Misc", ImVec2(0, 0));
										{
											ImAdd::CheckBox(XorStr("Sticky Aim"), &globals::aimbot_sticky);
											ImGui::Spacing();
											ImAdd::CheckBox(XorStr("Prediction"), &globals::camera_prediction);
											if (globals::camera_prediction) {
												ImAdd::SliderFloat("Prediction X", &globals::camera_prediction_x, 1.0f, 50.f);
												ImAdd::SliderFloat("Prediction Y", &globals::camera_prediction_y, 1.0f, 50.f);
											}

											ImGui::Spacing();

											std::vector<std::string> item_names = {
		"Knocked Check",
		"Dead Check",
		"Grabbed Check",
		"Team Check"
											};
											DrawMultiCombo("Aim Assist Checks", item_names, globals::aimbot_checks);


										}
										ImAdd::EndChild();
									}

								}
								else if (iTabID == 1) {

									ImGui::BeginGroup();
									{
										ImAdd::BeginChild("Visuals", ImVec2(fGroupWidth, 0));
										{
											ImAdd::CheckBox("ESP", &globals::esp);
											ImAdd::CheckBox("Anti Aliasing", &globals::autoparry);
											ImAdd::SliderFloat("Alpha", &globals::alpha, 0, 150);
											ImAdd::CheckBox("Bounding Box", &globals::box_esp);
											ImAdd::CheckBox("Fill", &globals::fill_box);
											std::vector<std::string> box_options = { "2d", "2d cornered" };

											ImGui::SeparatorText("CrossHair and FOV options");

											ImAdd::CheckBox("Fov", &globals::draw_aimbot_fov);
											ImAdd::SliderFloat("Fov Size", &globals::aimbot_fov_size, 1, 200);
											ImAdd::CheckBox("CrossHair", &globals::crosshair);
											ImAdd::SliderInt("Crosshair Gap", &globals::crosshair_gap, 1, 15);
											ImAdd::SliderInt("Crosshair Size", &globals::crosshair_size, 1, 40);
											ImAdd::SliderInt("Crosshair Speed", &globals::crosshair_speed, 1, 20);


										}
										ImAdd::EndChild();
									}
									ImGui::EndGroup();

									ImGui::SameLine();
									ImGui::BeginGroup();
									{
										ImAdd::BeginChild("Visual Settings", ImVec2(0, fGroupHeight));
										{
										std::vector<std::string> item_names = {
	
    "Knocked Check",
	"Health Check",
	"Grabbed Check",
	"Team Check"
											};
											DrawMultiCombo("Esp Checks", item_names, globals::esp_checks);
											ImAdd::CheckBox("Name", &globals::name_esp);
											ImAdd::CheckBox("Tool", &globals::tool_esp);
											ImAdd::CheckBox("Distance", &globals::distance_esp);
											ImAdd::CheckBox("Healthbar", &globals::health_bar);
											ImAdd::CheckBox("Name Outline", &globals::name_outline);
											ImAdd::CheckBox("Tool Outline", &globals::tool_outline);
											ImAdd::CheckBox("Distance Outline", &globals::distance_outline);
											ImAdd::CheckBox("Health Bar Text", &globals::health_bar_text);
											ImAdd::SliderFloat("Max Distance", &globals::max_render_distance, 0.0f, 10000.0f);



										}
										ImAdd::EndChild();

										ImAdd::BeginChild("Vehicle & Chams Settings", ImVec2(0, 0));
										{
											ImAdd::CheckBox("Dahood Vehicles Esp", &globals::skeleton_esp);
											ImAdd::CheckBox("Chams", &globals::chams);
											ImAdd::CheckBox("Cham Outline", &globals::chams_outline);
											ImAdd::CheckBox("Cham Fill", &globals::fill_chams);

											ImAdd::EndChild();
											ImGui::EndGroup();
										}
									}
								}

								else if (iTabID == 2) {
									ImAdd::BeginChild("Players", ImVec2(fGroupWidth, 0));
									{
										ImAdd::Text(style.Colors[ImGuiCol_Text], "PlayerList");

										const char* statusItems[] = { "Neutral", "Friend", "Enemy" };

										static int selectedPlayerIndex = -1;
										static std::unordered_map<std::string, int> playerStatuses;
										std::vector<std::string> playerNames;

										for (const auto& playerName : globals::players.GetChildren(false)) {
											playerNames.push_back(playerName.GetName());
										}

										if (ImAdd::BeginCombo("Select Player", selectedPlayerIndex == -1 ? "None" : playerNames[selectedPlayerIndex].c_str())) {
											for (int i = 0; i < playerNames.size(); ++i) {
												bool isSelected = (selectedPlayerIndex == i);
												if (ImAdd::Selectable(playerNames[i].c_str(), isSelected)) {
													selectedPlayerIndex = i;
												}
											}
											ImGui::EndCombo();
										}

										if (selectedPlayerIndex >= 0 && selectedPlayerIndex < playerNames.size()) {
											const std::string& selectedPlayerName = playerNames[selectedPlayerIndex];

											if (playerStatuses.find(selectedPlayerName) == playerStatuses.end()) {
												playerStatuses[selectedPlayerName] = 0;
											}

											ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

											switch (playerStatuses[selectedPlayerName]) {
											case 1:
												textColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
												break;
											case 2:
												textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
												break;
											}

											ImAdd::Text(textColor, selectedPlayerName.c_str());

											if (ImAdd::BeginCombo(("##Status" + selectedPlayerName).c_str(), statusItems[playerStatuses[selectedPlayerName]])) {
												for (int i = 0; i < IM_ARRAYSIZE(statusItems); ++i) {
													bool isSelected = (playerStatuses[selectedPlayerName] == i);
													if (ImAdd::Selectable(statusItems[i], isSelected)) {

														playerStatuses[selectedPlayerName] = i;
														
													}
												}
												ImGui::EndCombo();
											}
										}
									}
									ImAdd::EndChild();
								}

								else if (iTabID == 3) {
									ImAdd::BeginChild("Misc", ImVec2(fGroupWidth, 0));
									{
										ImAdd::Text(style.Colors[ImGuiCol_Text], "Settings");
										ImAdd::CheckBox("Lplayer", &globals::localplayercheck);
										ImAdd::CheckBox("Only Lplayer", &globals::onlylocal);
										ImAdd::CheckBox("Vsync", &globals::vsync);
										ImAdd::CheckBox("Off Screen Check", &globals::offscreen_Check);
										ImAdd::CheckBox("Cache Off Screen Check", &globals::cframe);
										ImAdd::CheckBox("Slow Cache", &globals::HeavyOptimize);
										ImAdd::CheckBox("WaterMark", &WatermarkNshit);
										const float width = ImGui::GetContentRegionAvail().x;

										ImAdd::CheckBox("Auto Rescan", &globals::rescan);
										if (ImAdd::Button("Rescan", ImVec2(-0.1f, 0))) {
											RBX::load_game_data();






										}
									}
									ImAdd::EndChild();
								}




								else if (iTabID == 4) {

									static float fogEnd = 500.0f;
									static float fogStart = 100.0f;
									static float clockTime = 12.0f;
									static float brightness = 1.0f;
									static float environmentScale = 1.0f;
									static float fps = 1.0f;
									static float shadowsoftness = 0.0f;
									static ImVec4 fogColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
									static ImVec4 ambienceColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
									static ImVec4 backDoorAmbience = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
									static ImVec4 colorShiftBottom = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
									static ImVec4 colorShiftTop = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
									static ImVec4 outdoorAmbient = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

									static bool globalShadows = true;


									// First BeginChild (Fog and Lighting Settings)
									ImGui::BeginGroup();
									{
										ImAdd::BeginChild("Fog & Lighting Settings", ImVec2(fGroupWidth, 0));
										{
											ImAdd::SliderFloat("Fog End", &fogEnd, 0.0f, 1000.0f);
											ImAdd::SliderFloat("Fog Start", &fogStart, 0.0f, 1000.0f);
											ImAdd::SliderFloat("Shadow Softness", &shadowsoftness, 0.0f, 1000.0f);
											ImAdd::SliderFloat("Clock Time", &clockTime, 0.0f, 24.0f);
											ImAdd::SliderFloat("Brightness", &brightness, 0.0f, 5.0f);
											ImAdd::SliderFloat("Environment Scale", &environmentScale, 0.0f, 5.0f);
										}
										ImAdd::EndChild();
									}
									ImGui::EndGroup();


									ImGui::SameLine();
									ImGui::BeginGroup();
									{
										ImAdd::BeginChild("Color Settings", ImVec2(0, fGroupHeight));
										{
											ImAdd::ColorEdit4("Fog Color", (float*)&fogColor);
											ImAdd::ColorEdit4("Ambience Color", (float*)&ambienceColor);
											ImAdd::ColorEdit4("Back Door Ambience Color", (float*)&backDoorAmbience);
											ImAdd::ColorEdit4("Color Shift Bottom", (float*)&colorShiftBottom);
											ImAdd::ColorEdit4("Color Shift Top", (float*)&colorShiftTop);
											ImAdd::ColorEdit4("Outdoor Ambient", (float*)&outdoorAmbient);




										}
										ImAdd::EndChild();
									}
									ImGui::EndGroup();


									auto lighting = globals::lighting;
									lighting.setFogEnd(fogEnd);
									lighting.setFogStart(fogStart);
									lighting.setBrightness(brightness);
									lighting.setFogColor({ fogColor.x, fogColor.y, fogColor.z });
									lighting.setAmbientColor({ ambienceColor.x, ambienceColor.y, ambienceColor.z });
									lighting.setBackDoorAmbienceColor({ backDoorAmbience.x, backDoorAmbience.y, backDoorAmbience.z });
									lighting.setColorShiftBottom({ colorShiftBottom.x, colorShiftBottom.y, colorShiftBottom.z });
									lighting.setColorShiftTop({ colorShiftTop.x, colorShiftTop.y, colorShiftTop.z });
									lighting.setOutdoorAmbient({ outdoorAmbient.x, outdoorAmbient.y, outdoorAmbient.z });
									lighting.setGlobalShadows(globalShadows);


								}
							}
						}
					}
				}
			}



			if (m_bDrawPage[MenuPage_Self])
			{

			}

			if (m_bDrawPage[MenuPage_Players])
			{

			}

			if (m_bDrawPage[MenuPage_Misc])
			{

			}

			if (m_bDrawPage[MenuPage_Scripts])
			{

			}

			if (m_bDrawPage[MenuPage_Appearance])
			{

			}

			if (m_bDrawPage[MenuPage_Updates])
			{

			}

		}

		if (draw) {
			SetLayeredWindowAttributes(hw, 0, 255, LWA_ALPHA);
			const MARGINS margin = { -1 };
			DwmExtendFrameIntoClientArea(hw, &margin);
			SetWindowLong(hw, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

			BringWindowToTop(hw);
			SetForegroundWindow(hw);
		}
		else {
			SetWindowLong(hw, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
			SetLayeredWindowAttributes(hw, 0, 255, LWA_ALPHA);
			const MARGINS margin = { -1 };
			DwmExtendFrameIntoClientArea(hw, &margin);
		}

		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		d3d11DeviceContext->OMSetRenderTargets(1, &d3d11RenderTargetView, nullptr);
		d3d11DeviceContext->ClearRenderTargetView(d3d11RenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		dxgiSwapChain->Present(globals::vsync, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(!globals::highcpuusageesp)));
	}

	init = false;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_device_d3d();
	DestroyWindow(hw);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
}

void overlay::move_window(HWND hw)
{
	HWND target = FindWindowA(nullptr, "Roblox");
	HWND foregroundWindow = GetForegroundWindow();

	if (target != foregroundWindow && hw != foregroundWindow)
	{
		MoveWindow(hw, 0, 0, 0, 0, true);
		return;
	}

	RECT rect;
	if (!GetWindowRect(target, &rect))
	{
		utils::output::error("Failed to get target window rect.\n");
		return;
	}

	int rsize_x = rect.right - rect.left;
	int rsize_y = rect.bottom - rect.top;

	if (full_screen(target))
	{
		rsize_x += 16;
	}
	else
	{
		rsize_y -= 39;
		rect.left += 4;
		rect.top += 31;
	}

	if (!MoveWindow(hw, rect.left, rect.top, rsize_x, rsize_y, TRUE))
	{
		std::cerr << "failed to move window." << std::endl;
	}
}

bool overlay::create_device_d3d(HWND hw) {
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hw;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL selected_level;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, feature_levels, _countof(feature_levels), D3D11_SDK_VERSION, &sd, &dxgiSwapChain, &d3d11Device, &selected_level, &d3d11DeviceContext);

	if (FAILED(hr)) {
		if (hr == DXGI_ERROR_UNSUPPORTED) {
			hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, feature_levels, _countof(feature_levels), D3D11_SDK_VERSION, &sd, &dxgiSwapChain, &d3d11Device, &selected_level, &d3d11DeviceContext);
		}
	}

	create_render_target();
	return true;
}

void overlay::cleanup_device_d3d()
{
	cleanup_render_target();

	if (dxgiSwapChain)
	{
		dxgiSwapChain->Release();
		dxgiSwapChain = nullptr;
	}

	if (d3d11DeviceContext)
	{
		d3d11DeviceContext->Release();
		d3d11DeviceContext = nullptr;
	}

	if (d3d11Device)
	{
		d3d11Device->Release();
		d3d11Device = nullptr;
	}
}

void overlay::create_render_target()
{
	ID3D11Texture2D* back_buffer = nullptr;
	if (SUCCEEDED(dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&back_buffer))))
	{
		d3d11Device->CreateRenderTargetView(back_buffer, nullptr, &d3d11RenderTargetView);
		back_buffer->Release();
	}
	else
	{
		std::cerr << "failed to get back buffer" << std::endl;
	}
}

void overlay::cleanup_render_target()
{
	if (d3d11RenderTargetView)
	{
		d3d11RenderTargetView->Release();
		d3d11RenderTargetView = nullptr;
	}
}

LRESULT __stdcall overlay::window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (d3d11Device != nullptr && wParam != SIZE_MINIMIZED)
		{
			cleanup_render_target();
			dxgiSwapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			create_render_target();
		}
		return 0;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}