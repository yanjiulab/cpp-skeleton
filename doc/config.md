# 程序配置

## 程序交互

`CLI11.hpp` 提供了命令行、配置文件、环境变量的统一配置。

- subcommand：支持子命令添加
- args：支持 flag 和 option 两种命令行参数。
- env：支持 args 绑定环境变量
- confile：支持从配置文件读取命令行参数。
  - ini：默认
  - toml：默认
  - json：需要自定义
  - yaml：需要自定义
  - xml：需要自定义
  - csv：需要自定义

程序运行后，需要通过控制台来获取以及更改程序状态。



- db
- console
