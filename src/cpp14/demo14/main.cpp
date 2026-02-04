#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

// 命名空间简化代码，demo场景适用
using namespace std;

auto create_int_vec() {
    return vector<int>{1, 2, 3, 4, 5};
}

auto create_complex_map() {
    return vector<pair<string, int>>{{"C++11", 2011}, {"C++14", 2014}, {"C++17", 2017}};
}

// ==============================================
// 特性1：Lambda三大增强（泛型+初始化捕获+自动返回值推导）
// ==============================================
void test_lambda_enhance() {
    cout << "===== 1. C++14 Lambda表达式增强 =====" << endl;

    // 1.1 泛型Lambda：auto形参支持任意类型
    auto add = [](auto a, auto b) { return a + b; };
    cout << "泛型Lambda-Int相加: " << add(10, 25) << endl;
    cout << "泛型Lambda-字符串拼接: " << add(string("C++"), string("14")) << endl;
    cout << "泛型Lambda-浮点数相加: " << add(3.14, 2.86) << endl;

    // 1.2 初始化捕获+智能指针：自动管理内存，无泄漏
    int base = 100;
    auto calc = [p = make_unique<int>(base), mul = 2](int x) {
        *p *= mul;
        return *p + x;
    };
    cout << "初始化捕获-计算结果: " << calc(50) << endl;

    // 1.3 多return自动推导返回值
    auto judge_num = [](int x) {
        if (x > 0)
            return 1;
        else if (x == 0)
            return 0;
        else
            return -1;
    };
    cout << "多return自动推导-正数判断: " << judge_num(10) << endl;
    cout << "多return自动推导-负数判断: " << judge_num(-5) << endl;
    cout << endl;
}

// ==============================================
// 特性2：类型推导增强（auto返回值+decltype(auto)精准推导）
// ==============================================
void test_type_deduce_enhance() {
    cout << "===== 2. C++14 类型推导增强（auto+decltype(auto)） =====" << endl;

    // 2.1 普通函数auto返回值推导
    auto vec = create_int_vec();
    cout << "函数返回值auto推导-vector内容: ";
    for (int n : vec) cout << n << " ";
    cout << endl;

    // 2.2 复杂类型auto返回值推导
    auto complex_map = create_complex_map();
    cout << "复杂类型auto推导-键值对: ";
    for (auto& p : complex_map) cout << p.first << ":" << p.second << " ";
    cout << endl;

    // 2.3 decltype(auto)保留原始类型（引用/值）
    int num = 100;
    int& ref_num = num;
    auto a = ref_num;            // auto：值拷贝，推导为int
    decltype(auto) b = ref_num;  // decltype(auto)：保留引用，推导为int&
    a = 200;
    cout << "auto推导-修改后原变量num: " << num << endl;
    b = 300;
    cout << "decltype(auto)推导-修改后原变量num: " << num << endl;

    // 2.4 decltype(auto)返回引用/值（Lambda替代局部函数）
    auto get_ref = [&]() -> decltype(auto) { return ref_num; };
    auto get_val = [&]() -> decltype(auto) { return num; };
    get_ref() = 400;
    cout << "decltype(auto)返回引用-修改后num: " << num << endl;
    cout << endl;
}

// ==============================================
// 特性3：数值字面量增强（二进制0b + 数字分隔符'）
// ==============================================
void test_literal_enhance() {
    cout << "===== 3. C++14 数值字面量增强（二进制+数字分隔符） =====" << endl;

    // 二进制字面量
    constexpr int bin1 = 0b1010;
    constexpr int bin2 = 0b110011;
    cout << "二进制0b1010转十进制: " << bin1 << endl;
    cout << "二进制0b110011转十进制: " << bin2 << endl;

    // 数字分隔符（整型/浮点型均支持）
    constexpr long long big_num1 = 1'000'000'000;
    constexpr double pi = 3.1415'9265'3589'7932;
    cout << "数字分隔符-10亿: " << big_num1 << endl;
    cout << "数字分隔符-浮点数π: " << pi << endl;

    // 二进制+分隔符组合
    constexpr int bin3 = 0b1001'1100'0110'1010;
    cout << "二进制分段0b1001'1100'0110'1010转十进制: " << bin3 << endl;
    cout << endl;
}

// ==============================================
// 特性4：标准库增强（make_unique + 泛型STL算法）
// ==============================================
void test_std_lib_enhance() {
    cout << "===== 4. C++14 标准库增强（make_unique+泛型STL） =====" << endl;

    // 4.1 std::make_unique：安全创建unique_ptr，替代裸new（C++14新增）
    class TestObj {
      public:
        TestObj(int val) : m_val(val) { cout << "TestObj构造-值: " << m_val << endl; }
        ~TestObj() { cout << "TestObj析构-值: " << m_val << endl; }
        int get_val() const { return m_val; }

      private:
        int m_val;
    };
    auto p1 = make_unique<TestObj>(10);
    auto p2 = make_unique<vector<int>>(5, 100);
    cout << "make_unique-对象值: " << p1->get_val() << endl;
    cout << "make_unique-vector第一个元素: " << (*p2)[0] << endl;

    // 4.2 泛型Lambda适配STL算法：一个Lambda支持所有类型
    vector<int> int_vec = {5, 2, 9, 1, 5, 6};
    vector<string> str_vec = {"C++14", "Lambda", "STL", "make_unique"};
    auto generic_sort = [](auto& vec) { sort(vec.begin(), vec.end()); };
    generic_sort(int_vec);
    generic_sort(str_vec);

    cout << "泛型Lambda排序-int容器: ";
    for (int n : int_vec) cout << n << " ";
    cout << endl;
    cout << "泛型Lambda排序-string容器: ";
    for (string& s : str_vec) cout << s << " ";
    cout << endl;
    cout << endl;
}

// ==============================================
// 特性5：多线程增强（shared_timed_mutex 读写锁）
// ==============================================
void test_thread_enhance() {
    cout << "===== 5. C++14 多线程增强（shared_timed_mutex读写锁） =====" << endl;

    shared_timed_mutex rw_mutex;
    int shared_data = 100;

    // 读函数：共享锁，多线程同时读
    auto read_func = [&]() {
        shared_lock<shared_timed_mutex> lock(rw_mutex);
        cout << "读线程-获取共享数据: " << shared_data << endl;
    };

    // 写函数：独占锁，阻塞所有读/写
    auto write_func = [&](int new_val) {
        unique_lock<shared_timed_mutex> lock(rw_mutex);
        shared_data = new_val;
        cout << "写线程-修改共享数据为: " << shared_data << endl;
    };

    // 模拟读多写少场景
    read_func();
    read_func();
    read_func();
    write_func(200);
    read_func();
    read_func();
    cout << endl;
}

// ==============================================
// 主函数：统一调用所有C++14特性测试
// ==============================================
int main() {
    cout << "========== C++14 核心特性全量测试（严格遵循C++14标准） ==========" << endl
         << endl;

    test_lambda_enhance();
    test_type_deduce_enhance();
    test_literal_enhance();
    test_std_lib_enhance();
    test_thread_enhance();

    cout << "========== C++14 所有特性测试完成 ==========" << endl;
    return 0;
}