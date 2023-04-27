cmake_minimum_required(VERSION 2.8)
project(find_objects)

SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib64/pkcs11)
link_directories(/usr/lib64/pkcs11)

add_executable(find_objects find_objects.c)
target_link_libraries(find_objects :yubihsm_pkcs11.so)
