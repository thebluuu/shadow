#include "../../rbx_engine.h"
#include "../../../driver/driver_impl.hpp"
#include "../../offsets/offsets.hpp"
RBX::Vector3 RBX::Instance::getVec3FromValue() const
{
    driver::read<RBX::Vector3>(this->address + Offsets::Value);
}
void RBX::Instance::SetVec3FromValue(RBX::Vector3 Value)
{
    driver::write<RBX::Vector3>(this->address + Offsets::Value, Value);
}