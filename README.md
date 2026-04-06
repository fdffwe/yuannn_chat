分布式全栈即时通讯系统
![alt text](https://img.shields.io/badge/Language-C%2B%2B14-blue.svg)
![alt text](https://img.shields.io/badge/Framework-Qt5%2F6-green.svg)
![alt text](https://img.shields.io/badge/RPC-gRPC-orange.svg)
![alt text](https://img.shields.io/badge/License-MIT-red.svg)

📖 项目简介
LLFC-Chat 是一款基于 Gateway-Worker-Router 经典微服务架构开发的高性能即时通讯系统。项目实现了从底层异步网络通信、分布式状态管理到前端跨平台 GUI 渲染的全链路开发。支持万人级并发长连接，具备极高的扩展性与稳定性。

🌟 核心功能
用户系统：基于 Node.js 验证服的邮箱注册、Token 鉴权登录、单点登录（SSO）冲突处理。
即时通讯：文本消息实时收发、消息持久化存储、离线消息缓存与自动拉取。
分布式治理：
跨服通信：基于 gRPC 实现不同 ChatServer 节点间的消息透明转发。
动态负载均衡：GateServer 根据 StatusServer 统计的各节点实时连接数，动态分配最优 ChatServer。
跨服踢人：支持同账号异地登录自动检测并远程下线旧设备。
高可用保障：
双向心跳检测：实时清理僵尸连接，保障服务器资源高效利用。
池化组件：手写 MySQL、Redis、gRPC Stub 连接池，规避高频 IO 瓶颈。
精致 UI：仿微信布局，基于 Model/View 架构 优化海量消息渲染，QPainter 手绘气泡对话框，支持自适应布局。

🏗 系统架构图
code
Mermaid
graph TD
    Client[Qt Client] -- HTTP --> Gate[GateServer]
    Client -- TCP --> Chat[ChatServer 集群]
    Gate -- gRPC --> Status[StatusServer]
    Gate -- gRPC --> Verify[VerifyServer Node.js]
    Chat -- gRPC --> Status
    Chat -- gRPC --> Chat
    Status -- Cache --> Redis[(Redis Cluster)]
    Chat -- Persistence --> MySQL[(MySQL)]

🛠 技术栈
模块	技术实现
前端 (Client)	Qt5/6, QSS, Model/View, QPainter, QtNetwork
后端核心	C++14, Boost.Asio (异步IO), gRPC, Protobuf
微服务	Node.js (验证服), 单例模式, 生产者消费者模型
中间件	Redis (分布式锁/缓存), MySQL (持久化)
部署/构建	CMake, Docker-Compose, Shell 脚本自动化, GitHub Actions

📂 项目目录结构
code
Text
.
├── Client              # Qt 客户端源码
├── GateServer          # 网关服务：负载均衡、注册登录鉴权
├── ChatServer          # 聊天服务：维护 TCP 长连接、业务逻辑处理
├── StatusServer        # 状态服务：全局路由管理、连接数统计
├── VerifyServer        # 验证服务：Node.js 邮件验证码发送
├── common              # 公共库：Protobuf 定义、配置文件解析
├── docker-compose.yml  # 数据库环境一键启动
└── scripts             # start_all.sh / stop_all.sh 自动化脚本

🚀 快速开始
1. 环境准备
安装 Docker & Docker-Compose。
安装 C++ 编译环境 (Ubuntu 20.04+, CMake, Boost, gRPC, Protobuf)。
安装 Node.js 环境。
2. 启动基础数据库
code
Bash
cd yuannnchat_db
sudo docker compose up -d
3. 编译后端服务
code
Bash
# 以 ChatServer 为例
cd ChatServer && mkdir build && cd build
cmake .. && make
4. 一键启动业务集群
code
Bash
./start_all.sh
5. 运行客户端
使用 Qt Creator 打开 Client 目录，修改 config.ini 中的网关公网 IP，编译运行即可。

📈 性能压测
单机并发：经测试，单台 ChatServer 支持 8000+ 活跃连接。
响应延迟：跨服消息投递平均延迟 < 50ms。
渲染表现：消息列表在 10W+ 数据量下滚动依然保持 60 FPS。

🔗 开发日志 (部分)

手搓微服务基础框架搭建

基于 Node.js 的邮件认证服务实现

Redis 封装与分布式 Token 校验

MySQL 连接池及注册/重置逻辑完善

Qt 聊天布局与气泡对话框封装

跨服踢人与动态连接数统计 (分布式锁实现)

客户端与服务端双向心跳检测机制

离线消息接收与数据持久化
Author: yuannn
Contact: 3517649062@qq.com
License: MIT