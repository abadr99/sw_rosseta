#pragma once

#include <cstdint>
#include "std_types.hpp"

namespace test
{
class Calculator
{
public:
    Calculator(float operand1, float operand2);

    Status add();
    Status subtract();
    Status multiply();
    Status divide();

    Status setOperands(float operand1, float operand2);
    Status getResult(float* res) const;

private:
    float a{};
    float b{};
    float result{};
};
}  // namespace tes