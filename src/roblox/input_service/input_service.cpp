#include <windows.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <filesystem>
#include <fstream>
#include <wininet.h>
#include <ShlObj.h>
#include "../rbx_engine.h"
#include "../../misc/output/output.hpp"
#include "../../driver/driver_impl.hpp"
#include "../offsets/Offsets.hpp"
#include "../../misc/globals/globals.hpp"
#include "../../Graphics/overlay/overlay.hpp"

void RBX::Instance::cache_mouse_service() {
    while (!globals::mouse_service) {
        globals::mouse_service = globals::game.FindFirstChild("MouseService", false, {}).address;

        if (globals::mouse_service) {
            break;
        }
    }

    if (!globals::mouse_service) {
        std::cout << " - failed to cache game.MouseService" << std::endl;
    }
}

std::uint64_t RBX::Instance::cached_input_object = 0;

std::uint64_t RBX::Instance::get_current_input_object(std::uint64_t base_address) {
    return driver::read<std::uint64_t>(base_address + 0xF8);
}

void RBX::Instance::initialize_mouse_service(std::uint64_t address) {
    cached_input_object = get_current_input_object(address);

    if (cached_input_object && cached_input_object != 0xFFFFFFFFFFFFFFFF) {
        const char* base_pointer = reinterpret_cast<const char*>(cached_input_object);

        _mm_prefetch(base_pointer + 0xE4, _MM_HINT_T0);
        _mm_prefetch(base_pointer + 0xE4 + sizeof(RBX::Vector2), _MM_HINT_T0);
    }
}
void RBX::Instance::write_mouse_position(std::uint64_t address, float x, float y) {
    cached_input_object = get_current_input_object(address);

    if (cached_input_object != 0 && cached_input_object != 0xFFFFFFFFFFFFFFFF) {
        RBX::Vector2 new_position = { x, y };

        driver::write<RBX::Vector2>(cached_input_object + 0xE4, new_position);
    }
}