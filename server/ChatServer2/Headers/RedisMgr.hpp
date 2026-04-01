#pragma once
#include "Singleton.hpp"
#include "const.hpp"

#include <hiredis/hiredis.h>

// 基本上所有的 Pool 都是 生产者消费者模型
class RedisConPool{

public: 

    // 生产者
    RedisConPool(size_t poolSize,const std::string &host, const int& port, const std::string& passwd)
    : _poolSize(poolSize), _host(host), _port(port), _b_stop(false){
        for(int i = 0; i<_poolSize; i++){
            // 本就返回的指针呢
            auto context = redisConnect(host.c_str(),port);
            if(context == nullptr || context->err){
                if(context != nullptr) redisFree(context);
                continue ;
            }

            // 返回的 void* 
            auto reply = (redisReply*) redisCommand(context, "auth %s", passwd.c_str());
            // if(reply->type == REDIS_REPLY_ERROR){
            //     std::cout<< "passwd: " << passwd << std::endl; 

            //     std::cout << "redis 认证失败" << std::endl;
            //     //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
            //     freeReplyObject(reply);
            //     continue;
            // }

            //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
            freeReplyObject(reply);
            std::cout << "redis 认证成功" << std::endl;
            connections_.push(context);

        }
    }

    redisContext* getConnection(){
        std::unique_lock<std::mutex> lock(_mutex);
        condition_.wait(lock,[this]{return _b_stop || !connections_.empty();});

        if(_b_stop) return nullptr; 

        auto context = connections_.front(); 
        connections_.pop(); 
        
        return context; 
    }

    void returnConnection(redisContext* context){
        std::unique_lock<std::mutex> lock(_mutex);
        if(_b_stop) return ;
        connections_.push(context);
        condition_.notify_one(); 
    }


    ~RedisConPool(){
        std::lock_guard<std::mutex> lock(_mutex); 
        while(!connections_.empty()){
            connections_.pop(); 
        }
    }

    void close(){
        _b_stop = true; 
        condition_.notify_all(); 
    }

private: 
    std::string _host; 
    int _port; 

    std::atomic<bool> _b_stop; 
    size_t _poolSize;
    std::mutex _mutex; 
    std::queue<redisContext*> connections_;
    std::condition_variable condition_;


};






class RedisMgr : public Singleton<RedisMgr>{
    friend class Singleton<RedisMgr>; 

public: 
    ~RedisMgr();
    // 普通类型
    bool Get(const std::string &key, std::string& value);
    bool Set(const std::string &key, const std::string &value);

    // List
    bool LPush(const std::string &key, const std::string &value);
    bool LPop(const std::string &key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key, std::string& value);
    
    // hash
    bool HSet(const std::string &key, const std::string  &hkey, const std::string &value);
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string HGet(const std::string &key, const std::string &hkey);
    bool Del(const std::string &key);
    bool HDel(const std::string& key, const std::string& field);

    bool ExistsKey(const std::string &key);
    void Close();

    // lock
    std::string acquireLock(const std::string& lockName,
		int lockTimeout, int acquireTimeout);

	bool releaseLock(const std::string& lockName,
		const std::string& identifier);

private: 
    RedisMgr();
    std::unique_ptr<RedisConPool> _con_pool;
};