# Daemon

daemon 是一个网络守护进程，基于 asio 实现。包括

log 添加 cli 接口，log 过滤命令。log 动态添加sink，修改原来的sink添加逻辑。

cli> log
cli> monitor ---> add sink
cli-log> info
cli-log> monitor
cli-log> 