// C++17 demo: structured bindings and filesystem mention
#include <iostream>
#include <tuple>
int main() {
    std::tuple<int,int> t{1,2};
    auto [a,b] = t;
    std::cout << "cpp17 demo: a=" << a << " b=" << b << "\n";
    return 0;
}
