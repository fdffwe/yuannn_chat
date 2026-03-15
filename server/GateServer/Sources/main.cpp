#include"const.hpp"
#include"Server.hpp"

int main(){

    try{
        net::io_context ioc{1}; 
        unsigned short port = 8080;

        net::signal_set signals(ioc,SIGINT,SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& ec, int signal){

            if(ec) return;
            ioc.stop();

        });

        std::make_shared<Server>(ioc,port)->start(); 
        ioc.run();

    }
    catch(std::exception& e){
        std::cout <<"exception is " << e.what() << std::endl;
        return EXIT_FAILURE;
    }



    return 0;
}