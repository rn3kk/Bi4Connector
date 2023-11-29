cmake_minimum_required(VERSION 3.5)

project(Bi4Connector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Bi4Connector src/main.cpp
  src/Peer.h src/Peer.cpp
  src/TaskHandler.h src/TaskHandler.cpp
  src/Session.h src/Session.cpp


)

#include(GNUInstallDirs)
install(TARGETS Bi4Connector
    #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
