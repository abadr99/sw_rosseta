#include "../inc/calc.hpp"
#include <iostream>

int main()
{
    test::Calculator calc(5, 10);

    calc.add();
    float result{};
    calc.getResult(&result);
    std::cout << "5 + 10 = " << result << '\n';

    calc.divide();
    calc.getResult(&result);
    std::cout << "5 / 10 = " << result << '\n';

    return 0;
}