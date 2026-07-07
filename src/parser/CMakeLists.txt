ADD_LIBRARY(axiom_parser STATIC)

TARGET_SOURCES(axiom_parser PRIVATE 
    parser.cpp
)

TARGET_INCLUDE_DIRECTORIES(axiom_parser PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

TARGET_COMPILE_FEATURES(axiom_parser PUBLIC 
    cxx_std_23
)

SET_PROJECT_WARNINGS(axiom_parser)