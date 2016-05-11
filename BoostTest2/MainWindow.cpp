#include "MainWindow.hpp"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
}

bool MainWindow::event(QEvent *e) {
    if (e->type()==QEvent::None) {
        std::cout<<"???"<<std::endl;
        return true;
    }
    return QMainWindow::event(e);
}

MainWindow::~MainWindow()
{

}
