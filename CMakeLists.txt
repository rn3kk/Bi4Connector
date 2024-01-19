cmake_minimum_required(VERSION 3.5)

project(Bi4Connector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Bi4Connector src/main.cpp
  src/Peer.h src/Peer.cpp
  src/TaskHandler.h src/TaskHandler.cpp
  src/Logger.h
  src/Logger.cpp
  src/Session.h src/Session.cpp
  src/IdController.h src/IdController.cpp
  src/Msg.cpp
  src/Msg.h
  src/Server.h
  src/Server.cpp


)

include(FindPkgConfig)
pkg_search_module(systemd libsystemd IMPORTED_TARGET)
target_link_libraries(Bi4Connector PUBLIC PkgConfig::systemd)
install(TARGETS Bi4Connector
    #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
