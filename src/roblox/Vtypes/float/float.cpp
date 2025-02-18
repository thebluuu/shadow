#include "../../rbx_engine.h"
#include "../../../driver/driver_impl.hpp"
#include "../../offsets/offsets.hpp"
void RBX::Instance::SetFloatValue(float value) const
{
    driver::write<float>(this->address + Offsets::Value, value);
}
float RBX::Instance::GetFloatFromValue() const
{
  return  driver::read<float>(this->address + Offsets::Value);
}