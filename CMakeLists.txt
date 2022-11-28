cmake_minimum_required(VERSION 3.8)

# set the project name
project(tcp_test VERSION 1.0)

# specify the C++ standard
add_library(compiler_flags INTERFACE)

target_compile_features(compiler_flags INTERFACE cxx_std_17)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
target_compile_options(compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused;>>"
)

find_package (Threads)

add_subdirectory(lib)
list(APPEND EXTRA_LIBS socket_test)

enable_testing()

# add the executable
add_executable(tcp_client client/src/tcp_client.cc)

target_link_libraries(tcp_client PUBLIC ${EXTRA_LIBS} 
                                      ${compiler_flags}
)

target_include_directories(tcp_client PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           ${EXTRA_INCLUDES}
                           )

# add the executable
add_executable(tcp_server server/src/tcp_server.cc)

target_link_libraries(tcp_server PUBLIC ${EXTRA_LIBS} Threads::Threads
                                      ${compiler_flags}
)

target_include_directories(tcp_server PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           ${EXTRA_INCLUDES}
                           )

set (DESTINATION "/home/bofan/usr")
message (INFO " destination variable is ${DESTINATION}")
install(TARGETS tcp_client DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h"
  DESTINATION include
  )

