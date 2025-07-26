# C Project Template

## 项目结构

```bash
.
├── 3rd                 # 第三方依赖库
│   ├── include         # 头文件（每个库独立目录）
│   │   ├── ...         # 依赖 A
│   │   └── ...         # 依赖 B
│   └── lib             # 库文件（一般不再细分目录）
├── build               # 构建目录/分发目录
├── cmake               # cmake 工具
│   └── toolchain       # 编译链工具
│       ├── aarch64-linux-gnu.cmake
│       └── arm-linux-gnueabihf.cmake
├── CMakeLists.txt      # cmake 构建脚本（推荐使用 cmake 构建项目）
├── config.h            # config.h.in 生成的项目配置头文件
├── config.h.in         # 项目配置头文件
├── doc                 # 文档目录
│   ├── CHANGELOG.md    # 版本变更文档
│   └── USAGE.md        # 软件使用文档
├── etc                 # 配置文件目录
├── include             # 本项目生成 lib 对外提供 API 头文件。若是纯 exe 项目，则为空。
├── lib                 # 本项目生成 lib 库文件。若是纯 exe 项目，则为空。
├── Makefile            # make 构建脚本
├── README.md           # 本项目说明文档
├── scripts             # 脚本（部署、备份等）
├── src                 # 源代码目录
│   ├── app             # app 可执行模块               
│   ├── cppdemo         # cppdemo 可执行模块
│   └── util            # C 常用工具模块
├── test                # 测试代码目录
├── tmp                 # 临时文件/未归档文件
└── tools               # 工具
```

### 设计哲学

使用该项目模板须遵循以下规则：

- 禁止混合C/C++特性：该项目模板可以用于 C/C++ 开发，尽管 C++ 兼容 C，但不推荐混合使用 C/C++，使用 C++ 时应使用纯 C++ 特性，除非有必须使用的 C 库。
- cmake 构建：项目使用 cmake 构建。
- 必须提供文档：项目必须两个文档

### 依赖搜索

在 C/C++ 中，依赖搜索包括**头文件**和**库文件**。

头文件搜索路径包括：

- 默认搜索路径：系统头文件目录、编译器内置头文件目录等，不需要本项目特别指定的。
- config.h：本项目自身 config 头文件
- include：本项目提供的 lib 对外接口头文件
- 3rd/include：本项目提供的第三方库头文件目录

库文件搜索路径包括：

- 默认搜索路径：系统库文件目录、编译器内置库文件目录等，不需要本项目特别指定的。
- lib：本项目提供的 lib 库目录。
- 3rd/lib：本项目内置的第三方库目录。

当依赖是**仅头文件库**（header only）时，推荐将其直接放入 `3rd/include` 目录中，这样第三方库会直接集成进可执行文件中，便于使用，但带来了代码膨胀的问题，不过大部分情况下是可以接受的。

当依赖是**预编译库**（pre-compiled）时，有两种选择：

- 通过**包管理器**或**源码编译安装**依赖：如未特殊指定安装目录，则一般安装到 `/usr/local` 或 `/usr` 目录下，一般情况下，上述两个目录都是系统默认的搜索目录。在某些系统中 `/usr/local` 并没有加入默认搜索路径，推荐设置加入。
- 下载对应的头文件和库文件，将其放置于 `3rd` 目录下。

### 版本管理

通过 `CMakeLists.txt` 中 `project(proj VERSION 0.1.0)` 定义与管理项目版本，最终生成 `config.h` 中版本宏，在代码中可直接使用。

```c
printf("VERSION: %d.%d.%d\n", proj_VERSION_MAJOR, proj_VERSION_MINOR, proj_VERSION_PATCH);
// output: 
// VERSION: 0.1.0
```

TODO：编译宏

## 项目构建

```shell
mkdir build; cd build
cmake ..
cmake --build . # make
```

交叉编译时，项目内置了 arm 和 aarch64 交叉编译工具链设置，如果采用其他的交叉编译链，修改 `TOOLCHAIN_PATH` 即可。

```shell
mkdir build-arm; cd build-arm
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain/arm-linux-gnueabihf.cmake ..
# cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain/aarch64-linux-gnu.cmake ..
cmake --build .
```

## 项目分发

项目构建完成后，主要包括如下目录

```bash
build
├── bin
│   ├── cppdemo       # execute file
│   └── etc           # copy from ./etc
├── doc
│   ├── CHANGELOG.md  # copy from ./doc/CHANGELOG.md
│   └── USAGE.md      # copy from ./doc/USAGE.md
└── etc               # copy from ./etc, same as bin/etc
```

项目分发即：将项目可执行文件或库部署到目标平台。库的分发比较简单，直接将生成的头文件和目标平台库文件复制到目标平台即可。

可执行程序的分发包括：

使用 `./scripts/release.sh` 将 build 内容打包为 `proj-v{X.Y.Z}.tar.gz` 包。

若需分发到其他目标，则使用 `tar xfv proj-v0.1.0.tar.gz` 解压包即可。

```bash
$ tar xfv proj-v0.1.0.tar.gz
proj/bin/
proj/bin/...
proj/doc/
proj/doc/USAGE.md
proj/doc/CHANGELOG.md
proj/etc/
proj/etc/config.json
```

## 第三方库

- nlohmann/json
- spdlog
- libhv
