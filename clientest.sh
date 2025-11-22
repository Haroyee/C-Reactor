#!/bin/bash

SERVER_IP="127.0.0.1"  # 服务器 IP
PORT=8889                  # 服务器端口
CONCURRENT=100000             # 并发连接数
DURATION=360                # 测试时长（秒）
DATA="GET / HTTP/1.1\r\nHost: $SERVER_IP\r\n\r\n"  # 发送的数据（可自定义）

# 循环创建并发连接
for ((i=1; i<=$CONCURRENT; i++)); do
  # 后台运行，每个连接持续发送数据
  (while true; do
    echo -e "$DATA" | nc $SERVER_IP $PORT > /dev/null 2>&1
    sleep 0.001  # 每个请求间隔 0.1 秒，可调整
  done) &
done

# 等待测试时长
sleep $DURATION

# 结束所有后台进程
pkill -f "nc $SERVER_IP $PORT"
echo "测试结束"