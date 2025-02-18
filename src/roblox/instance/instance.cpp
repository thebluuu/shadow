#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include "../rbx_engine.h"
#include "../../misc/output/output.hpp"
#include "../../driver/driver_impl.hpp"
#include "../offsets/Offsets.hpp"
#include "../../misc/globals/globals.hpp"
#include "../../Graphics/overlay/overlay.hpp"

namespace RBX {

    std::string Instance::GetName() const {
        return driver::ReadString(driver::read<std::uint64_t>(this->address + Offsets::Name));
    }

    std::string Instance::GetClass() const {
        std::uint64_t classAddress = driver::read<std::uint64_t>(this->address + Offsets::Class_Name);
        return driver::ReadString(driver::read<std::uint64_t>(classAddress + Offsets::Size));
    }

    std::vector<Instance> Instance::GetChildren(bool isHumanoid) const {
        std::vector<Instance> children;
        if (!this->address) return children;

        std::uint64_t start = driver::read<std::uint64_t>(this->address + Offsets::Children);
        std::uint64_t end = driver::read<std::uint64_t>(start + Offsets::Size);
        if (!start || !end) return children;

        for (auto instance = driver::read<std::uint64_t>(start); instance != end; instance += 16u) {
            children.emplace_back(driver::read<Instance>(instance));
        }
        return children;
    }

    Instance Instance::FindFirstChild(std::string childName, bool isCharChild, std::vector<Instance> loadedChildren) const {
        auto children = this->GetChildren(isCharChild);
        for (const auto& child : children) {
            if (child.GetName() == childName) return child;
        }
        return Instance();
    }

    bool Instance::IsA(const std::string& className) const {
        return this->GetClass() == className;
    }

    bool Instance::IsDescendantOf(const Instance& ancestor) const {
        Instance current = *this;
        while (current.address) {
            if (current.address == ancestor.address) return true;
            current = current.GetParent();
        }
        return false;
    }

    std::vector<Instance> Instance::GetDescendants() const {
        std::vector<Instance> descendants;
        std::vector<Instance> stack = this->GetChildren(false);

        while (!stack.empty()) {
            Instance instance = stack.back();
            stack.pop_back();
            descendants.push_back(instance);
            auto children = instance.GetChildren(false);
            stack.insert(stack.end(), children.begin(), children.end());
        }

        return descendants;
    }

    Instance Instance::FindFirstDescendant(const std::string& name) const {
        auto descendants = this->GetDescendants();
        for (const auto& descendant : descendants) {
            if (descendant.GetName() == name) return descendant;
        }
        return Instance();
    }

    Instance Instance::GetService(std::string serviceName) {
        for (const auto& child : this->GetChildren(false)) {
            if (child.GetClass() == serviceName) return child;
        }
        return Instance();
    }

    bool Instance::SetProperty(const std::string& propertyName, std::uint64_t value) {
        std::uint64_t propertyAddr = this->address + globals::game.FindFirstChild(propertyName, false, {}).address;
        if (!propertyAddr) return false;
        driver::write<std::uint64_t>(propertyAddr, value);
        return true;
    }

    std::uint64_t Instance::GetProperty(const std::string& propertyName) const {
        std::uint64_t propertyAddr = this->address + globals::game.FindFirstChild(propertyName, false, {}).address;
        if (!propertyAddr) return 0;
        return driver::read<std::uint64_t>(propertyAddr);
    }

    bool Instance::Destroy() {
        if (!this->address) return false;
        driver::write<std::uint64_t>(this->address + Offsets::Parent, 0);
        return true;
    }

    bool Instance::Clone(Instance& newInstance) const {
        if (!this->address) return false;
        newInstance = driver::read<Instance>(this->address);
        return newInstance.address != 0;
    }

    bool Instance::MoveTo(Instance& parent) {
        if (!this->address || !parent.address) return false;
        driver::write<std::uint64_t>(this->address + Offsets::Parent, parent.address);
        return true;
    }
    Instance Instance::GetParent() {
        return driver::read<Instance>(this->address + Offsets::Parent);
    }
    std::vector<Instance> Instance::FindAllChildrenByName(const std::string& childName, bool isCharChild) const {
        std::vector<Instance> matchingInstances;
        auto children = this->GetChildren(isCharChild);
        for (const auto& child : children) {
            if (child.GetName() == childName) matchingInstances.push_back(child);
        }
        return matchingInstances;
    }

    std::vector<Instance> Instance::FindAllDescendantsByName(const std::string& name) const {
        std::vector<Instance> matchingInstances;
        auto descendants = this->GetDescendants();
        for (const auto& descendant : descendants) {
            if (descendant.GetName() == name) matchingInstances.push_back(descendant);
        }
        return matchingInstances;
    }

}