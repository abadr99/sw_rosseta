#include "../inc/calc.hpp"

#include <iostream>

namespace test {

Calculator::Calculator(float operand1, float operand2)
    : a(operand1), b(operand2) {
}

Status Calculator::add() {
    result = a + b;
    return Status::E_OK;
}

Status Calculator::subtract() {
    result = a - b;
    return Status::E_OK;
}

Status Calculator::multiply() {
    result = a * b;
    return Status::E_OK;
}

Status Calculator::divide() {
    if (0.0f == b) {
        std::cout << "Error: Division by zero is not allowed.\n";
        return Status::E_NOK;
    }

    result = a / b;
    return Status::E_OK;
}

Status Calculator::setOperands(float operand1, float operand2) {
    a = operand1;
    b = operand2;
    return Status::E_OK;
}

Status Calculator::getResult(float* res) const {
    if (nullptr == res) {
        return Status::E_NOK;
    }

    *res = result;
    return Status::E_OK;
}

}  // namespace test
