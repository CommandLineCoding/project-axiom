FUNCTION(set_project_warnings target_name)
    IF(MSVC)
        TARGET_COMPILE_OPTIONS(${target_name} PRIVATE /W4 /permissive-)
    ELSE()
        TARGET_COMPILE_OPTIONS(${target_name} PRIVATE 
            -Wall -Wextra -Wpedantic
            -Wshadow -Wnon-virtual-dtor -Wold-style-cast
            -Wcast-align -Wunused -Woverloaded-virtual
            -Wconversion -Wsign-conversion -Wnull-dereference
            -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough
        )
    ENDIF()
ENDFUNCTION()