#include "../../rbx_engine.h"
#include "../../../driver/driver_impl.hpp"
#include "../../offsets/offsets.hpp"
void RBX::Instance::SetBoolFromValue(bool value) const
{
    driver::write<bool>(this->address + Offsets::Value, value);
}
bool RBX::Instance::getBoolFromValue() const
{
   return  driver::read<std::uint8_t>(this->address + Offsets::Value);
}