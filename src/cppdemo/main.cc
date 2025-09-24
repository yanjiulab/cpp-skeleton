#include "main.h"

#include <iostream>
#include <memory>

class user {
  private:
    /* data */
  public:
    int age;
    user() {
        age = 47;
        printf("%p: 触发默认构造参数 (age=%d)\n", this, this->age);
    };
    user(int a) : age(a) {
        printf("%p: 触发参数构造参数 (age=%d)\n", this, this->age);
    };

    user(const user&) {
        printf("%p: 触发复制构造参数 (age=%d)\n", this, this->age);
    }
    user& operator=(const user&) {
        printf("%p: 触发复制赋值运算符 (age=%d)\n", this, this->age);
    }
    user(user&& o) {
        printf("%p: 触发移动构造参数 (age=%d)\n", this, this->age);
    }
    user& operator=(user&&) {
        printf("%p: 触发移动赋值运算符 (age=%d)\n", this, this->age);
    }

    // user(const user&) = delete;
    // user& operator=(const user&) = delete;
    // user(user&& o) = delete;
    // user& operator=(user&&) = delete;
    ~user();
};

user::~user() {
}

std::unique_ptr<user> create() {
    auto u = std::make_unique<user>();
    u->age = 10;
    // u.age = 10;
    printf("%p: %d\n", u.get(), u->age);
    return u;
}

void oo_demo() {
    auto u1 = create();
    u1->age = 20;
    printf("%p: %d\n", u1.get(), u1->age);

    auto u2 = std::move(u1);
    // printf("%p: %d\n", &u, u.age);
    printf("%p: %d\n", u2.get(), u2->age);

    user u3;
    printf("u3: %p: %d\n", &u3, u3.age);
    user u4{19};
    printf("u4: %p: %d\n", &u4, u4.age);
    user u5 = u4;  // copy constractor = delete error
    user u6;
    u6 = u4;                        // operator= = delte error
    user u7 = std::move(user{10});  // function "user::user(user &&o)" is a deleted function
    user u8;
    u8 = std::move(user{10});  // function "user::operator=(user &&)" is a deleted function
}

int main(int argc, char* argv[]) {
    // user u1;

    // spdlog_demo();
    // vector_demo();
    // json_demo();
    // asio_20_echo_demo();
    asio_20_chat_demo(argc, argv);

    return 0;
}