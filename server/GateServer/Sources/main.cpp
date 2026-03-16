#include"const.hpp"
#include"Server.hpp"
#include "ConfigMgr.hpp"

int main(){

    try{
        net::io_context ioc{1}; 

        ConfigMgr gCfgMgr;
        std::string gate_port_str = gCfgMgr["GateServer"]["port"];
        unsigned short gate_port = atoi(gate_port_str.c_str());
                

        net::signal_set signals(ioc,SIGINT,SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& ec, int signal){

            if(ec) return;
            ioc.stop();

        });

        std::make_shared<Server>(ioc,gate_port)->start(); 
        ioc.run();

    }
    catch(std::exception& e){
        std::cout <<"exception is " << e.what() << std::endl;
        return EXIT_FAILURE;
    }



    return 0;
}