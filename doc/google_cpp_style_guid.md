# Google Style Guide

## 头文件

通常每一个 .cc 文件都有一个对应的 .h 文件. 也有一些常见例外, 如单元测试代码和只包含 main() 函数的 .cc
文件。

所有头文件都应该使用 `#define` 来防止头文件被多重包含, 命名格式当是: `<PROJECT>_<PATH>_<FILE>_H_` .

尽可能地避免使用前置声明。使用 #include 包含需要的头文件即可。

只有当函数只有 10 行甚至更少时才将其定义为内联函数.

使用标准的头文件包含顺序可增强可读性, 避免隐藏依赖: 相关头文件, C 库, C++ 库, 其他库的 .h, 本项目内
的 .h.

项目内头文件应按照项目源代码目录树结构排列, 避免使用 UNIX 特殊的快捷目录 . (当前目录) 或 .. (上级目
录)

## 作用域

FATAL

## 类

不要在构造函数中进行复杂的初始化 (尤其是那些有可能失败或者需要调用虚函数的初始化)

构造函数不得调用虚函数, 或尝试报告一个非致命错误. 如果对象需要进行有意义的 (non-trivial) 初始化,
考虑使用明确的 Init() 方法或使用工厂模式

如果类中定义了成员变量, 则必须在类中为每个类提供初始化函数或定义一个构造函数. 若未声明构造函数, 则
编译器会生成一个默认的构造函数, 这有可能导致某些成员未被初始化或被初始化为不恰当的值.

对单个参数的构造函数使用 C++ 关键字 explicit.

如果你的类型需要, 就让它们支持拷贝/ 移动. 否则, 就把隐式产生的拷贝和移动函数禁用。

在能够减少重复代码的情况下使用委派和继承构造函数

仅当只有数据时使用 struct, 其它一概使用 class

使用组合 (composition, YuleFox 注: 这一点也是 GoF 在 <<Design Patterns>> 里反复强调的) 常常比使用
继承更合理. 如果使用继承的话, 定义为 public 继承.

真正需要用到多重实现继承的情况少之又少. 只在以下情况我们才允许多重继承: 最多只有一个基类是非抽象
类; 其它基类都是以 Interface 为后缀的纯接口类

接口是指满足特定条件的类, 这些类以 Interface 为后缀 (不强制)

除少数特定环境外，不要重载运算符

一般不要重载运算符. 尤其是赋值操作 (operator=) 比较诡异, 应避免重载. 如果需要的话, 可以定义类似
Equals(), CopyFrom() 等函数.
然而, 极少数情况下可能需要重载运算符以便与模板或 “标准” C++ 类互操作 (如 operator<<(ostream&,
const T&)). 只有被证明是完全合理的才能重载, 但你还是要尽可能避免这样做. 尤其是不要仅仅为了在
STL 容器中用作键值就重载 operator== 或 operator<; 相反, 你应该在声明容器的时候, 创建相等判断和
大小比较的仿函数类型.
有些 STL 算法确实需要重载 operator== 时, 你可以这么做, 记得别忘了在文档中说明原因

倾向编写简短, 凝练的函数.
我们承认长函数有时是合理的, 因此并不硬性限制函数的长度. 如果函数超过 40 行, 可以思索一下能不能在不影
响程序结构的前提下对其进行分割.
即使一个长函数现在工作的非常好, 一旦有人对其修改, 有可能出现新的问题. 甚至导致难以发现的 bug. 使函数
尽量简短, 便于他人阅读和修改代码.
在处理代码时, 你可能会发现复杂的长函数. 不要害怕修改现有代码: 如果证实这些代码使用/ 调试困难, 或者你
需要使用其中的一小段代码, 考虑将其分割为更加简短并易于管理的若干函数.

## 所有权

动态分配出的对象最好有单一且固定的所有主（onwer） , 且通过智能指针传递所有权（ownership）

## cpp 特性

所有按引用传递的参数必须加上 const.
事实上这在 Google Code 是一个硬性约定: 输入参数是值参或 const 引用, 输出参数为指针. 输入参数可
以是 const 指针, 但决不能是非 const 的引用参数，除非用于交换，比如 swap()

使用 C++ 的类型转换, 如 static_cast<>(). 不要使用 int y = (int)x 或 int y = int(x) 等转换方式;

只在记录日志时使用流

简单性原则告诫我们必须从中选择其一, 最后大多数决定采用 printf + read/write

对于迭代器和其他模板对象使用前缀形式 (++i) 的自增, 自减运算符

我们强烈建议你在任何可能的情况下都要使用 const. 此外有时改用 C++11 推出的 constexpr 更好

在 C++11 里，用 constexpr 来定义真正的常量，或实现常量初始化。

整数用 0, 实数用 0.0, 指针用 nullptr 或 NULL, 字符 (串) 用 '\0'.

用 auto 绕过烦琐的类型名，只要可读性好就继续用，别用在局部变量之外的地方。

你可以用列表初始化

 只使用 Boost 中被认可的库

 允许使用以下库:
• Call Traits : boost/call_traits.hpp
• Compressed Pair : boost/compressed_pair.hpp
• <The Boost Graph Library (BGL) : boost/graph, except serialization (adj_list_serialize.
hpp) and parallel/distributed algorithms and data structures(boost/graph/parallel/*and boost/
graph/distributed/*)
• Property Map : boost/property_map.hpp
• The part of Iterator that deals with defining iterators: boost/iterator/iterator_adaptor.hpp,
boost/iterator/iterator_facade.hpp, and boost/function_output_iterator.hpp
• The part of Polygon that deals with Voronoi diagram construction and doesn’t depend on the rest
of Polygon: boost/polygon/voronoi_builder.hpp, boost/polygon/voronoi_diagram.hpp, and
boost/polygon/voronoi_geometry_type.hpp• Bimap : boost/bimap
• Statistical Distributions and Functions : boost/math/distributions
• Multi-index : boost/multi_index
• Heap : boost/heap
• The flat containers from Container: boost/container/flat_map, and boost/container/flat_set
我们正在积极考虑增加其它 Boost 特性, 所以列表中的规则将不断变化.
以下库可以用，但由于如今已经被 C++ 11 标准库取代，不再鼓励：
• Pointer Container : boost/ptr_container, 改用 std::unique_ptr
• Array : boost/array.hpp, 改用 std::array

## 命名约定

函数命名，变量命名，文件命名要有描述性；少用缩写。

文件名要全部小写, 可以包含下划线 (_) 或连字符 (-). 按项目约定来. 如果并没有项目约定， ”_” 更好。

C++ 文件要以 .cc 结尾, 头文件以 .h 结尾

类型名称的每个单词首字母均大写, 不包含下划线: MyExcitingClass, MyExcitingEnum.

变量名一律小写, 单词之间用下划线连接. 类的成员变量以下划线结尾, 但结构体的就不用

类数据成员：不管是静态的还是非静态的，类数据成员都可以和普通变量一样, 但要接下划线。

不管是静态的还是非静态的，结构体数据成员都可以和普通变量一样, 不用像类那样接下划线

对全局变量没有特别要求, 少用就好, 但如果你要用, 可以用 g_ 或其它标志作为前缀, 以便更好的区分局
部变量

在全局或类里的常量名称前加 k: kDaysInAWeek. 且除去开头的 k 之外每个单词开头字母均大写。

常 规 函 数 使 用 大 小 写 混 合, 取 值 和 设 值 函 数 则 要 求 与 变 量 名 匹 配:


倾向于不在圆括号内使用空格. 关键字 if 和 else 另起一行

switch 语句可以使用大括号分段，以表明 cases 之间不是连在一起的。在单语句循环里，括号可用可不用。
空循环体应使用 {} 或 continue

句点或箭头前后不要有空格. 指针/地址操作符 (*, &) 之后不能有空格

在声明指针变量或参数时, 星号与类型或变量名紧挨都可以.在单个文件内要保持风格一致, 所以, 如果是修改现有文件, 要遵照该文件的风格.

用 =, () 和 {} 均可.

垂直留白越少越好