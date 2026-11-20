# Version of the forced compilers, shared by both toolchain files
#
# The toolchain files skip the compiler detection (CMAKE_<LANG>_COMPILER_FORCED), so CMake does not know the compiler
# version. Without it CMake knows neither the -std options nor the default standard of the compiler and never turns
# C_STANDARD/CXX_STANDARD and C_EXTENSIONS/CXX_EXTENSIONS into -std=..., every target then builds with the default
# standard of the compiler. With a forced compiler CMake derives the default standard from the version
# (Compiler/GNU-<LANG>.cmake), so this file only sets CMAKE_<LANG>_COMPILER_VERSION for C and CXX from -dumpfullversion.
# If a compiler gives no answer, the version stays unset and CMake adds no -std (as before) with a warning.

function(_sb_set_compiler_version lang)
    execute_process(COMMAND "${CMAKE_${lang}_COMPILER}" -dumpfullversion
                    OUTPUT_VARIABLE version
                    RESULT_VARIABLE result
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)
    if(NOT result EQUAL 0 OR NOT version MATCHES "^[0-9]+[.][0-9]+")
        message(WARNING "Could not determine the version of ${CMAKE_${lang}_COMPILER}. "
                        "CMake does not know its default standard and ignores ${lang}_STANDARD and ${lang}_EXTENSIONS.")
        return()
    endif()
    set(CMAKE_${lang}_COMPILER_VERSION "${version}" PARENT_SCOPE)
endfunction()

_sb_set_compiler_version(C)
_sb_set_compiler_version(CXX)
