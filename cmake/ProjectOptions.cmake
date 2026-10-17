# Compiler settings shared by our own targets (not by third-party code).
# Link `project_options` PRIVATE into every engine, game and test target.

add_library(project_options INTERFACE)

if(MSVC)
    target_compile_options(project_options INTERFACE /W4 /permissive- /utf-8 /Zc:__cplusplus)
else()
    target_compile_options(project_options INTERFACE -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
endif()

# --- AddressSanitizer (Debug only) ------------------------------------------
set(GAME_ASAN_ACTIVE OFF)
if(GAME_ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GAME_ASAN_ACTIVE ON)
endif()

if(GAME_ASAN_ACTIVE)
    if(MSVC)
        # MSVC's ASan can't be combined with runtime checks (/RTC1, a CMake
        # Debug default) or incremental linking, so strip both. This must run
        # before the dependencies are added so they inherit the same flags.
        foreach(flags_var CMAKE_C_FLAGS_DEBUG CMAKE_CXX_FLAGS_DEBUG)
            string(REGEX REPLACE "/RTC[1csu]+" "" ${flags_var} "${${flags_var}}")
        endforeach()
        target_compile_options(project_options INTERFACE /fsanitize=address)
        target_link_options(project_options INTERFACE /INCREMENTAL:NO)
    else()
        target_compile_options(project_options INTERFACE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(project_options INTERFACE -fsanitize=address)
    endif()
    message(STATUS "AddressSanitizer: ON")
endif()

# MSVC's ASan runtime is a DLL. Copy it next to an executable so the program
# also starts outside a Developer Command Prompt (Explorer, CTest, the IDE).
function(game_copy_asan_runtime target)
    if(GAME_ASAN_ACTIVE AND MSVC)
        get_filename_component(msvc_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(asan_dll "${msvc_bin_dir}/clang_rt.asan_dynamic-x86_64.dll")
        if(EXISTS "${asan_dll}")
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${asan_dll}" "$<TARGET_FILE_DIR:${target}>"
                VERBATIM)
        endif()
    endif()
endfunction()
