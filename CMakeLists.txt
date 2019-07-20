add_compile_options(-static)

aux_source_directory(./src src_dir)

add_library(xtrace STATIC ${src_dir})

target_link_libraries(xtrace PRIVATE
        opentracing_mocktracer
        opentracing
        zipkin_opentracing
        zipkin
        -lpthread -ldl -lcurl)