#!/bin/bash
echo "开始生成 Protobuf 和 gRPC C++ 代码..."

# 生成普通的 pb 文件
protoc -I="." --cpp_out="." message.proto

# 生成 grpc 文件
protoc -I="." --grpc_out="." --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` message.proto

echo "代码生成完毕！请去 build 目录执行 make。"

# 执行
# chmod +x generate.sh  && ./generate.sh