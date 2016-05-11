CONFIG += c++14
win32-msvc*{
CONFIG+=console
}

QT += core gui
QT += widgets

TARGET = _2BoostTest
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp

HEADERS  += MainWindow.hpp

include($$PWD/../boost_with_asio.pri)

CONFIG += precompile_header
win32-msvc*{
PRECOMPILED_HEADER  += $$PWD/private/_msvc_core_utility_prebuild.hpp
}else{
PRECOMPILED_HEADER  += $$PWD/private/_core_utility_prebuild.hpp
}
