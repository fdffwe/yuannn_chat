#pragma once 
#include<memory>

template<class T> 
class Singleton{

public: 
    Singleton(const Singleton& ) = delete;
    Singleton& operator=(const Singleton& ) = delete;

static std::shared_ptr<T> getInstance(){
    static std::shared_ptr<T>instance(new T);
    return instance;  
}


protected: 
    Singleton() = default; 
    ~Singleton() = default; 

};