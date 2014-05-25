#
add_executable(MyEchoServer MyEchoServer/main.c)
target_link_libraries(MyEchoServer event_extra ${LIB_APPS} ${LIB_PLATFORM})
#add_dependencies(MyEchoServer event_extra)

add_executable(rco13Server MyEchoServer/rco13server.c)
target_link_libraries(rco13Server event_extra ${LIB_APPS} ${LIB_PLATFORM})

