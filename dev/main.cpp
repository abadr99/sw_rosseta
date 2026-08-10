#include <iostream>

#include "./inc/calc.hpp"

int main() {
    test::Calculator calc(5.0f, 10.0f);

    calc.add();
    float res = 0.0f;
    calc.getResult(&res);
    std::cout << "5 + 10 = " << res << "\n";

    calc.setOperands(5.0f, 10.0f);
    calc.divide();
    calc.getResult(&res);
    std::cout << "5 / 10 = " << res << "\n";

    return 0;
}
