#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <wininet.h>
#include <ShlObj.h>
#include "../../../misc/output/output.hpp"
#include "../../rbx_engine.h"
#include "../../offsets/Offsets.hpp"
#include "../../../driver/driver_impl.hpp"

namespace RBX {

    std::uint64_t Instance::GetRenderView() {
        return driver::read<uintptr_t>(TaskScheduler{}.GetJobByName("RenderJob") + 0x218);
    }

    std::uint64_t Instance::GetCameraMatrix() {
        auto renderView = GetRenderView();
        return renderView == 0 ? 0 : driver::read<uintptr_t>(renderView + 0x58);
    }

    std::pair<int, int> Instance::GetViewportSize() {
        auto renderView = GetRenderView();
        if (renderView == 0) return { 0, 0 };

        return { driver::read<int>(renderView + 0x2D0), driver::read<int>(renderView + 0x2D0) };
    }

    bool Instance::IsRenderViewValid() {
        return GetRenderView() != 0;
    }

    void Instance::SetRenderFeature(std::string featureName, bool enable) {
        auto renderView = GetRenderView();
        if (renderView == 0) return;

        uintptr_t featureAddress = driver::read<uintptr_t>(renderView + 0x1DA);
        if (featureName == "Bloom") {
            driver::write<bool>(featureAddress + 0x14C, enable);
        }
        else if (featureName == "Shadows") {
            driver::write<bool>(featureAddress + 0x2A0, enable);
        }
    }

} // namespace RBX
