if (BUILD_SHARED_LIBS)
  set(OPENTRACING_MOCKTRACER_LIBRARY opentracing_mocktracer)
else()
  set(OPENTRACING_MOCKTRACER_LIBRARY opentracing_mocktracer-static)
endif()

add_executable(mocktracer_tracer_test tracer_test.cpp)
target_link_libraries(mocktracer_tracer_test ${OPENTRACING_MOCKTRACER_LIBRARY})
add_test(NAME mocktracer_tracer_test COMMAND mocktracer_tracer_test)

add_executable(mocktracer_tracer_factory_test tracer_factory_test.cpp)
target_link_libraries(mocktracer_tracer_factory_test ${OPENTRACING_MOCKTRACER_LIBRARY})
add_test(NAME mocktracer_tracer_factory_test COMMAND mocktracer_tracer_factory_test)

add_executable(mocktracer_propagation_test propagation_test.cpp)
target_link_libraries(mocktracer_propagation_test ${OPENTRACING_MOCKTRACER_LIBRARY})
add_test(NAME mocktracer_propagation_test COMMAND mocktracer_propagation_test)

add_executable(mocktracer_json_test json_test.cpp)
target_link_libraries(mocktracer_json_test ${OPENTRACING_MOCKTRACER_LIBRARY})
add_test(NAME mocktracer_json_test COMMAND mocktracer_json_test)
