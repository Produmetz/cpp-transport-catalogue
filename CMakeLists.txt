cmake_minimum_required(VERSION 3.2)


# set the project name and version
project(TransportCatalogue)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_COMPILER g++)

set(SOURCE ./transport-catalogue/main.cpp 
           ./transport-catalogue/transport_catalogue.cpp 
           ./transport-catalogue/svg.cpp 
           ./transport-catalogue/request_handler.cpp 
           ./transport-catalogue/map_renderer.cpp 
           ./transport-catalogue/json.cpp 
           ./transport-catalogue/json_reader.cpp 
           ./transport-catalogue/geo.cpp 
           ./transport-catalogue/domain.cpp)

add_executable(myExe ${SOURCE})

#cmake -G "MSYS Makefiles" -D CMAKE_CXX_COMPILER=g++ ..
#cmake -G "MinGW Makefiles" -D CMAKE_CXX_COMPILER=g++ ..