#static import
INCLUDEPATH += $$PWD

DEFINES += BOOST_HAS_TR1
DEFINES += BOOST_SP_USE_STD_ATOMIC
DEFINES += BOOST_ALL_NO_LIB

linux-g++*{
QMAKE_CXXFLAGS+=-Wno-ignored-qualifiers
QMAKE_CXXFLAGS+=-Wno-deprecated-declarations
}

win32-g++*{
QMAKE_CXXFLAGS+=-Wno-ignored-qualifiers
QMAKE_CXXFLAGS+=-Wno-deprecated-declarations
}

win32:{
#boost filesystem
LIBS += -lAdvapi32
DEFINES *=_WIN32_WINNT=0x0501
}

win32-g++*{
LIBS += -lws2_32
LIBS += -lwsock32
}

SOURCES += $$PWD/boostLibs/boost_c_regex_traits.cpp
SOURCES += $$PWD/boostLibs/boost_chrono.cpp
SOURCES += $$PWD/boostLibs/boost_codecvt_error_category.cpp
SOURCES += $$PWD/boostLibs/boost_cpp_regex_traits.cpp
SOURCES += $$PWD/boostLibs/boost_cregex.cpp
SOURCES += $$PWD/boostLibs/boost_date_generators.cpp
SOURCES += $$PWD/boostLibs/boost_error_code.cpp
SOURCES += $$PWD/boostLibs/boost_fileiter.cpp
SOURCES += $$PWD/boostLibs/boost_greg_month.cpp
SOURCES += $$PWD/boostLibs/boost_greg_weekday.cpp
SOURCES += $$PWD/boostLibs/boost_gregorian_types.cpp
SOURCES += $$PWD/boostLibs/boost_icu.cpp
SOURCES += $$PWD/boostLibs/boost_instances.cpp
SOURCES += $$PWD/boostLibs/boost_operations.cpp
SOURCES += $$PWD/boostLibs/boost_path.cpp
SOURCES += $$PWD/boostLibs/boost_path_traits.cpp
SOURCES += $$PWD/boostLibs/boost_portability.cpp
SOURCES += $$PWD/boostLibs/boost_posix_api.cpp
SOURCES += $$PWD/boostLibs/boost_posix_time_types.cpp
SOURCES += $$PWD/boostLibs/boost_process_cpu_clocks.cpp
SOURCES += $$PWD/boostLibs/boost_regex.cpp
SOURCES += $$PWD/boostLibs/boost_regex_debug.cpp
SOURCES += $$PWD/boostLibs/boost_regex_raw_buffer.cpp
SOURCES += $$PWD/boostLibs/boost_regex_traits_defaults.cpp
SOURCES += $$PWD/boostLibs/boost_sp_collector.cpp
SOURCES += $$PWD/boostLibs/boost_sp_debug_hooks.cpp
SOURCES += $$PWD/boostLibs/boost_static_mutex.cpp
SOURCES += $$PWD/boostLibs/boost_thread_clock.cpp
SOURCES += $$PWD/boostLibs/boost_unique_path.cpp
SOURCES += $$PWD/boostLibs/boost_usinstances.cpp
SOURCES += $$PWD/boostLibs/boost_utf8_codecvt_facet.cpp
SOURCES += $$PWD/boostLibs/boost_w32_regex_traits.cpp
SOURCES += $$PWD/boostLibs/boost_wc_regex_traits.cpp
SOURCES += $$PWD/boostLibs/boost_wide_posix_api.cpp
SOURCES += $$PWD/boostLibs/boost_windows_file_codecvt.cpp
SOURCES += $$PWD/boostLibs/boost_winstances.cpp


