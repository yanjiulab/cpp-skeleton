# LOG

## 守护进程

syslog：一般用于设置启动过程中的
标准输出
日志log

## syslog

### 日志来源

facility（设施）是 syslog 对日志来源的分类标识，决定日志的分发路径，通过 `syslog()` 函数的第一个参数指定（如 `syslog(LOG_ERR | LOG_AUTH, "xxx")`）。

配置文件（如 `/etc/rsyslog.conf`）中通过 `facility.priority` 规则定义日志路由（例如 `authpriv.* /var/log/secure` 表示 `LOG_AUTHPRIV` 设施的所有优先级日志写入 `/var/log/secure`）。

| 日志类型         | Debian 系路径                  | RHEL 系路径                   | 使用的 facility（设施）                          | 写入来源（场景）                                                                 | 主要记录内容                                                                 |
|------------------|-------------------------------|-------------------------------|--------------------------------------------------|--------------------------------------------------------------------------------------|------------------------------------------------------------------------------|
| **系统核心日志** | `/var/log/syslog`             | `/var/log/messages`           | `LOG_USER`、`LOG_DAEMON`、`LOG_KERN` 等（多设施组合） | 系统整体运行状态相关事件（排除细分场景）                                             | 系统整体运行状态、用户程序日志、守护进程消息等                                  |
| **认证日志**     | `/var/log/auth.log`           | `/var/log/secure`             | `LOG_AUTH`、`LOG_AUTHPRIV`                       | 身份认证与权限验证相关操作                                                           | SSH 登录、sudo 操作、系统认证失败记录、PAM 模块日志                            |
| **内核日志**     | `/var/log/kern.log`           | `/var/log/kern.log`           | `LOG_KERN`                                       | 内核及硬件相关事件                                                                    | 内核事件（驱动加载、硬件故障）、printk 输出、模块操作、系统调用异常            |
| **守护进程日志** | `/var/log/daemon.log`         | 需手动配置（默认无）          | `LOG_DAEMON`                                    | 系统守护进程运行相关事件                                                             | 系统服务（如 nginx、sshd）的启动/停止、崩溃信息、运行状态变更                  |
| **邮件日志**     | `/var/log/mail.log`           | `/var/log/maillog`            | `LOG_MAIL`                                       | 邮件服务相关操作                                                                     | 邮件发送/接收记录、投递失败原因、邮件服务器（postfix/sendmail）运行日志       |
| **定时任务日志** | `/var/log/cron.log`（需启用） | `/var/log/cron`               | `LOG_CRON`                                       | 定时任务（cron/at）执行相关事件                                                      | crontab/at 任务执行结果、权限校验、任务启动/失败记录                           |
| **调试日志**     | `/var/log/debug`              | 需手动配置（默认无）          | 所有设施（`LOG_USER`/`LOG_DAEMON` 等）           | 程序调试模式下的详细输出                                                             | 程序调试信息、详细初始化步骤、变量值打印（数据量大，默认不启用）                |
| **用户会话日志** | `/var/log/wtmp`（二进制）     | `/var/log/wtmp`（二进制）     | 无特定 facility（由登录子系统直接生成）           | 用户登录/注销相关事件                                                                 | 用户登录/注销记录、终端类型、登录IP（需用 `last` 命令查看）                    |
| **自定义程序日志** | 通常为 `/var/log/程序名/`     | 通常为 `/var/log/程序名/`     | `LOG_LOCAL0`~`LOG_LOCAL7`（本地自定义设施）       | 第三方程序业务逻辑相关事件                                                           | 第三方程序业务日志（如 `/var/log/nginx/access.log`）                           |

### 日至等级

syslog 定义了 8 个标准日志级别（level），按优先级从高到低排序（数值数字越小优先级越高，越需要立即关注），用于表示日志信息的严重程度。以下是详细整理：

| 级别名称       | 数值 | 含义描述                                                                 | 典型使用场景                                                                 |
|----------------|------|--------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------|
| `LOG_EMERG`    | 0    | 紧急情况：系统不可用（最高优先级）                                       | 系统崩溃、核心服务完全中断（如“文件系统损坏，无法挂载”）                     |
| `LOG_ALERT`    | 1    | 警报：必须立即采取行动                                                   | 安全入侵（如“检测到未授权的 root 登录”）、关键资源耗尽（如“磁盘空间为 0”）   |
| `LOG_CRIT`     | 2    | 严重错误：严重条件                                                       | 数据库崩溃、关键服务核心功能失效（如“数据库连接池全部耗尽且无法恢复”）         |
| `LOG_ERR`      | 3    | 错误：错误条件                                                           | 函数调用失败（如“fork 系统调用失败”）、配置文件解析错误                     |
| `LOG_WARNING`  | 4    | 警告：异常情况但不影响核心功能                                           | 磁盘空间不足（如“剩余空间低于 10%”）、配置服务性能下降（如“响应时间超过阈值”） |
| `LOG_NOTICE`   | 5    | 通知：正常但值得注意的事件                                               | 服务启动/停止（如“nginx 服务已重启”）、权限变更（如“用户添加到 sudo 组”）   |
| `LOG_INFO`     | 6    | 信息：普通信息性消息                                                     | 常规操作记录（如“用户登录成功”）、程序运行状态（如“数据同步完成”）           |
| `LOG_DEBUG`    | 7    | 调试：调试信息（最低优先级）                                             | 程序内部变量值、函数调用栈、详细执行步骤（仅开发/调试时启用，数据量较大）     |

1. **优先级特性**：当日志服务配置为“记录某一级别”时，会**自动包含所有优先级更高的日志**。例如，配置 `*.err` 会记录 `LOG_ERR`、`LOG_CRIT`、`LOG_ALERT`、`LOG_EMERG` 这四级日志。
2. **使用方式**：在 `syslog()` 函数中通过“按位或”与设施（facility）组合使用，例如：

   ```cpp
   syslog(LOG_WARNING | LOG_USER, "磁盘空间不足");  // 用户程序的警告日志
   syslog(LOG_DEBUG | LOG_DAEMON, "进入数据处理函数");  // 守护进程的调试日志
   ```

3. **实际应用**：生产环境通常关闭 `LOG_DEBUG` 以减少日志量，仅记录 `LOG_INFO` 及以上级别；调试阶段可临时开启 `LOG_DEBUG` 排查问题。

### 常用配置

syslog 的配置主要通过日志服务（如 `rsyslog` 或 `syslog-ng`）的配置文件实现，用于定义日志的收集规则、过滤条件和输出目标（文件、网络、数据库等）。以下以最常用的 `rsyslog` 为例，介绍配置文件的基本结构和配置方法：

不同 Linux 发行版的默认配置文件路径略有差异：

- **Debian 系（Ubuntu/Debian）**：
  - 主配置文件：`/etc/rsyslog.conf`
  - 子配置文件：`/etc/rsyslog.d/` 目录下的 `.conf` 文件（推荐在此添加自定义规则）
- **RHEL 系（CentOS/RHEL）**：
  - 主配置文件：`/etc/rsyslog.conf`
  - 子配置文件：`/etc/rsyslog.d/` 目录

`rsyslog` 配置文件由以下几部分组成：

1. **模块加载**：加载所需模块（如网络日志、数据库输出等）
2. **全局配置**：设置日志缓存、权限等全局参数
3. **规则定义**：核心部分，定义“日志来源（设施+级别）→ 输出目标”的映射关系

一般情况下，我们不要更改 `/etc/rsyslog.conf`，而是从子配置目录下加载自定义规则。

#### 规则定义

最常用的规则很简单，格式：`[设施.级别] 输出目标`

- **设施（facility）**：`auth`、`kern`、`user`、`daemon` 等（或 `*` 表示所有设施）
- **级别（level）**：`err`、`warning`、`info` 等（或 `*` 表示所有级别，`none` 表示排除）
- **输出目标**：
  - 本地文件（如 `/var/log/messages`）
  - 网络地址（如 `@192.168.1.100:514` 表示发送到远程服务器）
  - 用户名（如 `root` 表示发送给 root 用户）
  - 管道（如 `|/usr/bin/logger` 表示通过管道传递给其他程序）

例如

```conf
# 所有设施的错误及以上级别日志写入 /var/log/errors.log
*.err         /var/log/errors.log

# 认证相关的所有日志写入 /var/log/auth.log（Debian 系默认）
auth,authpriv.*   /var/log/auth.log

# 邮件服务的日志排除 info 级别以下的信息，写入 /var/log/mail.log
mail.!info      /var/log/mail.log

# 内核日志发送到远程日志服务器（IP:192.168.1.100，端口514）
kern.*          @192.168.1.100:514

# 所有日志（除了邮件和认证）写入 /var/log/syslog（Debian 系默认）
*.*;mail.none;authpriv.none   -/var/log/syslog
# 注：文件名前的“-”表示异步写入，提高性能
```

#### 模块加载

`rsyslog` 功能通过模块扩展，常用模块需在配置文件中显式加载：

```conf
# 加载文件输入模块（默认已加载，用于读取本地日志）
$ModLoad imuxsock

# 加载网络输入模块（允许接收远程日志）
$ModLoad imudp
$UDPServerRun 514  # 启用 UDP 514 端口接收日志

# 加载 TCP 输入模块
$ModLoad imtcp
$InputTCPServerRun 514  # 启用 TCP 514 端口接收日志

# 加载数据库输出模块（如写入 MySQL）
$ModLoad ommysql
```

#### 全局配置

设置日志缓存、文件权限等全局参数，一般情况下也不用修改。

```conf
# 设置日志文件默认权限（所有者可读写，组和其他可读）
$FileCreateMode 0644

# 设置日志目录权限
$DirCreateMode 0755

# 设置日志缓存大小（默认 8k）
$WorkDirectory /var/spool/rsyslog  # 缓存文件存储路径
$ActionQueueFileName fwdRule1     # 缓存文件前缀
$ActionQueueMaxDiskSpace 1g       # 最大磁盘缓存 1GB
$ActionQueueSaveOnShutdown on     # 关闭时保存缓存
```

#### 自定义配置

假设需要将“自定义程序（使用 `LOG_LOCAL0` 设施）的 `info` 及以上级别日志”写入 `/var/log/myapp.log`，步骤如下：

1. 在 `/etc/rsyslog.d/` 目录创建自定义配置文件（推荐）：

   ```bash
   sudo vim /etc/rsyslog.d/10-myapp.conf
   ```

2. 添加规则：

   ```conf
   # 自定义程序日志规则
   local0.info    /var/log/myapp.log  # local0 设施的 info 及以上级别日志写入指定文件
   ```

3. 重启 `rsyslog` 服务使配置生效：

   ```bash
   # Debian 系
   sudo systemctl restart rsyslog
   
   # RHEL 系
   sudo service rsyslog restart
   ```

4. 测试配置：通过程序或 `logger` 命令生成日志验证

   ```bash
   # 使用 logger 命令发送一条 local0.info 级别的日志
   logger -p local0.info "这是一条测试日志"
   
   # 查看日志是否写入目标文件
   cat /var/log/myapp.log
   ```

#### 配置生效与调试

1. **检查配置文件语法**：

   ```bash
   rsyslogd -N 1  # 验证配置文件是否有语法错误
   ```

2. **查看 `rsyslog` 运行状态**：

   ```bash
   # 查看服务状态
   systemctl status rsyslog
   
   # 查看详细日志（排查配置问题）
   journalctl -u rsyslog
   ```

3. **实时监控日志输出**：

   ```bash
   tail -f /var/log/myapp.log  # 实时查看自定义日志文件
   ```

### 配置示例

实例 1：基础日志分类配置（适用于大多数服务器）

```conf
# 加载必要模块
$ModLoad imuxsock  # 本地日志接收模块
$ModLoad imklog    # 内核日志接收模块

# 全局配置：日志文件权限
$FileCreateMode 0640
$DirCreateMode 0755

# 认证日志（SSH登录、sudo等）
auth,authpriv.*         /var/log/auth.log

# 内核日志
kern.*                  /var/log/kern.log

# 守护进程日志（系统服务如nginx、sshd）
daemon.*                /var/log/daemon.log

# 邮件服务日志
mail.*                  -/var/log/mail.log  # "-"表示异步写入提高性能

# 定时任务日志
cron.*                  /var/log/cron.log

# 用户程序日志（默认设施LOG_USER）
user.*                  /var/log/user.log

# 所有日志（排除已单独配置的）汇总到syslog
*.*;auth,authpriv.none;mail.none;cron.none  /var/log/syslog

# 调试日志（仅记录debug级别，默认禁用）
# *.=debug               /var/log/debug

```

实例 2：远程日志服务器配置（接收其他主机的日志）

```conf
# 加载网络接收模块
$ModLoad imudp
$UDPServerRun 514  # 启用UDP 514端口（syslog默认端口）

$ModLoad imtcp
$InputTCPServerRun 514  # 启用TCP 514端口（更可靠，适合大量日志）

# 为远程主机日志创建单独目录（按IP区分）
$template RemoteLog,"/var/log/remote/%FROMHOST-IP%/%PROGRAMNAME%.log"

# 所有远程日志按模板存储
:fromhost-ip, !isequal, "127.0.0.1" ?RemoteLog

# 本地日志仍按原有规则处理
$IncludeConfig /etc/rsyslog.d/*.conf  # 包含基础配置
```

实例 3：日志转发到远程服务器（客户端配置）

```conf
# 转发所有日志到远程服务器（使用TCP更可靠）
*.* @@192.168.1.100:514  # @@表示TCP，@表示UDP，IP替换为实际日志服务器地址

# 仅转发错误级别及以上的日志（减少网络流量）
# *.err @@192.168.1.100:514

# 转发特定设施的日志（如仅转发内核日志）
# kern.* @@192.168.1.100:514
```

实例 4：自定义程序日志配置（按程序名分离）

```conf
# 1. 使用自定义设施（LOG_LOCAL0-7）的程序
# 例如：程序中用 syslog(LOG_INFO | LOG_LOCAL1, "消息") 输出的日志
local1.*                /var/log/myapp/myapp.log

# 2. 按程序名（PROGRAMNAME）过滤
# 例如：匹配所有来自 nginx 的日志
if $programname == 'nginx' then /var/log/nginx/access.log
& stop  # 匹配后停止处理，避免重复写入其他文件

# 3. 按日志内容过滤（如包含"ERROR"的日志）
if $msg contains 'ERROR' then /var/log/errors/error_agg.log
```

实例 5：日志轮转配置（防止日志文件过大）

```conf
/var/log/syslog
/var/log/auth.log
/var/log/kern.log
/var/log/daemon.log
/var/log/mail.log
/var/log/user.log
/var/log/cron.log {
    daily                  # 每天轮转一次
    rotate 7               # 保留7天的日志
    missingok              # 日志文件不存在时不报错
    notifempty             # 空文件不轮转
    compress               # 轮转后压缩（.gz）
    delaycompress          # 延迟压缩（保留最新的一个未压缩）
    sharedscripts          # 所有文件轮转后执行一次脚本
    postrotate             # 轮转后重启rsyslog
        systemctl restart rsyslog > /dev/null 2>&1 || true
    endscript
}

# 自定义应用日志的轮转配置
/var/log/myapp/myapp.log {
    hourly                 # 每小时轮转一次（适合高频率日志）
    rotate 24              # 保留24小时
    compress
    missingok
}
```
