#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
class User {
   private:
    std::string name;
    int age;

   public:
    User(/* args */);
    ~User();
};

User::User(/* args */) {}

User::~User() {}

int vector_demo() {
    std::vector<int> nums = {10, 21, 42, 43, 74, 10, 21, 42, 43, 74, 10, 21, 42,
                             43, 74, 10, 21, 42, 43, 74, 10, 21, 42, 43, 74};
    nums.push_back(10);

    std::sort(nums.begin(), nums.end());
    for (int num : nums) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}
