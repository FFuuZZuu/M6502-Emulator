add_library(M6502Lib src/m6502.cpp)

install(TARGETS M6502Lib
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)