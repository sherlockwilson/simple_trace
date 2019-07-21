aux_source_directory(./src simple_tcp_src)

add_library(simpletcp STATIC ${simple_tcp_src})

target_link_libraries(simpletcp PRIVATE -lpthread)