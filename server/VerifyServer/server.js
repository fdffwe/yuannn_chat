const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
const config_module = require('./config')
const { v4: uuidv4 } = require('uuid');
const emailModule = require('./email');
const redis_module = require('./redis')


// 这里是 验证码 的逻辑， 还不设计 token 
async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try{
        let query_res = await redis_module.GetRedis(const_module.code_prefix + call.request.email);
        console.log("query_res is ", query_res)
        let uniqueId = query_res;
        if(query_res == null){
            // 为什么这么判断，因为在规定的时间内，用户可能多次请求验证码，我们不希望每次都生成新的验证码，
            // 而是使用之前生成的验证码，直到过期为止。
            uniqueId = uuidv4();
            if (uniqueId.length > 4) {
                uniqueId = uniqueId.substring(0, 4);
            } 
            let bres = await redis_module.SetRedisExpire(const_module.code_prefix+call.request.email, uniqueId,600)
            if(!bres){
                callback(null, { email:  call.request.email,
                    error:const_module.Errors.RedisErr
                });
                return;
            }
        }

        // 发送邮件服务： 与 redis 无关了 （上面的 redis 操作）
        console.log("uniqueId is ", uniqueId)
        let text_str =  '您的验证码为'+ uniqueId +'请三分钟内完成注册'
        //发送邮件
        let mailOptions = {
            from: config_module.email_user,
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };
        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res)
        callback(null, { email:  call.request.email,
            error:const_module.Errors.Success
        });
    }catch(error){
        console.log("catch error is ", error)
        callback(null, { email:  call.request.email,
            error:const_module.Errors.Exception
        }); 
    }
}

function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync('127.0.0.1:50051', grpc.ServerCredentials.createInsecure(), () => {
        server.start()
        console.log('grpc server started')        
    })
}
main()