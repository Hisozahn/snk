cmake_minimum_required(VERSION 3.9)
project (snk C)

set(CMAKE_C_STANDARD 99)

add_executable(snk main.c snk.c snk_joint.c snk_snake.c snk_position.c tcp_app.c tcp_app.h tcp_util.c tcp_util.h)
add_executable(snk_client snk_client.c snk.c snk_joint.c snk_snake.c snk_position.c tcp_app.c tcp_app.h tcp_util.c tcp_util.h)
add_executable(snk_server snk_server.c snk.c snk_joint.c snk_snake.c snk_position.c tcp_app.c tcp_app.h tcp_util.c tcp_util.h)
target_link_libraries(snk Ws2_32.lib)
target_link_libraries(snk_client Ws2_32.lib)
target_link_libraries(snk_server Ws2_32.lib)
