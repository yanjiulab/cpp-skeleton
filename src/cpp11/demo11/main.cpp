#include <algorithm>  // 用于find等算法
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

// 演示string的使用
void demo_string() {
    cout << "\n=== 演示 string 容器 ===" << endl;

    // 初始化
    string str = "Hello";

    // 增加
    str += " World";
    str.append("!");
    cout << "添加后: " << str << endl;

    // 修改
    str[0] = 'h';
    cout << "修改后: " << str << endl;

    // 查找
    size_t pos = str.find("World");
    if (pos != string::npos) {
        cout << "找到 'World' 在位置: " << pos << endl;
    }

    // 删除
    str.erase(pos, 5);  // 从pos位置删除5个字符
    cout << "删除后: " << str << endl;

    // 迭代器遍历
    cout << "遍历字符串: ";
    for (string::iterator it = str.begin(); it != str.end(); ++it) {
        cout << *it;
    }
    cout << endl;

    // C++11 范围for循环
    cout << "范围for循环: ";
    for (char c : str) {
        cout << c;
    }
    cout << endl;
}

// 演示vector的使用
void demo_vector() {
    cout << "\n=== 演示 vector 容器 ===" << endl;

    // 初始化
    vector<int> vec = {1, 2, 3};

    // 增加
    vec.push_back(4);
    vec.insert(vec.begin() + 2, 10);  // 在索引2处插入10
    cout << "添加后大小: " << vec.size() << endl;

    // 修改
    vec[0] = 100;
    cout << "修改第一个元素后: " << vec[0] << endl;

    // 查找
    auto it = find(vec.begin(), vec.end(), 3);
    if (it != vec.end()) {
        cout << "找到元素 3 在位置: " << distance(vec.begin(), it) << endl;
    }

    // 删除
    vec.pop_back();              // 删除最后一个元素
    vec.erase(vec.begin() + 1);  // 删除索引1处的元素
    cout << "删除后大小: " << vec.size() << endl;

    // 迭代器遍历
    cout << "遍历vector: ";
    for (vector<int>::iterator iter = vec.begin(); iter != vec.end(); ++iter) {
        cout << *iter << " ";
    }
    cout << endl;
}

// 演示list的使用
void demo_list() {
    cout << "\n=== 演示 list 容器 ===" << endl;

    // 初始化
    list<int> lst = {1, 2, 3};

    // 增加
    lst.push_back(4);
    lst.push_front(0);
    lst.insert(next(lst.begin(), 2), 10);  // 在第二个元素后插入10
    cout << "添加后大小: " << lst.size() << endl;

    // 修改
    *lst.begin() = 100;
    cout << "修改第一个元素后: " << *lst.begin() << endl;

    // 查找
    auto it = find(lst.begin(), lst.end(), 3);
    if (it != lst.end()) {
        cout << "找到元素 3" << endl;
    }

    // 删除
    lst.pop_back();
    lst.pop_front();
    lst.erase(it);  // 删除找到的元素3
    cout << "删除后大小: " << lst.size() << endl;

    // 迭代器遍历
    cout << "遍历list: ";
    for (list<int>::iterator iter = lst.begin(); iter != lst.end(); ++iter) {
        cout << *iter << " ";
    }
    cout << endl;
}

// 演示map的使用
void demo_map() {
    cout << "\n=== 演示 map 容器 ===" << endl;

    // 初始化
    map<string, int> mp;

    // 增加
    mp["one"] = 1;
    mp.insert(make_pair("two", 2));
    mp.emplace("three", 3);  // C++11新特性
    cout << "添加后大小: " << mp.size() << endl;

    // 修改
    mp["one"] = 100;
    cout << "修改后 one 的值: " << mp["one"] << endl;

    // 查找
    auto it = mp.find("two");
    if (it != mp.end()) {
        cout << "找到 two: " << it->second << endl;
    }

    // 删除
    mp.erase("two");
    cout << "删除后大小: " << mp.size() << endl;

    // 迭代器遍历
    cout << "遍历map: ";
    for (map<string, int>::iterator iter = mp.begin(); iter != mp.end(); ++iter) {
        cout << iter->first << ":" << iter->second << " ";
    }
    cout << endl;
}

// 演示set的使用
void demo_set() {
    cout << "\n=== 演示 set 容器 ===" << endl;

    // 初始化
    set<int> st = {3, 1, 2};

    // 增加
    st.insert(4);
    st.insert(2);  // 插入重复元素，不会生效
    cout << "添加后大小: " << st.size() << endl;

    // 查找
    auto it = st.find(2);
    if (it != st.end()) {
        cout << "找到元素 2" << endl;
    }

    // 删除
    st.erase(3);
    cout << "删除后大小: " << st.size() << endl;

    // 迭代器遍历（自动排序）
    cout << "遍历set: ";
    for (set<int>::iterator iter = st.begin(); iter != st.end(); ++iter) {
        cout << *iter << " ";
    }
    cout << endl;
}

// 演示unordered_map的使用
void demo_unordered_map() {
    cout << "\n=== 演示 unordered_map 容器 ===" << endl;

    // 初始化
    unordered_map<string, int> umap;

    // 增加
    umap["one"] = 1;
    umap.insert(make_pair("two", 2));
    umap.emplace("three", 3);
    cout << "添加后大小: " << umap.size() << endl;

    // 修改
    umap["one"] = 100;
    cout << "修改后 one 的值: " << umap["one"] << endl;

    // 查找
    auto it = umap.find("two");
    if (it != umap.end()) {
        cout << "找到 two: " << it->second << endl;
    }

    // 删除
    umap.erase("two");
    cout << "删除后大小: " << umap.size() << endl;

    // 迭代器遍历（无序）
    cout << "遍历unordered_map: ";
    for (unordered_map<string, int>::iterator iter = umap.begin(); iter != umap.end(); ++iter) {
        cout << iter->first << ":" << iter->second << " ";
    }
    cout << endl;
}

// 演示unordered_set的使用
void demo_unordered_set() {
    cout << "\n=== 演示 unordered_set 容器 ===" << endl;

    // 初始化
    unordered_set<int> ust = {3, 1, 2};

    // 增加
    ust.insert(4);
    ust.insert(2);  // 插入重复元素，不会生效
    cout << "添加后大小: " << ust.size() << endl;

    // 查找
    auto it = ust.find(2);
    if (it != ust.end()) {
        cout << "找到元素 2" << endl;
    }

    // 删除
    ust.erase(3);
    cout << "删除后大小: " << ust.size() << endl;

    // 迭代器遍历（无序）
    cout << "遍历unordered_set: ";
    for (unordered_set<int>::iterator iter = ust.begin(); iter != ust.end(); ++iter) {
        cout << *iter << " ";
    }
    cout << endl;
}

// 演示stack的使用
void demo_stack() {
    cout << "\n=== 演示 stack 容器 ===" << endl;

    // 初始化
    stack<int> stk;

    // 增加
    stk.push(1);
    stk.push(2);
    stk.push(3);
    cout << "栈顶元素: " << stk.top() << endl;
    cout << "栈大小: " << stk.size() << endl;

    // 修改栈顶元素
    stk.top() = 100;
    cout << "修改后栈顶元素: " << stk.top() << endl;

    // 删除（栈没有查找操作，只能访问栈顶）
    stk.pop();
    cout << "弹出后栈顶元素: " << stk.top() << endl;
    cout << "弹出后栈大小: " << stk.size() << endl;

    // 遍历（需要弹出所有元素）
    cout << "遍历stack（会清空栈）: ";
    while (!stk.empty()) {
        cout << stk.top() << " ";
        stk.pop();
    }
    cout << endl;
}

// 演示queue的使用
void demo_queue() {
    cout << "\n=== 演示 queue 容器 ===" << endl;

    // 初始化
    queue<int> q;

    // 增加
    q.push(1);
    q.push(2);
    q.push(3);
    cout << "队首元素: " << q.front() << endl;
    cout << "队尾元素: " << q.back() << endl;
    cout << "队列大小: " << q.size() << endl;

    // 修改队首和队尾元素
    q.front() = 100;
    q.back() = 300;
    cout << "修改后队首元素: " << q.front() << endl;
    cout << "修改后队尾元素: " << q.back() << endl;

    // 删除（队列没有查找操作，只能访问队首和队尾）
    q.pop();
    cout << "弹出后队首元素: " << q.front() << endl;
    cout << "弹出后队列大小: " << q.size() << endl;

    // 遍历（需要弹出所有元素）
    cout << "遍历queue（会清空队列）: ";
    while (!q.empty()) {
        cout << q.front() << " ";
        q.pop();
    }
    cout << endl;
}

// 演示priority_queue的使用
void demo_priority_queue() {
    cout << "\n=== 演示 priority_queue 容器 ===" << endl;

    // 初始化（默认是最大堆）
    priority_queue<int> pq;

    // 增加
    pq.push(3);
    pq.push(1);
    pq.push(2);
    cout << "优先级最高的元素: " << pq.top() << endl;  // 最大的元素
    cout << "队列大小: " << pq.size() << endl;

    // 修改队首元素（需要先弹出再插入）
    pq.pop();
    pq.push(100);
    cout << "修改后优先级最高的元素: " << pq.top() << endl;

    // 删除（优先队列没有查找操作，只能访问优先级最高的元素）
    pq.pop();
    cout << "弹出后优先级最高的元素: " << pq.top() << endl;
    cout << "弹出后队列大小: " << pq.size() << endl;

    // 遍历（需要弹出所有元素）
    cout << "遍历priority_queue（会清空队列）: ";
    while (!pq.empty()) {
        cout << pq.top() << " ";
        pq.pop();
    }
    cout << endl;
}

class User {
 public:
    // 默认构造函数
    User() : name_(""), age_(0) {}

    // 带参数的构造函数
    // 参数：
    //   name - 用户姓名
    //   age - 用户年龄
    User(const std::string& name, int age) : name_(name), age_(age) {}

    // 获取用户姓名
    const std::string& name() const { return name_; }

    // 设置用户姓名
    // 参数：
    //   name - 新的用户姓名
    void set_name(const std::string& name) { name_ = name; }

    // 获取用户年龄
    int age() const { return age_; }

    // 设置用户年龄
    // 参数：
    //   age - 新的用户年龄
    void set_age(int age) { age_ = age; }

    // 重载输出运算符，用于打印用户信息
    friend std::ostream& operator<<(std::ostream& os, const User& user) {
        os << user.name_ << "(" << user.age_ << ")";
        return os;
    }

    // 重载相等运算符，用于比较两个用户是否相同
    bool operator==(const User& other) const {
        return (name_ == other.name_) && (age_ == other.age_);
    }

    // 重载小于运算符，用于map等容器的排序
    bool operator<(const User& other) const {
        if (name_ != other.name_) {
            return name_ < other.name_;
        }
        return age_ < other.age_;
    }

 private:
    std::string name_;  // 用户姓名
    int age_;           // 用户年龄
};

// 演示存储User对象的vector
void demo_vector_user() {
    cout << "\n=== 演示存储User对象的 vector ===" << endl;

    // 初始化
    vector<User> users;
    users.emplace_back("Alice", 25);
    users.emplace_back("Bob", 30);

    // 增加元素
    users.push_back(User("Charlie", 35));
    users.insert(users.begin() + 1, User("David", 28));

    cout << "添加后大小: " << users.size() << endl;
    cout << "所有用户: ";
    for (const auto& user : users) {
        cout << user << " ";
    }
    cout << endl;

    // 修改元素
    users[0].set_age(26);
    users[0].set_name("Alice Smith");
    cout << "修改第一个用户后: " << users[0] << endl;

    // 查找元素
    auto it = find(users.begin(), users.end(), User("Bob", 30));
    if (it != users.end()) {
        cout << "找到用户: " << *it << " 在位置: " << distance(users.begin(), it) << endl;
    }

    // 删除元素
    users.erase(users.begin() + 2);  // 删除索引2处的元素
    cout << "删除后大小: " << users.size() << endl;

    // 迭代器遍历
    cout << "遍历vector中的用户: ";
    for (vector<User>::iterator iter = users.begin(); iter != users.end(); ++iter) {
        cout << *iter << " ";
    }
    cout << endl;
}

// 演示存储User对象的list
void demo_list_user() {
    cout << "\n=== 演示存储User对象的 list ===" << endl;

    // 初始化
    list<User> users;
    users.emplace_back("Alice", 25);
    users.emplace_back("Bob", 30);

    // 增加元素
    users.push_back(User("Charlie", 35));
    users.push_front(User("David", 28));

    cout << "添加后大小: " << users.size() << endl;
    cout << "所有用户: ";
    for (const auto& user : users) {
        cout << user << " ";
    }
    cout << endl;

    // 修改元素
    users.front().set_age(29);
    cout << "修改第一个用户后: " << users.front() << endl;

    // 查找元素
    auto it = find(users.begin(), users.end(), User("Bob", 30));
    if (it != users.end()) {
        cout << "找到用户: " << *it << endl;
    }

    // 删除元素
    users.erase(it);  // 删除找到的元素
    cout << "删除后大小: " << users.size() << endl;

    // 迭代器遍历
    cout << "遍历list中的用户: ";
    for (list<User>::iterator iter = users.begin(); iter != users.end(); ++iter) {
        cout << *iter << " ";
    }
    cout << endl;
}

// 演示以name为键、User为值的map
void demo_map_name_key() {
    cout << "\n=== 演示以name为键、User为值的 map ===" << endl;

    // 初始化 - 以name为键，以User对象为值
    map<string, User> nameToUser;

    // 增加元素
    nameToUser.emplace("Alice", User("Alice", 25));
    nameToUser.insert({"Bob", User("Bob", 30)});
    nameToUser["Charlie"] = User("Charlie", 35);

    cout << "添加后大小: " << nameToUser.size() << endl;

    // 修改元素（修改年龄）
    nameToUser["Alice"].set_age(26);
    cout << "修改后Alice的信息: " << nameToUser["Alice"] << endl;

    // 查找元素
    auto it = nameToUser.find("Bob");
    if (it != nameToUser.end()) {
        cout << "找到用户: " << it->second << endl;
    }

    // 查找并修改元素
    it = nameToUser.find("Charlie");
    if (it != nameToUser.end()) {
        it->second.set_name("Charlie Brown");  // 修改用户名（键不变）
        it->second.set_age(36);                // 修改年龄
        cout << "修改后Charlie的信息: " << it->second << endl;
    }

    // 删除元素
    nameToUser.erase("Bob");
    cout << "删除Bob后大小: " << nameToUser.size() << endl;

    // 迭代器遍历
    cout << "遍历map中的用户: ";
    for (map<string, User>::iterator iter = nameToUser.begin(); iter != nameToUser.end(); ++iter) {
        cout << "[" << iter->first << " => " << iter->second << "] ";
    }
    cout << endl;

    // C++11范围for循环遍历
    cout << "范围for循环遍历: ";
    for (const auto& pair : nameToUser) {
        cout << "[" << pair.first << " => " << pair.second << "] ";
    }
    cout << endl;
}

int main() {
    cout << "=== C++11 容器使用示例 ===" << endl;

    demo_string();
    demo_vector();
    demo_list();
    demo_map();
    demo_set();
    demo_unordered_map();
    demo_unordered_set();
    demo_stack();
    demo_queue();
    demo_priority_queue();

    demo_vector_user();
    demo_list_user();
    demo_map_name_key();

    return 0;
}
