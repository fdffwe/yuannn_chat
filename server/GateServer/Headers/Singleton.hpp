#pragma once 
#include"const.hpp"



template <class T>
class Singleton{

public: 

    static std::shared_ptr<T> getInstance(){
        static std::shared_ptr<T> instance(new T);
        return instance; 
    }

    Singleton(const Singleton& ) = delete; 
    Singleton& operator=(const Singleton&) = delete; 
    
    ~Singleton() = default; 

protected: 
    Singleton() = default; 


};