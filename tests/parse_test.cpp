#include "server_ws.hpp"
#include "client_ws.hpp"
#include <iostream>
#include <cassert>

using namespace std;
using namespace SimpleWeb;

class SocketServerTest : public SocketServerBase<WS> {
public:
    SocketServerTest() : 
            SocketServerBase<WS>::SocketServerBase(8080) {}
            
    void accept() {}
    
    void parse_request_test() {
        std::shared_ptr<Connection> connection(new Connection(new WS(*io_service)));
        
        stringstream ss;
        ss << "GET /test/ HTTP/1.1\r\n";
        ss << "TestHeader: test\r\n";
        ss << "TestHeader2:test2\r\n";
        ss << "TestHeader3:test3a\r\n";
        ss << "TestHeader3:test3b\r\n";
        ss << "\r\n";
        
        parse_handshake(connection, ss);
        
        assert(connection->method=="GET");
        assert(connection->path=="/test/");
        assert(connection->http_version=="1.1");
        
        assert(connection->header.size()==4);
        auto header_it=connection->header.find("TestHeader");
        assert(header_it!=connection->header.end() && header_it->second=="test");
        header_it=connection->header.find("TestHeader2");
        assert(header_it!=connection->header.end() && header_it->second=="test2");
        
        header_it=connection->header.find("testheader");
        assert(header_it!=connection->header.end() && header_it->second=="test");
        header_it=connection->header.find("testheader2");
        assert(header_it!=connection->header.end() && header_it->second=="test2");
        
        auto range=connection->header.equal_range("testheader3");
        auto first=range.first;
        auto second=first;
        ++second;
        assert(range.first!=connection->header.end() && range.second!=connection->header.end() &&
               ((first->second=="test3a" && second->second=="test3b") ||
                (first->second=="test3b" && second->second=="test3a")));
    }
};

class SocketClientTest : public SocketClientBase<WS> {
public:
    SocketClientTest(const std::string& server_port_path) : SocketClientBase<WS>::SocketClientBase(server_port_path, 80) {}
    
    void connect() {}
    
    void constructor_parse_test1() {
        assert(path=="/test");
        assert(host=="test.org");
        assert(port==8080);
    }
    
    void constructor_parse_test2() {
        assert(path=="/test");
        assert(host=="test.org");
        assert(port==80);
    }
    
    void constructor_parse_test3() {
        assert(path=="/");
        assert(host=="test.org");
        assert(port==80);
    }
    
    void constructor_parse_test4() {
        assert(path=="/");
        assert(host=="test.org");
        assert(port==8080);
    }
    
    void parse_response_header_test() {
        connection=std::unique_ptr<Connection>(new Connection(new WS(*io_service)));
        
        stringstream ss;
        ss << "HTTP/1.1 200 OK\r\n";
        ss << "TestHeader: test\r\n";
        ss << "TestHeader2:test2\r\n";
        ss << "TestHeader3:test3a\r\n";
        ss << "TestHeader3:test3b\r\n";
        ss << "\r\n";
        
        parse_handshake(ss);
                
        assert(connection->header.size()==4);
        auto header_it=connection->header.find("TestHeader");
        assert(header_it!=connection->header.end() && header_it->second=="test");
        header_it=connection->header.find("TestHeader2");
        assert(header_it!=connection->header.end() && header_it->second=="test2");
        
        header_it=connection->header.find("testheader");
        assert(header_it!=connection->header.end() && header_it->second=="test");
        header_it=connection->header.find("testheader2");
        assert(header_it!=connection->header.end() && header_it->second=="test2");
        
        auto range=connection->header.equal_range("testheader3");
        auto first=range.first;
        auto second=first;
        ++second;
        assert(range.first!=connection->header.end() && range.second!=connection->header.end() &&
               ((first->second=="test3a" && second->second=="test3b") ||
                (first->second=="test3b" && second->second=="test3a")));
        
        connection.reset();
    }
};

int main() {
    SocketServerTest serverTest;
    serverTest.io_service=std::make_shared<boost::asio::io_service>();
    
    serverTest.parse_request_test();
    
    SocketClientTest clientTest("test.org:8080/test");
    clientTest.constructor_parse_test1();
    
    SocketClientTest clientTest2("test.org/test");
    clientTest2.constructor_parse_test2();
    
    SocketClientTest clientTest3("test.org");
    clientTest3.constructor_parse_test3();
    
    SocketClientTest clientTest4("test.org:8080");
    clientTest4.io_service=std::make_shared<boost::asio::io_service>();
    clientTest4.constructor_parse_test4();
    
    clientTest4.parse_response_header_test();
}
