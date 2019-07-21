aux_source_directory(./src xserver_src)

add_executable(xserver ${xserver_src})

target_link_libraries(xserver PRIVATE
        xtrace
        simpletcp
        -lpthread -ldl -lcurl)