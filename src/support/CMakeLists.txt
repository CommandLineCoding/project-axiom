ADD_LIBRARY(axiom_support STATIC)

TARGET_SOURCES(axiom_support PRIVATE 
    error.cpp 
    source_location.cpp
)

TARGET_INCLUDE_DIRECTORIES(axiom_support PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

TARGET_COMPILE_FEATURES(axiom_support PUBLIC 
    cxx_std_23
)

SET_PROJECT_WARNINGS(axiom_support)