aux_source_directory(./src xedge_src)

add_executable(xedge ${xedge_src})

target_link_libraries(xedge PRIVATE
        xtrace
        simpletcp
        -lpthread -ldl -lcurl)