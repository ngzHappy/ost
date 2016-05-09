#include "MainWindow.hpp"
#include <QtWidgets/QApplication>

#include <thread>
#include <functional>
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <string>
#include <iostream>

#include <QtCore/qdebug.h>
#include <boost/asio.hpp>
#include <list>
class Service {
    Service(const Service &)=delete;
    Service(Service&&)=delete;
    Service&operator=(const Service &)=delete;
    Service&operator=(Service&&)=delete;
    std::shared_ptr<boost::asio::io_service> service/*服务器*/;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor/*监听者*/;
    std::list<std::shared_ptr<boost::asio::ip::tcp::socket>> sockets/*接受的客户端*/;
    std::thread thread;
public:
    Service() { 
        thread=std::thread([this]() {this->run(); });
    }
    static std::shared_ptr<Service> instance() {
        return std::shared_ptr<Service>(
            new Service,[](Service *arg) {delete arg; });}
protected:
    virtual ~Service() {
        if (service) { service->stop(); }
        thread.join();
    }
    virtual void next() {
        using namespace boost::asio::ip;
        auto socket=std::make_shared<tcp::socket>(*service);
        acceptor->async_accept(
            *socket,
            [socket,this](boost::system::error_code e) {
            if (e) { qDebug().noquote()<<e.message().c_str(); }
            else {
                sockets.push_back(socket);
                socket->async_write_some(
                    boost::asio::buffer("xxxx"),
                    [socket](boost::system::error_code e,std::size_t) {
                    if (e) { qDebug()<<"write error!"; }
                });
                qDebug()<<sockets.size();
            }
            next();
        });
    }
    virtual void run() {
        service=std::make_shared<boost::asio::io_service>();
        const auto service_lock=service;
        using namespace boost::asio::ip;
        acceptor=std::make_shared<tcp::acceptor>(*service,tcp::endpoint(tcp::v4(),8001));
        next();
        service->run();
    }
    
};



int main(int argc, char *argv[])
{
    int main_ans=-9999;
    {
        auto service=Service::instance();
        QApplication app(argc,argv);
       
        MainWindow window;
        window.show();

        main_ans=app.exec();
    }
    return main_ans;
}
