cmake_minimum_required(VERSION 3.14)
project(ZenS)

set(CMAKE_CXX_STANDARD 14)

include_directories(..)

add_executable(ZenS
        in_SD/Air_Conditioner.infos
        in_SD/Curtain.infos
        in_SD/Light.infos
        in_SD/Television.infos
        Controler.cpp
        Controler.h
        Device.cpp
        Device.h
        Gest_Data.cpp
        Gest_Data.h
        Gest_Quantity_Data.cpp
        Gest_Quantity_Data.h
        Gesture.cpp
        Gesture.h
        Order.cpp
        Order.h
        Setting.h)
