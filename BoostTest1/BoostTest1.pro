CONFIG += c++14
win32-msvc*{
CONFIG+=console
}

QT += core gui
QT += widgets

TARGET = _000001BoostTest
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp

HEADERS  += MainWindow.hpp

include($$PWD/../boost_with_asio.pri)
