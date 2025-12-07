# C-Reactor

A high-performance, multi-reactor, multithreaded network server framework based on C++11 and epoll. This project adopts the Reactor design pattern and supports allocating client connections using **Round Robin** or **Load Based** strategies, making it suitable for high-concurrency server development and for learning network programming.

## Features

- Efficient event-driven model based on epoll
- Supports multi-reactor, multithreaded architecture
- Two client fd allocation modes: round robin and load-based
- Solve the TCP sticky packet problem by using a fixed-length packet header and a variable-length packet body
- Clear class hierarchy for event handling; easy to extend
- Built and tested on Linux

## Directory structure

```
.
├── include/            # Header files
│   ├── acceptorHandler.h
│   ├── connectionHandler.h
│   ├── eventHandler.h
│   ├── reactor.h
│   ├── socket.h
│   └── tcpServer.h
├── src/                # Source files
│   ├── acceptorHandler.cpp
│   ├── connectionHandler.cpp
│   ├── eventHandler.cpp
│   ├── reactor.cpp
│   ├── socket.cpp
│   ├── tcpServer.cpp
│   └── main.cpp
├── test/               # Test programs and scripts
│   ├── client.cpp      # Simple client example
│   ├── client.sh       # Script to launch many clients
│   ├── client          # Client executable
│   └── server.sh       # Script to run the server
├── Makefile
└── .gitignore
```

## Quick Start

### Requirements

- g++ (C++11 support)
- Linux OS (with epoll support)

### Build

```bash
make
```

### Run the server

```bash
# Start the server
bash test/server.sh
# Or
./output/main.exe
```

### High-concurrency client test

```bash
# After compiling the client, launch several thousand client connections to test the server (make sure the client is under test/)
bash test/client.sh
```

### Single client test

```bash
./test/client.exe
```

## Design Overview

### Core Classes

- `Reactor`: Wraps epoll and the event loop, implements handler add/modify/remove.
- `EventHandler`: Abstract base class for event handling (read/write events).
- `AcceptorHandler`: Listens on the main socket and assigns new connections (supports allocation strategies).
- `ConnectionHandler`: Handles client connections, responsible for reading and writing data.
- `TcpServer`: Overall server orchestration and startup.

### FD Allocation Modes

- `ROUND_ROBIN`: Distributes connections among reactor threads in a loop.
- `LOAD_BASED`: Assigns new connections to the Reactor with the fewest current connections (simple load balancing).

### Main Flow

1. `main.cpp` creates a `TcpServer` and specifies allocation mode and number of Reactors.
2. The main reactor listens for accept events and distributes connections to sub-reactors.
3. Each sub-reactor runs its own thread to process its connections and events.

## Example

Minimal server startup (`main.cpp`):

```c++
int main()
{
    std::string ip = "0.0.0.0";
    TcpServer server(ip, 8889, AllocMode::LOAD_BASED, 4);
    server.start();
}
```
