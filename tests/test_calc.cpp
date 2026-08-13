#include <gtest/gtest.h>
#include "calc.hpp"

namespace test {

TEST(CalculatorTest, BasicOperations) {
    Calculator calc(10.0f, 5.0f);
    float res = 0.0f;

    // Addition
    EXPECT_EQ(calc.add(), Status::E_OK);
    EXPECT_EQ(calc.getResult(&res), Status::E_OK);
    EXPECT_FLOAT_EQ(res, 15.0f);

    // Subtraction
    EXPECT_EQ(calc.subtract(), Status::E_OK);
    EXPECT_EQ(calc.getResult(&res), Status::E_OK);
    EXPECT_FLOAT_EQ(res, 5.0f);

    // Multiplication
    EXPECT_EQ(calc.multiply(), Status::E_OK);
    EXPECT_EQ(calc.getResult(&res), Status::E_OK);
    EXPECT_FLOAT_EQ(res, 50.0f);

    // Division
    EXPECT_EQ(calc.divide(), Status::E_OK);
    EXPECT_EQ(calc.getResult(&res), Status::E_OK);
    EXPECT_FLOAT_EQ(res, 2.0f);
}

TEST(CalculatorTest, SetOperandsAndNegativeNumbers) {
    Calculator calc(0.0f, 0.0f);
    float res = 0.0f;

    EXPECT_EQ(calc.setOperands(-6.0f, 2.5f), Status::E_OK);

    EXPECT_EQ(calc.add(), Status::E_OK);
    EXPECT_EQ(calc.getResult(&res), Status::E_OK);
    EXPECT_FLOAT_EQ(res, -3.5f);

    EXPECT_EQ(calc.multiply(), Status::E_OK);
    EXPECT_EQ(calc.getResult(&res), Status::E_OK);
    EXPECT_FLOAT_EQ(res, -15.0f);
}

TEST(CalculatorTest, DivisionByZero) {
    Calculator calc(10.0f, 0.0f);
    EXPECT_EQ(calc.divide(), Status::E_NOK);
}

}  // namespace test
