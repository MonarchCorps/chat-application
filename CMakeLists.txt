cmake_minimum_required(VERSION 4.2)
project(chat_application C)

set(CMAKE_C_STANDARD 99)

add_executable(xchat_server server_main.c server.c)
add_executable(xchat_client client_main.c client.c)