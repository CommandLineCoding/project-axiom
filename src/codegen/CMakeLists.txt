ADD_LIBRARY(axiom_codegen STATIC)

TARGET_SOURCES(axiom_codegen PRIVATE 
    codegen.cpp
)

TARGET_INCLUDE_DIRECTORIES(axiom_codegen PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

TARGET_COMPILE_FEATURES(axiom_codegen PUBLIC 
    cxx_std_23
)

SET_PROJECT_WARNINGS(axiom_codegen)