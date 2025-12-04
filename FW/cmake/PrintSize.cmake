function(print_size TARGET)
    set(OUT_NAME ${TARGET}.elf)
    set(HEX_NAME ${TARGET}.hex)
    set(BIN_NAME ${TARGET}.bin)
    set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${TARGET} SUFFIX ".elf")

    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND arm-none-eabi-size ${OUT_NAME}
            COMMENT "Size of ${OUT_NAME}")

    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex ${OUT_NAME} ${HEX_NAME}
            COMMENT "Generating ${HEX_NAME}")

    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Obinary ${OUT_NAME} ${BIN_NAME}
            COMMENT "Generating ${BIN_NAME}")
endfunction()
