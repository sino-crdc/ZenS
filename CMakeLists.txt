cmake_minimum_required(VERSION 3.14)
project(ZenS)

set(CMAKE_CXX_STANDARD 14)

include_directories("Bracelet Main")

add_executable(ZenS
        "Bracelet Main/Controler.cpp"
        "Bracelet Main/Controler.h"
        "Bracelet Main/Device.cpp"
        "Bracelet Main/Device.h"
        "Bracelet Main/Gest_Data.cpp"
        "Bracelet Main/Gest_Data.h"
        "Bracelet Main/Gest_Quantity_Data.cpp"
        "Bracelet Main/Gest_Quantity_Data.h"
        "Bracelet Main/Gesture.cpp"
        "Bracelet Main/Gesture.h"
        "Bracelet Main/Order.cpp"
        "Bracelet Main/Order.h"
<<<<<<< HEAD
        "Bracelet Main/Setting.h" Bracelet_Main/helloworld.cpp)
=======
        "Bracelet Main/Setting.h")
>>>>>>> 71f2f733c9af014b43d4987d8e79de4dde1f397d
