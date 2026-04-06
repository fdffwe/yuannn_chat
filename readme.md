YUANNN-Chat：分布式高性能全栈即时通讯系统
![alt text](https://img.shields.io/badge/Language-C%2B%2B14-blue.svg)
![alt text](https://img.shields.io/badge/Framework-Qt5%2F6-green.svg)
![alt text](https://img.shields.io/badge/RPC-gRPC-orange.svg)
![alt text](https://img.shields.io/badge/License-MIT-red.svg)

📖 项目简介
YUANNN-Chat 是一款基于 Gateway-Worker-Router 经典微服务架构开发的高性能即时通讯系统。项目实现了从底层非阻塞异步 IO 网络通信、分布式状态治理到前端跨平台 GUI 渲染的全链路开发。系统支持万人级并发长连接，具备极高的扩展性与稳定性，是进阶分布式后端开发的实战标杆。

🏗 系统架构图 (System Architecture)
系统采用功能解耦设计，将业务拆分为四个核心服务模块，通过 gRPC 进行内部高效通信。
code
Mermaid
graph TD
    %% 客户端层
    subgraph ClientLayer [前端客户端]
        C1[Qt Client 1]
        C2[Qt Client 2]
    end

    %% 服务端层
    subgraph ServerLayer [后端微服务集群]
        GS[GateServer<br/>网关/HTTP鉴权]
        CS1[ChatServer 1<br/>长连接/业务]
        CS2[ChatServer 2<br/>长连接/业务]
        SS[StatusServer<br/>状态服/路由中心]
        VS[VerifyServer<br/>Node.js验证服]
    end

    %% 存储层
    subgraph StorageLayer [数据持久与缓存]
        Redis[(Redis<br/>Token/状态/锁)]
        MySQL[(MySQL<br/>用户/好友/离线消息)]
    end

    %% 连接关系
    C1 -- 1.注册/登录 HTTP --> GS
    GS -- gRPC --> VS
    GS -- gRPC --> SS
    C1 -- 2.长连接 TCP --> CS1
    C2 -- 2.长连接 TCP --> CS2
    
    CS1 -- 状态上报 --> SS
    CS2 -- 状态上报 --> SS
    CS1 <== 3.跨服转发 gRPC ==> CS2
    
    GS & CS1 & CS2 & SS -- 存取 --> Redis
    GS & CS1 & CS2 -- 持久化 --> MySQL

🔄 核心业务时序图
1. 注册与登录时序图
展示了用户通过网关鉴权，并获取分布式环境下负载最优的聊天服务器的过程。
code
Mermaid
sequenceDiagram
    participant C as Qt 客户端
    participant G as GateServer
    participant V as VerifyServer
    participant S as StatusServer
    participant R as Redis
    participant M as MySQL

    Note over C, V: 注册阶段
    C->>G: 1. 请求验证码 (HTTP)
    G->>V: 2. 生成并发送邮件 (gRPC)
    V-->>G: 3. 返回成功
    G-->>C: 4. 邮件已发送

    Note over C, M: 登录阶段
    C->>G: 5. 登录请求 (账号, 密码)
    G->>M: 6. 验证身份
    M-->>G: 7. 身份合法
    G->>S: 8. 获取负载最低的 ChatServer
    S-->>G: 9. 返回 Chat1 地址 (IP:Port)
    G->>R: 10. 写入 Session Token (SETEX)
    G-->>C: 11. 返回 {Token, Chat1_Address}

2. TCP 连接与核心聊天时序图
展示了跨服务器（A 连在 Server1，B 连在 Server2）时的消息路由逻辑。
code
Mermaid
sequenceDiagram
    participant A as 用户 A (Client1)
    participant CS1 as ChatServer 1
    participant SS as StatusServer
    participant CS2 as ChatServer 2
    participant B as 用户 B (Client2)

    Note over A, CS1: 建立长连接
    A->>CS1: 1. TCP 三次握手
    A->>CS1: 2. 发送登录包 (Uid, Token)
    CS1->>SS: 3. 登录上报 (Uid_A 在 CS1)
    CS1-->>A: 4. 认证成功

    Note over A, B: 跨服聊天逻辑
    A->>CS1: 5. 发消息给 B [From:A, To:B, Msg:Hi]
    CS1->>SS: 6. 查询 B 的位置
    SS-->>CS1: 7. B 在 ChatServer 2
    CS1->>CS2: 8. 转发消息 (gRPC NotifyMsg)
    CS2->>B: 9. 推送消息 (TCP Push)
    CS2-->>CS1: 10. 转发成功
    CS1-->>A: 11. 消息已送达回复

3. 心跳保活与僵尸连接清理
通过双向心跳检测，确保服务器资源不被异常断开的连接占用。
code
Mermaid
sequenceDiagram
    participant C as 客户端
    participant S as ChatServer
    participant SS as StatusServer

    loop 每 30s 循环
        C->>S: Ping (心跳包)
        S-->>C: Pong (回包)
        S->>S: 更新最后活跃时间
    end

    Note right of S: 若 60s 未收到心跳
    S->>S: 1. 判定连接已失效
    S->>S: 2. 主动 Close Socket
    S->>SS: 3. 通知 Uid 下线 (gRPC)
    SS->>SS: 4. 清除 Redis 在线状态

📊 核心数据流转图 (Data Flow Diagram)
展示了系统中 Token（鉴权流）、状态（路由流）和消息（业务流）的数据走向。
code
Mermaid
flowchart TD
    %% 数据源
    UserA((用户A))
    UserB((用户B))

    %% 服务
    Gate{GateServer}
    ChatA[ChatServer A]
    ChatB[ChatServer B]
    Status[StatusServer]

    %% 存储
    Redis[(Redis 缓存)]
    MySQL[(MySQL 库)]

    %% 1. 鉴权数据流
    UserA -- "1. 账号密码" --> Gate
    Gate -- "2. 生成 Token" --> Redis
    Gate -- "3. Token + ChatIP" --> UserA

    %% 2. 状态路由流
    UserA -- "4. Token 校验" --> ChatA
    ChatA -- "5. UidA -> ServerA" --> Status
    Status -- "6. 维护在线索引" --> Redis

    %% 3. 消息业务流
    UserA -- "7. 消息体 [To:B]" --> ChatA
    ChatA -- "8. 寻址: B 在哪?" --> Status
    Status -- "9. B 在 ServerB" --> ChatA
    ChatA -- "10. RPC 转发" --> ChatB
    ChatB -- "11. 下发" --> UserB

    %% 4. 离线流
    ChatA -- "12. 若B不在线" --> MySQL
    UserB -- "13. 登录后拉取" --> MySQL

🌟 技术亮点与优化
高并发 IO 模型：基于 Boost.Asio 封装非阻塞异步服务器，引入 IOContext Pool 机制，单机实测支撑 8000+ 活跃连接。
分布式单点登录 (SSO)：通过 分布式锁 与 gRPC 异步信令 实现了完美的“跨服踢人”逻辑。
多维池化组件：手写了 MySQL 连接池（支持心跳保活）、Redis 连接池及 gRPC Stub 复用池，大幅降低系统资源开销。
原子化负载上报：利用 Redis HINCRBY 指令实现 ChatServer 连接数的原子计数，确保负载均衡策略的精准度。
Qt 极致渲染：前端基于 Model/View 架构 与 QPainter 自定义绘图，即使在 10W+ 聊天记录下滚动依然保持 60 FPS。

📂 项目目录结构
code
Text
.
├── Client              # Qt 跨平台客户端源码
├── GateServer          # 网关服务：负载均衡、注册登录、Token 分发
├── ChatServer          # 聊天服务：维护 TCP 长连接、分布式消息转发、踢人逻辑
├── StatusServer        # 状态服务：全局路由寻址、在线状态管理
├── VerifyServer        # 验证服务：基于 Node.js 实现的邮件验证码系统
├── common              # 公共组件：Protobuf 定义、单例模式封装、配置解析
├── scripts             # 运维脚本：start_all.sh (一键启动集群)
└── yuannnchat_db       # 数据库部署：Docker-Compose 配置与 SQL 初始化脚本

🚀 快速启动
启动数据库：cd yuannnchat_db && sudo docker compose up -d
编译后端：分别进入各 Server 目录，执行 cmake .. && make（建议开启 Swap 空间以防 OOM）。
运行集群：执行根目录下的 ./start_all.sh。
运行客户端：使用 Qt Creator 编译并运行 Client 目录，修改配置文件中的阿里云公网 IP。
Author: 何凯 (yuannn)
Contact: 18152764224 / 18152764224@163.com
Project Repository: YUANNN-Chat