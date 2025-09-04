# Started

不同容器有不同的特性和适用场景：

- string 适合处理字符串
- vector 提供快速随机访问，适合元素数量变化不大的场景
- list 适合频繁插入删除的场景
- map 和 set 是有序关联容器，内部通常用红黑树实现
- unordered_map 和 unordered_set 是无序关联容器，内部用哈希表实现，查找速度更快
- stack、queue 和 priority_queue 是适配器容器，提供特定的数据访问方式

你可以直接编译运行这个程序，观察每种容器的操作效果和输出结果，从而更好地理解它们的用法和区别。
