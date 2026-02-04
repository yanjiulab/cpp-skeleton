#include <algorithm>
#include <cstdlib>  // 为rand()补充头文件，避免隐式声明警告
#include <filesystem>
#include <fstream>  // 修复：补充ofstream所需头文件
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

// 命名空间简化，demo场景适用
using namespace std;
namespace fs = std::filesystem;  // 文件系统库命名空间别名

// ==============================================
// 特性1：结构化绑定（C++17核心语法糖，工程最高频）
// ==============================================
void test_structured_binding() {
    cout << "===== 1. C++17 结构化绑定 =====" << endl;

    // 1.1 解包数组
    int arr[] = {10, 20, 30};
    auto [a, b, c] = arr;  // 绑定数组元素，a=10, b=20, c=30
    cout << "解包数组: " << a << ", " << b << ", " << c << endl;

    // 1.2 解包结构体
    struct Person {
        string name;
        int age;
        double score;
    };
    Person p = {"Tom", 20, 95.5};
    auto [name, age, score] = p;  // 直接绑定结构体成员，无需p.name/p.age
    cout << "解包结构体: " << name << ", " << age << ", " << score << endl;

    // 1.3 解包元组（STL高频场景）
    auto tp = make_tuple("C++17", 2017, true);
    auto [lang, year, is_good] = tp;
    cout << "解包元组: " << lang << ", " << year << ", " << (is_good ? "是" : "否") << endl;

    // 1.4 解包STL键值对（map/set遍历高频）
    pair<string, int> pr = {"count", 100};
    auto [key, val] = pr;
    cout << "解包键值对: " << key << " = " << val << endl;

    cout << endl;
}

// ==============================================
// 特性2：constexpr Lambda（C++17重磅优化，编译期Lambda）
// ==============================================
void test_constexpr_lambda() {
    cout << "===== 2. C++17 constexpr Lambda =====" << endl;

    // 2.1 编译期Lambda计算（直接用于constexpr变量初始化）
    constexpr auto add = [](int a, int b) { return a + b; };
    constexpr int sum = add(100, 200);  // 编译期计算，运行期直接用常量300
    cout << "编译期Lambda相加: " << sum << endl;

    // 2.2 编译期Lambda遍历数组（配合constexpr）
    constexpr int nums[] = {1, 2, 3, 4, 5};
    constexpr auto array_sum = [&]() {
        int s = 0;
        for (int n : nums) s += n;
        return s;
    }();
    cout << "编译期Lambda计算数组和: " << array_sum << endl;

    cout << endl;
}

// ==============================================
// 特性3：if/switch初始化（缩小变量作用域，代码更整洁）
// ==============================================
void test_if_switch_init() {
    cout << "===== 3. C++17 if/switch 初始化 =====" << endl;

    // 3.1 if初始化（STL查找/指针判空高频场景）
    vector<int> vec = {1, 3, 5, 7, 9};
    if (auto it = find(vec.begin(), vec.end(), 5); it != vec.end()) {
        // it仅在if块内有效，无需在外层定义
        cout << "if初始化-找到元素: " << *it << endl;
    } else {
        cout << "未找到元素" << endl;
    }

    // 3.2 switch初始化
    switch (int n = rand() % 3; n) {  // 初始化n，仅在switch块内有效
        case 0:
            cout << "switch初始化-随机数: 0" << endl;
            break;
        case 1:
            cout << "switch初始化-随机数: 1" << endl;
            break;
        case 2:
            cout << "switch初始化-随机数: 2" << endl;
            break;
    }

    cout << endl;
}

// ==============================================
// 特性4：std::optional（优雅处理空值，替代裸指针/魔法值）
// ==============================================
void test_optional() {
    cout << "===== 4. C++17 std::optional =====" << endl;

    // 4.1 定义可选值（存在/不存在两种状态）
    optional<int> opt1 = 10;          // 存在值：10
    optional<int> opt2;               // 不存在值：空
    optional<string> opt3 = nullopt;  // 显式置空

    // 4.2 判空+取值
    if (opt1.has_value()) {
        cout << "opt1值: " << opt1.value() << endl;  // 取值：10
    }
    cout << "opt2默认值: " << opt2.value_or(-1) << endl;  // 空值时返回默认值-1

    // 4.3 函数返回optional（工程核心场景：查找可能失败）
    auto find_num = [](const vector<int>& v, int x) -> optional<int> {
        auto it = find(v.begin(), v.end(), x);
        return it != v.end() ? optional<int>(*it) : nullopt;
    };
    vector<int> vec = {2, 4, 6, 8};
    auto res = find_num(vec, 6);
    if (res) {                                 // 直接判空，语法糖
        cout << "查找结果: " << *res << endl;  // 直接解引用，类型安全
    }

    cout << endl;
}

// ==============================================
// 特性5：std::variant（类型安全的联合体，替代C风格union）
// ==============================================
void test_variant() {
    cout << "===== 5. C++17 std::variant =====" << endl;

    // 5.1 定义变体（可存储int/string/double中的一种）
    variant<int, string, double> var;
    var = 100;      // 存储int
    var = "C++17";  // 覆盖为string
    var = 3.14;     // 覆盖为double

    // 5.2 类型判断+取值（get<类型>/get<索引>）
    if (var.index() == 2) {  // 索引：0=int,1=string,2=double
        cout << "var存储double: " << get<double>(var) << endl;
    }
    // 安全取值：如果类型不匹配，不会崩溃，返回nullopt
    if (auto p = get_if<string>(&var); p) {
        cout << "var存储string: " << *p << endl;
    } else {
        cout << "var当前不存储string" << endl;
    }

    // 5.3 遍历变体（配合std::visit，C++17+）
    variant<int, string> var2 = "hello";
    visit([](auto&& val) {  // 泛型Lambda遍历，自动匹配类型
        cout << "std::visit遍历变体: " << val << endl;
    },
          var2);

    cout << endl;
}

// ==============================================
// 特性6：std::string_view（轻量字符串视图，极致性能）
// ==============================================
void test_string_view() {
    cout << "===== 6. C++17 std::string_view =====" << endl;

    // 6.1 构造string_view（无拷贝，直接指向原字符串）
    const char* c_str = "Hello C++17";
    string str = "string to string_view";
    string_view sv1(c_str);
    string_view sv2(str);
    string_view sv3 = "direct init";  // 直接初始化，无拷贝

    cout << "sv1: " << sv1 << ", 长度: " << sv1.size() << endl;
    cout << "sv2子串: " << sv2.substr(0, 6) << endl;  // 切分无拷贝

    // 6.2 函数传参（替代const string&，避免临时对象拷贝）
    auto print_sv = [](string_view sv) {
        cout << "字符串视图传参: " << sv << endl;
    };
    print_sv("no copy");  // 直接传字面量，无临时string创建
    print_sv(str);        // 传string，自动转换为string_view，无拷贝

    // 6.3 拼接（需转string，因为string_view是只读的）
    string res = string(sv1) + " | " + string(sv3);
    cout << "拼接结果: " << res << endl;

    cout << endl;
}

// ==============================================
// 特性7：折叠表达式（简化模板参数包，替代递归模板，工程高频）
// ==============================================
template <typename... T>
auto sum_all(T... args) {
    return (args + ...);  // 二元左折叠：((a1+a2)+a3)+...+an
}

template <typename... T>
bool all_true(T... args) {
    return (args && ...);  // 折叠与运算：所有参数为true才返回true
}

// 修复：将原Lambda改为模板函数，支持C++17参数包
template <typename... Args>
string concat(Args&&... args) {
    string res;
    (res.append(std::forward<Args>(args)), ...);  // 逗号折叠，依次追加
    return res;
}

void test_fold_expression() {
    cout << "===== 7. C++17 折叠表达式 =====" << endl;

    // 7.1 数值求和（替代递归模板，一行代码）
    int s1 = sum_all(1, 2, 3, 4, 5);
    double s2 = sum_all(1.1, 2.2, 3.3);
    cout << "折叠求和-Int: " << s1 << ", Double: " << s2 << endl;

    // 7.2 布尔与运算（判断所有参数是否为true）
    bool b1 = all_true(true, true, 1, !0);
    bool b2 = all_true(true, false, 1);
    cout << "所有参数为true: " << (b1 ? "是" : "否") << endl;
    cout << "所有参数为true: " << (b2 ? "是" : "否") << endl;

    // 7.3 字符串拼接（折叠表达式+string_view）
    string str = concat(string_view("C++"), string_view("17"), string_view(" "),
                        string_view("fold"), string_view(" "), string_view("expression"));
    cout << "折叠拼接字符串: " << str << endl;

    cout << endl;
}

// ==============================================
// 特性8：std::filesystem（跨平台文件系统库，替代boost/filesystem）
// ==============================================
void test_filesystem() {
    cout << "===== 8. C++17 std::filesystem 跨平台文件库 =====" << endl;

    // 8.1 获取当前工作目录
    fs::path cwd = fs::current_path();
    cout << "当前工作目录: " << cwd << endl;

    // 8.2 创建目录（多级目录）
    fs::path test_dir = cwd / "cpp17_test_dir";
    if (fs::create_directories(test_dir)) {
        cout << "创建目录成功: " << test_dir << endl;
    }

    // 8.3 创建文件并判断属性
    fs::path test_file = test_dir / "test.txt";
    ofstream f(test_file);  // 现在正常：已补充<fstream>头文件
    f << "C++17 filesystem";
    f.close();
    cout << "文件是否存在: " << boolalpha << fs::exists(test_file) << endl;
    cout << "是否为普通文件: " << fs::is_regular_file(test_file) << endl;
    cout << "文件大小(字节): " << fs::file_size(test_file) << endl;

    // 8.4 遍历目录
    cout << "遍历目录 " << test_dir << ": ";
    for (const auto& entry : fs::directory_iterator(test_dir)) {
        cout << entry.path().filename() << " ";
    }
    cout << endl;

    // 8.5 删除文件+目录
    fs::remove(test_file);
    fs::remove(test_dir);
    cout << "删除文件/目录后是否存在: " << !fs::exists(test_dir) << endl;

    cout << endl;
}

// ==============================================
// 特性9：std::apply（解包元组为函数参数，配合tuple高频使用）
// ==============================================
void test_apply() {
    cout << "===== 9. C++17 std::apply 解包元组 =====" << endl;

    // 9.1 普通函数+元组解包
    auto calc = [](int a, int b, int c) {
        return a * 100 + b * 10 + c;
    };
    auto tp = make_tuple(1, 2, 3);
    int res1 = apply(calc, tp);  // 解包tp为calc的3个参数：1,2,3
    cout << "解包元组调用函数: " << res1 << endl;

    // 9.2 成员函数+元组解包
    struct Data {
        int sum(int a, int b) { return a + b; }
        string concat(string s1, string s2) { return s1 + s2; }
    };
    Data d;
    auto tp2 = make_tuple("C++", "17");
    // 修复：用tuple_cat平铺对象指针和参数元组，避免嵌套
    string res2 = apply(&Data::concat, tuple_cat(make_tuple(&d), tp2));
    cout << "解包元组调用成员函数: " << res2 << endl;

    // 9.3 结合STL算法+apply
    auto tp3 = make_tuple(vector<int>{1, 2, 3}, 0);
    int res3 = apply([](auto& v, int init) {
        return accumulate(v.begin(), v.end(), init);
    },
                     tp3);
    cout << "解包元组调用Lambda: " << res3 << endl;

    cout << endl;
}

// ==============================================
// 主函数：统一调用所有C++17高频特性测试
// ==============================================
int main() {
    srand(time(nullptr));  // 初始化随机数种子，让rand()结果更合理
    cout << "========== C++17 高频核心特性全量测试 ==========" << endl
         << endl;

    // 按工程实用度排序调用
    test_structured_binding();
    test_constexpr_lambda();
    test_if_switch_init();
    test_optional();
    test_variant();
    test_string_view();
    test_fold_expression();
    test_filesystem();
    test_apply();

    cout << "========== C++17 所有高频特性测试完成 ==========" << endl;
    return 0;
}