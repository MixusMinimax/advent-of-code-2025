add_executable(d10p1 main10p1.cpp)
target_link_libraries(d10p1 PRIVATE lexy)

add_executable(d10p2 main10p2.cpp)
target_link_libraries(d10p2 PRIVATE lexy libscip)
