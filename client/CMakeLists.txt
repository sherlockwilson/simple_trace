aux_source_directory(./src xclient_src)

add_executable(xclient ${xclient_src})

target_link_libraries(xclient PRIVATE
        xtrace
        simpletcp
        -lpthread -ldl -lcurl)