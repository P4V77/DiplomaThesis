
# This file needs path to the linker script: LINKER_SCRIPT_PATH, LINKER_SCRIPT_NAME
# and name of the MCU: MCU_NAME

set(EMBEDDED_COMPILER_FLAGS_C
        -ffunction-sections
        -fdata-sections
        --specs=nano.specs)

set(EMBEDDED_COMPILER_FLAGS_CXX
        -fno-rtti
        -fno-exceptions
        -fno-non-call-exceptions
        -fno-threadsafe-statics
        -fno-unwind-tables
        -fno-use-cxa-atexit)
list(APPEND EMBEDDED_COMPILER_FLAGS_CXX ${EMBEDDED_COMPILER_FLAGS_C})

# -fstack-usage
# -fcallgraph-info=su,da

set(EMBEDDED_LINKER_FLAGS
        -specs=nano.specs
        -specs=nosys.specs
        -static
        -Wl,--gc-sections
        -Wl,--wrap=malloc
        -Wl,--undefined=exception_table
        -Wl,--print-memory-usage
        -Wl,-L${PROJECT_SOURCE_DIR}/${LINKER_SCRIPT_PATH}
        -Wl,-T${PROJECT_SOURCE_DIR}/${LINKER_SCRIPT_NAME})

set(MCU_FLAGS_CM0
        -mcpu=cortex-m0
        -mtune=cortex-m0
        -mlittle-endian
        -mthumb
        -mfloat-abi=soft)

set(MCU_FLAGS_CM0PLUS
        -mcpu=cortex-m0plus
        -mtune=cortex-m0plus
        -mlittle-endian
        -mthumb
        -mfloat-abi=soft)

set(MCU_FLAGS_CM4
        -mcpu=cortex-m4
        -mtune=cortex-m4
        -mlittle-endian
        -mthumb
        -mfloat-abi=soft)

set(MCU_FLAGS_CM4F
        -mcpu=cortex-m4
        -mtune=cortex-m4
        -mlittle-endian
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16)

set(MCU_FLAGS_CM33F
        -mcpu=cortex-m33
        -mtune=cortex-m33
        -mlittle-endian
        -mthumb
        -mabi=aapcs
        -mfp16-format=ieee)

add_library(mcu_cm0 INTERFACE)
target_compile_options(mcu_cm0 INTERFACE ${MCU_FLAGS_CM0}
        ${MCU_FLAGS_CM0}
        $<$<COMPILE_LANGUAGE:C>:${EMBEDDED_COMPILER_FLAGS_C}>
        $<$<COMPILE_LANGUAGE:CXX>:${EMBEDDED_COMPILER_FLAGS_CXX}>)
target_link_options(mcu_cm0 INTERFACE ${EMBEDDED_LINKER_FLAGS} ${MCU_FLAGS_CM0})
target_compile_definitions(mcu_cm0 INTERFACE __${MCU_NAME}__)

add_library(mcu_cm0plus INTERFACE)
target_compile_options(mcu_cm0plus INTERFACE
        ${MCU_FLAGS_CM0PLUS}
        $<$<COMPILE_LANGUAGE:C>:${EMBEDDED_COMPILER_FLAGS_C}>
        $<$<COMPILE_LANGUAGE:CXX>:${EMBEDDED_COMPILER_FLAGS_CXX}>)
target_link_options(mcu_cm0plus INTERFACE ${EMBEDDED_LINKER_FLAGS} ${MCU_FLAGS_CM0PLUS})
target_compile_definitions(mcu_cm0plus INTERFACE __${MCU_NAME}__)

add_library(mcu_cm4 INTERFACE)
target_compile_options(mcu_cm4 INTERFACE
        ${MCU_FLAGS_CM4}
        $<$<COMPILE_LANGUAGE:C>:${EMBEDDED_COMPILER_FLAGS_C}>
        $<$<COMPILE_LANGUAGE:CXX>:${EMBEDDED_COMPILER_FLAGS_CXX}>)
target_link_options(mcu_cm4 INTERFACE ${EMBEDDED_LINKER_FLAGS} ${MCU_FLAGS_CM4})
target_compile_definitions(mcu_cm4 INTERFACE __${MCU_NAME}__)

add_library(mcu_cm4f INTERFACE)
target_compile_options(mcu_cm4f INTERFACE
        ${MCU_FLAGS_CM4F}
        $<$<COMPILE_LANGUAGE:C>:${EMBEDDED_COMPILER_FLAGS_C}>
        $<$<COMPILE_LANGUAGE:CXX>:${EMBEDDED_COMPILER_FLAGS_CXX}>)
target_link_options(mcu_cm4f INTERFACE ${EMBEDDED_LINKER_FLAGS} ${MCU_FLAGS_CM4F})
target_compile_definitions(mcu_cm4f INTERFACE __${MCU_NAME}__)

add_library(mcu_cm33f INTERFACE)
target_compile_options(mcu_cm33f INTERFACE
        ${MCU_FLAGS_CM33F}
        $<$<COMPILE_LANGUAGE:C>:${EMBEDDED_COMPILER_FLAGS_C}>
        $<$<COMPILE_LANGUAGE:CXX>:${EMBEDDED_COMPILER_FLAGS_CXX}>)
target_link_options(mcu_cm33f INTERFACE ${EMBEDDED_LINKER_FLAGS} ${MCU_FLAGS_CM33F})
target_compile_definitions(mcu_cm33f INTERFACE __${MCU_NAME}__)
