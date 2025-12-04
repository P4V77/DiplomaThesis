set(COMPILER_WARNINGS_C
    -ggdb
    -Wall
    -Wextra
    -Wunused
    -Wnull-dereference
    -Wduplicated-cond
    -Wduplicated-branches
    -Wshadow
    -Wcast-align
    -Wconversion
    -Wsign-conversion
    -Wdouble-promotion
    -Wfloat-conversion
    -Wformat=2
    -Wmisleading-indentation
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wimplicit-fallthrough
    -Wno-unknown-pragmas
)

set(COMPILER_WARNINGS_CXX
    -Wpedantic
    #-Wnon-virtual-dtor
    -Woverloaded-virtual
    -Wno-volatile
    #-Wold-style-cast
    #-Wuseless-cast
)

list(APPEND COMPILER_WARNINGS_CXX ${COMPILER_WARNINGS_C})

# Useful for monitoring stack usage
#-Wstack-usage=128
#-Wlarger-than=1024

add_library(warnings INTERFACE)
target_compile_options(warnings INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${COMPILER_WARNINGS_C}>
        $<$<COMPILE_LANGUAGE:CXX>:${COMPILER_WARNINGS_CXX}>
)
