#pragma once

#include "./std_types.hpp"

namespace test {

class Calculator {
 public:
    Calculator(float operand1, float operand2);

    Status add();
    Status subtract();
    Status multiply();
    Status divide();

    Status setOperands(float operand1, float operand2);
    Status getResult(float* res) const;

 private:
    float a{0.0f};
    float b{0.0f};
    float result{0.0f};
};

}  // namespace test
