#include "MainWindow.hpp"
#include <QtWidgets/QApplication>

#include "global_cplusplus.hpp"
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

cvalue(service,boost::asio::io_service);
auto service_function=[]() {
    cvalue(work{service},boost::asio::io_service::work);
    service.run();
};

template<typename _T_/*std::shared_ptr*/>
class SharedLock {
    using _I_=typename _T_::element_type;
    cvalue(data_,_T_);
    cvalue(lock_=cexpr(nullptr),std::unique_lock<_I_>*);
public:
    SharedLock() {}
    SharedLock(decltype(nullptr)) {}
    SharedLock(const SharedLock &)=delete;
    SharedLock(SharedLock &&_o_) { 
        std::swap(this->lock_,_o_.lock_);
        std::swap(this->data_,_o_.data_);
    }
    SharedLock&operator=(const SharedLock &)=delete;
    SharedLock&operator=(SharedLock &&_o_) {
        if (this==&_o_) { return *this; }
        std::swap(this->lock_,_o_.lock_);
        std::swap(this->data_,_o_.data_);
        return *this;
    }
    ~SharedLock() { delete lock_; }
public:
    template<typename __T_,typename ... _Args_>
    SharedLock(__T_&&_v_,_Args_&&..._args_):data_(std::forward<__T_>(_v_)) {
        if (bool(data_)==false) { return; }
        lock_=new std::unique_lock<_I_>(*data_,
            std::forward<_Args_>(_args_)...);
    }
};

template<typename _T_,typename ... _Args_>
auto make_unique_lock_shared_mutex(_T_&&_v_,_Args_&&..._args_) {
    using _A_=std::remove_const_t<std::remove_reference_t<_T_>>;
    return SharedLock<_A_>(std::forward<_T_>(_v_),
        std::forward<_Args_>(_args_)...);
}

int main(int argc, char *argv[])
{

    auto m=std::make_shared<std::recursive_mutex>();
    auto y=make_unique_lock_shared_mutex(m);
    auto x=std::move(y);

    {
        cvalue(service_thread{service_function},std::thread);
        service_thread.detach();
    }
    cvalue(main_ans=-9999,int);
    
    {

        QApplication app(argc,argv);
        std::cout<<std::this_thread::get_id()<<std::endl;

        service.post([]() {
            std::cout<<std::this_thread::get_id()<<std::endl;
        });

        MainWindow window;
        window.show();

        main_ans=app.exec();
    }

    service.stop();
    return main_ans;
}
