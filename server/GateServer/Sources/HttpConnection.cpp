#include"HttpConnection.hpp"
#include"LogicSystem.hpp"


HttpConnection::HttpConnection(net::io_context& ioc): _socket(ioc){

}

void HttpConnection::start(){
    
    auto self = shared_from_this(); 

    // 为短连接设置单次保护性超时：先设置到期并立即注册等待回调，
    // 避免延迟注册导致立即触发的问题。
    checkDeadline();

    http::async_read(_socket,_buffer,_request,[self](beast::error_code ec, size_t bytes){
        try{
            if(ec){
                std::cout << "http read error is " << ec.what() << std::endl; 
                // 读失败时取消定时器并关闭socket，释放资源
                self->_deadline.cancel();
                self->_socket.close();
                return; 
            }

            // 处理读取的数据 
            boost::ignore_unused(bytes);
            self->handleReq(); 
        }
        catch(std::exception& e){
            std::cout << "excption is " << e.what() << std::endl; 
            self->_deadline.cancel();
            self->_socket.close();
        }
    });

}

void HttpConnection::handleReq(){
    // start 就是监听 read 事件
    // 这个回调函数： 构造对应的 response 回应对端

    // 基本设置
    _response.version(_request.version());
    _response.keep_alive(false); 

    // get 请求
    if(_request.method() == http::verb::get){

        parseGetParam();
        // _request.target() 是 url (原始的，带参数的)
        // _getUrl 是我解析了以后的， 只是Url
        bool success = LogicSystem::getInstance()->handleGet(_getUrl,shared_from_this());

        if(!success){
            _response.result(http::status::not_found);
            _response.set(http::field::content_type,"text/plain"); 
            
            beast::ostream(_response.body()) << "url not found \r\n";
            writeResponse(); 

            return ;
        }

        _response.result(http::status::ok);
        _response.set(http::field::server,"GateServer"); 
        writeResponse(); 
        
        return ;
    }
    else if(_request.method() == http::verb::post){
        bool success = LogicSystem::getInstance()->handlePost(_request.target(),shared_from_this());
        
        if(!success){
            _response.result(http::status::not_found);
            _response.set(http::field::content_type,"text/plain"); 

            beast::ostream(_response.body()) << "url not found!\r\n";
            writeResponse();
            return ;
        } 

        _response.result(http::status::ok);
        _response.set(http::field::server,"GateServer");
        writeResponse();
    }
}

void HttpConnection::writeResponse(){
    auto self = shared_from_this(); 

    http::async_write(_socket,_response,[self](beast::error_code ec, size_t bytes){

        if(ec){
            std::cerr << "http write error: " << ec.message() << std::endl;
        }
        // 先取消定时器，防止随后定时器回调错误访问已关闭资源
        self->_deadline.cancel();
        // 发送方向关闭并关闭 socket（短连接） （断开发送端了）
        self->_socket.shutdown(tcp::socket::shutdown_send);

    });
}


void HttpConnection::checkDeadline(){
    auto self = shared_from_this(); 

    _deadline.async_wait([self](beast::error_code ec){

        // 没有错误才是超时，调用的回调
        if (!ec) {
            // 真正超时，关闭 socket
            self->_socket.close();
        }

        // if（ec） 终止 || 错误
    });
}

tcp::socket& HttpConnection::getSocket(){
    return _socket;
}



unsigned char ToHex(unsigned char x){
    return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x){
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}

// URL 编码  你好---> xxxx
std::string UrlEncode(const std::string& str){
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        //判断是否仅有数字和字母构成
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ') //为空字符
            strTemp += "+";
        else
        {
            //其他字符需要提前加%并且高四位和低四位分别转为16进制
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] & 0x0F);
        }
    }
    return strTemp;
}

// URL 解码  xxx---> 你好
std::string UrlDecode(const std::string& str){
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        //还原+为空
        if (str[i] == '+') strTemp += ' ';
        //遇到%将后面的两个字符从16进制转为char再拼接
        else if (str[i] == '%')
        {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}


void HttpConnection::parseGetParam(){
    // 提取 URI  
    auto uri = _request.target();
    // 查找查询字符串的开始位置（即 '?' 的位置）  
    auto query_pos = uri.find('?');
    if (query_pos == std::string::npos) {
        _getUrl = uri;
        return;
    }
    _getUrl = uri.substr(0, query_pos);
    std::string query_string = uri.substr(query_pos + 1);
    std::string key;
    std::string value;
    size_t pos = 0;
    while ((pos = query_string.find('&')) != std::string::npos) {
        auto pair = query_string.substr(0, pos);
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            key = UrlDecode(pair.substr(0, eq_pos)); // 假设有 url_decode 函数来处理URL解码  
            value = UrlDecode(pair.substr(eq_pos + 1));
            _getParams[key] = value;
        }
        query_string.erase(0, pos + 1);
    }
    // 处理最后一个参数对（如果没有 & 分隔符）  
    if (!query_string.empty()) {
        size_t eq_pos = query_string.find('=');
        if (eq_pos != std::string::npos) {
            key = UrlDecode(query_string.substr(0, eq_pos));
            value = UrlDecode(query_string.substr(eq_pos + 1));
            _getParams[key] = value;
        }
    }

}