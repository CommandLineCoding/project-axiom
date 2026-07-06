ADD_LIBRARY(axiom_lexer STATIC)

TARGET_SOURCES(axiom_lexer PRIVATE 
    lexer.cpp
)

TARGET_INCLUDE_DIRECTORIES(axiom_lexer PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

TARGET_COMPILE_FEATURES(axiom_lexer PUBLIC 
    cxx_std_23
)

SET_PROJECT_WARNINGS(axiom_lexer)