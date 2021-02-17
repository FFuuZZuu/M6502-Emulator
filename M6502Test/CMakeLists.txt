add_executable(M6502Test src/main.cpp src/6502LoadRegisterTests.cpp src/6502StoreRegisterTests.cpp)
include_directories(${CMAKE_SOURCE_DIR}/M6502Lib)
target_link_libraries(M6502Test gtest)
target_link_libraries(M6502Test M6502Lib)

install(TARGETS M6502Test RUNTIME DESTINATION bin)