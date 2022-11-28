add_library(socket_test linux_socket.cc echo.cc message_format.cc)
target_include_directories(socket_test 
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )

target_link_libraries(socket_test compiler_flags)

install(TARGETS socket_test DESTINATION lib)

install(FILES linux_socket.h socketfactory.h tcpsocketfactory.h echo.h DESTINATION include)


