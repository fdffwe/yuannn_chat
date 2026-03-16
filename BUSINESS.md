# LLFCChat — 业务文档

> 该文档面向产品与开发人员，简要说明项目业务场景、主要模块、关键流程与运行调试步骤，便于后续功能扩展与对接。

**项目简介**

- 项目名称：LLFCChat（学习/示例用途的即时通讯小系统，包含 Qt 客户端与多个后端服务）
- 主要目标：提供一个包含注册/验证码/登录流程的轻量级即时通讯框架示例，便于教学与二次开发。

**代码结构（高层）**

- `client/`：Qt 桌面客户端（UI、网络管理、业务交互）。关键文件：
  - [client/Sources/main.cpp](client/Sources/main.cpp) - 客户端启动与配置读取
  - [client/Headers/HttpMgr.hpp](client/Headers/HttpMgr.hpp) 和 [client/Sources/HttpMgr.cpp](client/Sources/HttpMgr.cpp) - HTTP 请求管理与异步回调分发
  - [client/Sources/RegisterDialog.cpp](client/Sources/RegisterDialog.cpp) - 注册 UI 与验证码请求逻辑
- `server/`：后端服务集合（示例：GateServer 网关服务）。关键文件：
  - [server/GateServer/Sources/main.cpp](server/GateServer/Sources/main.cpp) - 服务入口
  - [server/GateServer/Sources/LogicSystem.cpp](server/GateServer/Sources/LogicSystem.cpp) - 业务路由（注册 GET/POST 处理函数）
  - [server/GateServer/Sources/HttpConnection.cpp](server/GateServer/Sources/HttpConnection.cpp) - HTTP 连接/请求解析与响应写回
- `client/config.ini`：客户端网关地址配置（节：`GateServer`）

**主要业务流程（当前实现与建议）**

1. 获取验证码（前端触发）
   - 客户端：在 `RegisterDialog` 中调用 `HttpMgr::postHttpReq(GATE_URL_PREFIX + "/get_verifycode", {"email":...})`。
   - 服务端：在 `LogicSystem::regPost("/get_verifycode", ...)` 中解析请求并返回 JSON 包含 `email` 与 `error` 字段（当前实现示例返回 `{"email":"...","error":0}`）。
   - 推荐改进：服务端生成随机验证码、缓存（内存 map 或 Redis），并通过邮件/模拟通道发送；返回结构增加 `status`、`message` 字段以便客户端提示。

2. 注册提交（后端待完善）
   - 目前：客户端已经有 `ReqId::ID_REG_USER` 枚举但没有完整调用链；服务端也未实现 `/reg_user` 完整逻辑。
   - 推荐实现：增加 `/reg_user` POST 接口，参数 `{"email":"...","code":"...","password":"..."}`；服务端验证验证码、校验重复注册、对密码做哈希（如 bcrypt/argon2）、保存用户（简单可用文件/SQLite 或 MySQL）。

3. 登录（后续）
   - 建议实现登录接口（POST `/login`），返回登录凭据（短期可返回 session token 或 JWT）并在客户端保存会话信息。

**接口说明（当前/建议）**

- POST /get_verifycode
  - 请求体（示例）：

```json
{"email":"test@example.com"}
```

  - 当前响应（示例）：

```json
{"email":"test@example.com","error":0}
```

  - 建议响应（推荐）：

```json
{"status":0,"message":"验证码已发送","email":"test@example.com"}
```

- POST /reg_user （建议新增）
  - 请求体：`{"email":"...","code":"...","password":"..."}`
  - 响应示例：`{"status":0,"message":"注册成功","user_id":123}` 或错误返回 `status!=0` 并包含 `message`。

- POST /login （建议新增）
  - 请求体：`{"email":"...","password":"..."}`
  - 响应示例：`{"status":0,"token":"...","user_id":123}`。

**数据模型建议（简要）**

- User
  - id: int
  - email: string
  - password_hash: string
  - created_at: timestamp
- VerifyCode（可短期使用内存存储）
  - email: string
  - code: string
  - expire_at: timestamp
  - attempt_count: int

**错误码与异常处理（建议）**

- 0 / SUCCESS: 成功
- 1 / ERR_JSON: JSON 解析失败
- 2 / ERR_NETWORK: 网络错误（客户端）
- 1001 / ERR_INVALID_PARAMS: 参数校验失败
- 1002 / ERR_CODE_INVALID: 验证码错误或过期
- 1003 / ERR_USER_EXISTS: 用户已存在

请在服务端统一返回 `{status:int, message:string, data:object?}` 的规范，客户端据此展示用户友好提示。

**运行与调试（快速）**

- 启动 GateServer（在本仓库）示例：

```bash
cd server/GateServer
mkdir -p build && cd build
cmake ..
make -j
./bin/wechat   # 若路径不同请根据构建输出调整
```

- 使用 curl 测试 `/get_verifycode`：

```bash
curl -s -X POST "http://localhost:8080/get_verifycode" -H "Content-Type: application/json" -d '{"email":"test@example.com"}'
```

- 构建客户端（示例使用 CMake）：

```bash
cd client
mkdir -p build && cd build
cmake ..
make -j
# 运行可执行文件（路径按构建输出调整）
./wechat
```

- 客户端配置文件：编辑 `client/config.ini` 节 `GateServer` 下的 `host` 与 `port`，例：

```
[GateServer]
host=localhost
port=8080
```

**安全与运维注意事项（简短）**

- 永远不要以明文存储密码；使用加盐哈希算法（bcrypt/argon2）。
- 验证码应设置有限有效期和尝试次数限制以防滥用。
- 生产环境使用 HTTPS，并限制来源与接口频率（限流）。
- 生产邮件服务应使用成熟的邮件服务商或带队列的发送模块。

**开发建议与优先级（短清单）**

1. 端到端验证：先跑通 `/get_verifycode` 的请求链并在客户端展示结果（高）。
2. 实现 `/reg_user`：生成/存储/校验验证码并实现用户持久化（中高）。
3. 登录与会话管理（中）。
4. 单元/集成测试与 README（中）。
5. 安全加固与容器化（低，针对生产）。

**后续工作与负责人建议**

- 如果你是开发者/维护者，建议优先实现服务端验证码与注册逻辑，然后完善客户端提交与错误处理。
- 我可以帮你：启动服务并跑接口测试；实现 `/reg_user` 的代码补丁；或修复客户端 UI 布局（你选其一）。

---

文档由本仓库当前代码（`client` 与 `server/GateServer`）整理而成，供产品与开发参考。如需我按照此文档逐项实现（代码补丁与测试），请指示要优先实现的条目。