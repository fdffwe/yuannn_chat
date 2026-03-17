//  服务间是通过： grpc 来进行通信的， 下面是 grpc 的 proto 文件的加载和导出
// node.js 版本的 grpc 需要使用 @grpc/grpc-js 和 @grpc/proto-loader 来加载 proto 文件

const path = require('path')
const grpc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')
const PROTO_PATH = path.join(__dirname, 'message.proto')
const packageDefinition = protoLoader.loadSync(PROTO_PATH, { keepCase: true, longs: String, enums: String, defaults: true, oneofs: true })


const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)
const message_proto = protoDescriptor.message
module.exports = message_proto