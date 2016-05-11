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
    cvalue(work{ service },boost::asio::io_service::work);
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

class GCInterFace {
public:
    virtual ~GCInterFace()=default;
    virtual void aboutToDelete() {/*clear resouce before delete*/}
public:
    GCInterFace()=default;
    GCInterFace(const GCInterFace&)=default;
    GCInterFace&operator=(const GCInterFace&)=default;
    GCInterFace(GCInterFace&&)=default;
    GCInterFace&operator=(GCInterFace&&)=default;
};

class CplusplusThread {
    std::shared_ptr<boost::asio::io_service> io_service_;
    std::shared_ptr<std::thread::id> thread_id_;
    std::shared_ptr<std::list<std::function<void(void)>>> before_start_;
    CplusplusThread&operator=(const CplusplusThread&)=delete;
    CplusplusThread&operator=(CplusplusThread&&)=delete;
    CplusplusThread(const CplusplusThread&)=delete;
    CplusplusThread(CplusplusThread&&)=delete;
public:
    CplusplusThread() {
        before_start_=std::make_shared<std::list<std::function<void(void)>>>();
    }

    void deleteClass(GCInterFace*_class_){
        if (_class_==nullptr) { return; }
        _class_->aboutToDelete();
        post([_class_]() {delete _class_; });
    }

    ~CplusplusThread() {
        if (io_service_) { io_service_->stop(); }
    }
    std::thread::id threadID() const {
        if (bool(thread_id_)==false) { return std::this_thread::get_id(); }
        return *thread_id_;
    }

    template<typename _F_>
    void post(_F_&&_f_) {
        if (io_service_) {
            io_service_->post(std::forward<_F_>(_f_));
        }
        else {
            before_start_->push_back(std::forward<_F_>(_f_));
        }
    }

    void start() {
        io_service_=std::make_shared<boost::asio::io_service>();
        thread_id_=std::make_shared<std::thread::id>();

        auto run_function=[arg_thread_id=this->thread_id_,
            arg_io_service=this->io_service_,
            arg_before_start=this->before_start_
        ]()mutable {

            boost::asio::io_service::work work_lock{ *arg_io_service };
            {
                *arg_thread_id=std::this_thread::get_id();

                for (auto &i:*(arg_before_start)) { if (i) { i(); }; }
                arg_before_start->clear();
                arg_before_start.reset();
                arg_thread_id.reset();
            }
            arg_io_service->run();

        };

        std::thread varThread{ std::move(run_function) };
        varThread.detach();
    }
};

int main(int argc,char *argv[]) {

    CplusplusThread thread_;

    thread_.post([]() {std::cout<<"1"<<std::endl; });
    thread_.post([]() {std::cout<<"2"<<std::endl; });
    thread_.start();
    thread_.post([]() {std::cout<<"3"<<std::endl; });

    auto m=std::make_shared<std::recursive_mutex>();
    auto y=make_unique_lock_shared_mutex(m);
    auto x=std::move(y);

    {
        cvalue(service_thread{ service_function },std::thread);
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
