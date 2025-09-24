# 代码风格

## 函数命名

在 C/C++ 项目中，函数的命名应遵循**动词 + 名词**的规则，清晰表达函数的功能，同时保持风格一致。以下是推荐的前缀及其使用场景：

### 资源/生命周期相关

- **`init`**：初始化模块或实例
  例：`init_args()`、`init_signals()`

- **`create`**：创建对象/资源（通常涉及内存分配）  
  例：`create_socket()`、`create_thread_pool()`

- **`destroy`**：销毁对象/资源（释放内存）  
  例：`destroy_socket()`、`destroy_config()`
  （与 `create` 配对使用，形成“创建-销毁”生命周期）

- **`start`**：启动服务/进程（进入运行状态）  
  例：`start_server()`、`start_monitor()`  

- **`stop`**：停止服务/进程（进入终止状态）  
  例：`stop_server()`、`stop_timer()`  

- **`cleanup`**：清理临时资源（不销毁主体，仅释放中间状态）  
  例：`cleanup_cache()`、`cleanup_temp_files()`  

- **`shutdown`**：彻底关闭（释放所有关联资源）  
  例：`shutdown_network()`、`shutdown_database()`  

### 数据/状态操作

- **`load`**：加载数据（从文件/网络到内存）  
  例：`load_config()`、`load_model()`  

- **`save`**：保存数据（从内存到文件/存储）  
  例：`save_log()`、`save_user_data()`  

- **`parse`**：解析数据（字符串/二进制→结构化数据）  
  例：`parse_json()`、`parse_command_line()`  

- **`serialize`**：序列化（结构化数据→字符串/二进制）  
  例：`serialize_packet()`、`serialize_object()`  

- **`set`**：设置属性/状态  
  例：`set_timeout()`、`set_log_level()`  

- **`get`**：获取属性/状态  
  例：`get_version()`、`get_connection_count()`  

- **`reset`**：重置状态（恢复初始值）  
  例：`reset_counters()`、`reset_buffer()`  

### 行为/动作相关

- **`do`**：执行某个具体动作（通常用于简短操作）  
  例：`do_send()`、`do_retry()`  

- **`handle`**：处理事件/消息  
  例：`handle_signal()`、`handle_request()`  

- **`process`**：处理数据/任务（较复杂的处理逻辑）  
  例：`process_packet()`、`process_task_queue()`  

- **`dispatch`**：分发任务/事件（路由到不同处理逻辑）  
  例：`dispatch_event()`、`dispatch_command()`  

- **`notify`**：通知事件（触发回调或信号）  
  例：`notify_completion()`、`notify_error()`  

### 检查/验证相关

- **`check`**：检查状态/条件（返回布尔值或错误码）  
  例：`check_validity()`、`check_permissions()`  

- **`verify`**：验证数据完整性/合法性（更严格的检查）  
  例：`verify_signature()`、`verify_config()`  

- **`is`**：判断状态（返回布尔值，通常作为谓语）  
  例：`is_running()`、`is_connected()`  

### 输入/输出相关

- **`read`**：读取数据（从设备/流读取）  
  例：`read_from_socket()`、`read_registry()`  

- **`write`**：写入数据（向设备/流写入）  
  例：`write_to_file()`、`write_to_buffer()`  

- **`send`**：发送数据（网络/通信场景）  
  例：`send_packet()`、`send_response()`  

- **`recv`**：接收数据（网络/通信场景）  
  例：`recv_packet()`、`recv_command()`  

### 命名原则总结

1. **一致性**：同一项目中前缀含义保持统一（如 `start` 和 `stop` 必须配对）。  
2. **精确性**：前缀+后缀能准确描述函数功能（如 `load_config` 比 `read_config` 更强调“加载配置到内存并生效”）。  
3. **简洁性**：避免过长前缀，复杂逻辑可拆分为多个函数）。  

## 配置系统

toml.hpp
json.hpp
CLI.hpp
ryml.hpp
mINI/iniparser/inih
