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

#include <stdexcept>
#include <QtCore/qdebug.h>
#include <boost/asio.hpp>
#include <list>
#include <vector>
#include <algorithm>
#include <atomic>
#include <mutex>

using namespace std::string_literals;

class AsioTypeTraits {
public:
    template<typename _T_>
    using shared_ptr_type=std::shared_ptr<_T_>;
    template<typename _T_>
    using weak_ptr_type=std::weak_ptr<_T_>;
    template<typename _T_>
    using enable_shared_from_this_type=std::enable_shared_from_this<_T_>;
    template<typename _T_>
    using function_type=std::function<_T_>;
    using thread_type=std::thread;
    using thread_id_type=std::thread::id;
    static thread_id_type get_this_thread_id() { return std::this_thread::get_id(); }
    using atomic_bool_type=std::atomic<bool>;
    using tcp_type=boost::asio::ip::tcp;
    using tcp_acceptor_type=tcp_type::acceptor;
    using tcp_endpoint_type=tcp_type::endpoint;
    static tcp_endpoint_type get_default_endpoint() { return{tcp_type::v4(),8001}; }
    using io_service_type=boost::asio::io_service;
    using tcp_socket_type=tcp_type::socket;
    using io_service_work_type=io_service_type::work;
    using mutex_type=std::recursive_mutex;
    using mutex_lock_type=std::unique_lock<mutex_type>;
    using size_type=std::size_t;
    using error_code_type=boost::system::error_code;
    template<typename ..._Args_>
    static decltype(auto) buffer(_Args_&&..._args_) { return boost::asio::buffer(std::forward<_Args_>(_args_)...); }
    template<typename _T_,typename ..._Args_>
    static shared_ptr_type<_T_>make_shared(_Args_&&..._args_) {
        return shared_ptr_type<_T_>(new _T_(std::forward<_Args_>(_args_)...));
    }
   
public:
    AsioTypeTraits()=default;
    virtual ~AsioTypeTraits()=default;
protected:
    AsioTypeTraits&operator=(const AsioTypeTraits&)=default;
    AsioTypeTraits(const AsioTypeTraits&)=default;
    AsioTypeTraits(AsioTypeTraits&&)=default;
    AsioTypeTraits&operator=(AsioTypeTraits&&)=default;
};

class AsioService :
    public AsioTypeTraits::enable_shared_from_this_type<AsioService>,
    public AsioTypeTraits {
public:
    static auto default_run_function()->const shared_ptr_type<const function_type<void(void)>> &{
        static const shared_ptr_type<const function_type<void(void)>>ans{ new
            function_type<void(void)>{[]() {}}
        };
        return ans;
    }
protected:
    shared_ptr_type<io_service_type> service_;
    atomic_bool_type is_started_{ false };
    shared_ptr_type<const function_type<void(void)>> run_function_{ default_run_function() };
    thread_id_type thread_id_;
public:
    AsioService()=default;
    template<typename _T_>
    AsioService(_T_&&arg) { setRunFunction(std::forward<_T_>(arg)); }
    static shared_ptr_type<AsioService> instance() {
        return shared_ptr_type<AsioService>(new AsioService,
            [](AsioService * arg) {delete arg; });
    }

public:
    auto threadID() const->const thread_id_type& { return thread_id_; }
    auto isStarted() const->const atomic_bool_type&{ return is_started_; }
    auto service()const->const shared_ptr_type<io_service_type>&{ return service_; }
    void start() {
        if (is_started_.load()) { return; }
        service_=shared_ptr_type<io_service_type>{ new io_service_type };
        thread_type var_thread=thread_type{ [this]() {this->_p_thread_function(); } };
        var_thread.detach();
        is_started_.store(true);
    }
    void stop() {
        if (is_started_.load()==false) { return; }
        if (service_) { service_->stop(); }
        is_started_.store(false);
    }
public:
    auto runFunction()const->const shared_ptr_type<const function_type<void(void)>> & {
        if (bool(run_function_)&&(*run_function_)) { return run_function_; }
        return default_run_function();
    }
    auto getRunFunction()const->const shared_ptr_type<const function_type<void(void)>> & { return runFunction(); }
    void setRunFunction(shared_ptr_type<const function_type<void(void)>> arg) {
        if (is_started_.load()) {
            error("the service has run!!");
            return;
        }
        if (bool(arg)&&(*arg)) { run_function_=std::move(arg); }
        else { run_function_=default_run_function(); }
    }
    void setRunFunction(const function_type<void(void)> arg) {
        if (is_started_.load()) {
            error("the service has run!!");
            return;
        }
        if (bool(arg)) {
            run_function_=
                shared_ptr_type<const function_type<void(void)>>(
                    new auto(std::move(arg)));
        }
        else { run_function_=default_run_function(); }
    }
protected:
    virtual ~AsioService() {
        if (is_started_.load()==false) {}
        else { this->stop(); }
    }
    virtual void error(const char *) {}

private:
    void _p_thread_function() try{
        weak_ptr_type<AsioService> var_service_=this->shared_from_this();
        try{
            this->thread_id_=get_this_thread_id();
            auto run_function_lock=run_function_;
            auto var_service_lock=service_;
            io_service_work_type work{ *var_service_lock };
            this->_p_run();
            var_service_lock->run();
        }
        catch (const std::exception &e) { 
            auto var_service=var_service_.lock();
            if (false==var_service) { 
                qDebug()<<e.what();return;
            }
            var_service->error(e.what());
        }
        catch (...) { 
            auto var_service=var_service_.lock();
            if (false==var_service) { 
                qDebug()<<"unknow exception ???";return;
            }
            var_service->error("unknow exception ???"); 
        }
    }
    catch (std::exception&e) {
        qDebug()<<"?may start in construct?"<<e.what();
    }
    
    void _p_run() { if (bool(run_function_)&&(*run_function_)) { (*run_function_)(); } }
};

class TcpService :public AsioTypeTraits{
public:

    class TcpSocket :public AsioTypeTraits{
        tcp_socket_type socket_;
    public:
        TcpSocket(const shared_ptr_type<io_service_type>&arg):socket_(*arg) {}
        ~TcpSocket() {}

        tcp_socket_type & socket() { return socket_; }
        const tcp_socket_type & socket() const{ return socket_; }

    };

    class TcpAcceptorService:public AsioService{
        TcpService * super_;
        shared_ptr_type<tcp_acceptor_type> acceptor_;
    public:
        TcpAcceptorService(TcpService *arg):super_(arg) {
            setRunFunction([this]() {this_run_function(); });
        }

        static auto instance(TcpService *arg)->shared_ptr_type<TcpAcceptorService> {
            return shared_ptr_type<TcpAcceptorService> {
                new TcpAcceptorService(arg),[](auto *arg) {delete arg; }
            };
        }

    protected:
        void this_run_function() {
            acceptor_=shared_ptr_type<tcp_acceptor_type>{
                new tcp_acceptor_type{*service(),super_->endpoint()}
            };
            next();
        }
        ~TcpAcceptorService() {}
        void next() {
            auto var_socket=super_->genASocket();
            weak_ptr_type<AsioService> var_this_watcher=shared_from_this();
            acceptor_->async_accept(
                var_socket->socket(),
                [var_socket,var_this_watcher,this](const error_code_type&e) {
                if (e) { 
                    auto var_message=e.message(); 
                    qDebug()<<var_message.c_str();
                }
                else {
                    var_socket->socket().async_write_some(
                        buffer("okay"s),[](const error_code_type&,auto) {}
                    );
                }
                if (var_this_watcher.lock()) { this->next(); }
            });
            
        }
    };

    class TcpSocketService:public AsioService {
        TcpService * super_;
        shared_ptr_type<mutex_type> sockets_mutex_;
        std::list<shared_ptr_type<TcpSocket>> sockets_;
    public:
        TcpSocketService(TcpService *arg):super_(arg) {
            sockets_mutex_=shared_ptr_type<mutex_type>{new mutex_type};
            setRunFunction([this]() {this_run_function(); });
        }

        static auto instance(TcpService *arg)->shared_ptr_type<TcpSocketService> {
            return shared_ptr_type<TcpSocketService> {
                new TcpSocketService(arg),[](auto *arg) {delete arg; }
            };
        }

        size_type socketsSize()const {
            mutex_lock_type varLock{*sockets_mutex_};
            return sockets_.size();
        }

        void addASocket(shared_ptr_type<TcpSocket>arg){
            mutex_lock_type varLock{*sockets_mutex_};
            sockets_.push_back(std::move(arg));
        }

        void clear() {
            mutex_lock_type varLock{*sockets_mutex_};
            sockets_.clear();
        }
    protected:
        void this_run_function() {}
    };

protected:
    tcp_endpoint_type endpoint_;
    std::int32_t socket_thread_size_=1;
    shared_ptr_type<TcpAcceptorService> tcp_acceptor_service_;
    shared_ptr_type<mutex_type> tcp_socket_services_mutex_;
    std::vector<shared_ptr_type<TcpSocketService>> tcp_socket_services_;
public:
    const tcp_endpoint_type &endpoint() const {return endpoint_; }
public:
    TcpService(
        const tcp_endpoint_type & arg_endpoint=get_default_endpoint(),
        std::int32_t arg_socket_thread_size=3
    ) :endpoint_(arg_endpoint){
        tcp_socket_services_mutex_=shared_ptr_type<mutex_type>(new mutex_type);
        /*************************************/
        arg_socket_thread_size=std::max(1,arg_socket_thread_size);
        socket_thread_size_=arg_socket_thread_size;
        tcp_socket_services_.reserve(arg_socket_thread_size);
        for (std::int32_t i=0; i<arg_socket_thread_size;++i) {
            tcp_socket_services_.push_back(TcpSocketService::instance(this));
        }
        /*************************************/
        tcp_acceptor_service_=TcpAcceptorService::instance(this);
        /*************************************/
        for (auto & i:tcp_socket_services_) {
            i->start();
        }
        tcp_acceptor_service_->start();
    }

    ~TcpService() {
        auto mutex_data_lock_=tcp_socket_services_mutex_;
        mutex_lock_type tcp_socket_services_mutex_lock_(*mutex_data_lock_);
        tcp_acceptor_service_->stop();
        for (auto & i:tcp_socket_services_) { 
            i->stop(); i->clear();
        }
    }

public:
    shared_ptr_type<TcpSocket> genASocket() { 
        using ItemType=std::pair<size_type,shared_ptr_type<TcpSocketService>>;
        std::vector<ItemType> varItems;
        {
            mutex_lock_type tcp_socket_services_mutex_lock_(*tcp_socket_services_mutex_);
            varItems.reserve(tcp_socket_services_.size());
            for (auto & i:tcp_socket_services_) {
                varItems.emplace_back(i->socketsSize(),i);
            }
        }
        std::sort(varItems.begin(),varItems.end(),
            [](auto &l,auto &r) {return l.first<r.first; });
      
        auto varSocket=shared_ptr_type<TcpSocket>{new
            TcpSocket{ varItems[0].second->service() }};
        varItems[0].second->addASocket(varSocket);
        return std::move(varSocket);
    }

};

int main(int argc,char *argv[]) {
    int main_ans=-9999;
    {
       
        QApplication app(argc,argv);
        TcpService tcpservice;
        
        {
           
            MainWindow window;
            window.show();

            main_ans=app.exec();
        }
    }
    return main_ans;
}
