function(assemble_rule target)
    set(inputVar "${MP_CC};-c;${MP_EXTRA_AS_PRE};-mcpu=ATtiny1627;-x;assembler-with-cpp;-D__ATtiny1627__;-mdfp=C:/Users/richa/.mchp_packs/Microchip/ATtiny_DFP/3.1.260/xc8;-Wl,--gc-sections;-O0;-ffunction-sections;-fdata-sections;-fshort-enums;-fno-common;-funsigned-char;-funsigned-bitfields;-Wall;-DXPRJ_default=default;-gdwarf-3;-mno-const-data-in-progmem;-Wa,--defsym=__MPLAB_BUILD=1;${INSTRUMENTED_TRACE_OPTIONS}")
    string(REGEX REPLACE "[,]+" "," noDoubleCommas "${inputVar}")
    string(REGEX REPLACE ",$" "" noDanglingCommas "${noDoubleCommas}")
    target_compile_options(${target} PRIVATE "${noDanglingCommas}")
endfunction()
function(assemblePreprocess_rule target)
    set(inputVar "-c;${MP_EXTRA_AS_PRE};-mcpu=ATtiny1627;-x;assembler-with-cpp;-D__ATtiny1627__;-mdfp=C:/Users/richa/.mchp_packs/Microchip/ATtiny_DFP/3.1.260/xc8;-Wl,--gc-sections;-O0;-ffunction-sections;-fdata-sections;-fshort-enums;-fno-common;-funsigned-char;-funsigned-bitfields;-Wall;-DXPRJ_default=default;-gdwarf-3;-mno-const-data-in-progmem;-Wa,--defsym=__MPLAB_BUILD=1;${INSTRUMENTED_TRACE_OPTIONS}")
    string(REGEX REPLACE "[,]+" "," noDoubleCommas "${inputVar}")
    string(REGEX REPLACE ",$" "" noDanglingCommas "${noDoubleCommas}")
    target_compile_options(${target} PRIVATE "${noDanglingCommas}")
endfunction()
function(compile_rule target)
    set(inputVar "${MP_CC};${MP_EXTRA_CC_PRE};-mcpu=ATtiny1627;-c;-x;c;-D__ATtiny1627__;-mdfp=C:/Users/richa/.mchp_packs/Microchip/ATtiny_DFP/3.1.260/xc8;-Wl,--gc-sections;-O0;-ffunction-sections;-fdata-sections;-fshort-enums;-fno-common;-funsigned-char;-funsigned-bitfields;-Wall;-DXPRJ_default=default;-gdwarf-3;-mno-const-data-in-progmem;${INSTRUMENTED_TRACE_OPTIONS};${FUNCTION_LEVEL_PROFILING_OPTIONS}")
    string(REGEX REPLACE "[,]+" "," noDoubleCommas "${inputVar}")
    string(REGEX REPLACE ",$" "" noDanglingCommas "${noDoubleCommas}")
    target_compile_options(${target} PRIVATE "${noDanglingCommas}")
endfunction()
function(link_rule target)
    set(inputVar "${MP_EXTRA_LD_PRE};-mcpu=ATtiny1627;-Wl,-Map=mem.map;-DXPRJ_default=default;-Wl,--defsym=__MPLAB_BUILD=1;-mdfp=C:/Users/richa/.mchp_packs/Microchip/ATtiny_DFP/3.1.260/xc8;-Wl,--gc-sections;-O0;-ffunction-sections;-fdata-sections;-fshort-enums;-fno-common;-funsigned-char;-funsigned-bitfields;-Wall;-gdwarf-3;-mno-const-data-in-progmem;${INSTRUMENTED_TRACE_OPTIONS};${FUNCTION_LEVEL_PROFILING_OPTIONS};-Wl,--memorysummary,memoryfile.xml,;-Wl,--start-group;${LIBRARIES};-Wl,--end-group")
    string(REGEX REPLACE "[,]+" "," noDoubleCommas "${inputVar}")
    string(REGEX REPLACE ",$" "" noDanglingCommas "${noDoubleCommas}")
    target_link_options(${target} PRIVATE "${noDanglingCommas}")
endfunction()
