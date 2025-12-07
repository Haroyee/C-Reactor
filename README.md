# C-Reactor

一个基于C++11和epoll实现的高性能多Reactor多线程网络服务器框架。该项目采用Reactor设计模式，支持以**轮询（Round Robin）**或**负载均衡（Load Based）**分配客户端连接，适合高并发场景的服务端开发和网络编程学习。

## 特性

- 基于epoll的高效事件驱动模型
- 支持多Reactor多线程架构
- 支持两种客户端fd分配模式：轮询和负载均衡
- 采用固定长度包头与变长包体解决tcp粘包问题
- 事件处理类层次结构清晰，扩展性强
- 编译与运行在Linux下完成测试

## 目录结构

```
.
├── include/            # 头文件目录
│   ├── acceptorHandler.h
│   ├── connectionHandler.h
│   ├── eventHandler.h
│   ├── reactor.h
│   ├── socket.h
│   └── tcpServer.h
├── src/                # 源文件目录
│   ├── acceptorHandler.cpp
│   ├── connectionHandler.cpp
│   ├── eventHandler.cpp
│   ├── reactor.cpp
│   ├── socket.cpp
│   ├── tcpServer.cpp
│   └── main.cpp
├── test/               # 测试程序和脚本
│   ├── client.cpp      # 简单客户端例子
│   ├── client.sh       # 批量启动客户端脚本
│   ├── client          # 客户端可执行文件
│   └── server.sh       # 启动服务器脚本
├── Makefile
└── .gitignore
```

## 快速开始

### 依赖环境

- g++ (C++11支持)
- Linux操作系统（需支持epoll）

### 编译

```bash
make
```

### 运行服务端

```bash
# 启动服务端
bash test/server.sh
# 或者
./output/main.exe
```

### 测试高并发客户端

```bash
# 编译客户端后启动上千个连接测试服务端（注意：client需在test/下）
bash test/client.sh
```

### 客户端单独测试

```bash
./test/client.exe
```

## 设计说明

### 核心类

- `Reactor`：封装epoll与事件循环，实现添加、修改、删除处理器。
- `EventHandler`：事件处理器抽象基类（读/写事件的接口）。
- `AcceptorHandler`：负责监听主socket并分配新连接（支持分配策略）。
- `ConnectionHandler`：客户端连接处理器，负责读写数据。
- `TcpServer`：Server整体调度和启动。

### fd分配模式

- `ROUND_ROBIN`：循环方式分配给不同Reactor线程
- `LOAD_BASED`：优先分配到当前连接数最少的Reactor，实现简单负载均衡

### 简易主流程

1. `main.cpp` 创建 `TcpServer`，并指定分配模式和Reactor数量。
2. 主Reactor监听accept事件，分配连接到子Reactor。
3. 每个Reactor用独立线程处理自己的连接和事件。

## 示例代码

服务器启动示例（`main.cpp`）：

```c++
int main()
{
    std::string ip = "0.0.0.0";
    TcpServer server(ip, 8889, AllocMode::LOAD_BASED, 4);
    server.start();
}
```

