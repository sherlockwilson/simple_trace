if (BUILD_MOCKTRACER AND BUILD_SHARED_LIBS)
  include_directories(../../mocktracer/include)
  add_executable(tutorial-example tutorial-example.cpp)
  target_link_libraries(tutorial-example  opentracing_mocktracer)
  add_test(NAME tutorial-example COMMAND tutorial-example)
endif()
