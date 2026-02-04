// C++14 demo: generic lambdas and constexpr improvements
#include <iostream>
int main() {
    auto add = [](auto a, auto b) { return a + b; };
    std::cout << "cpp14 demo: 2 + 3 = " << add(2,3) << "\n";
    return 0;
}
