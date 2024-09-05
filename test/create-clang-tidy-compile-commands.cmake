cmake_minimum_required(VERSION 3.20)

# Filters the `compile_commands.json` for compile commands used for clang-tidy checks
# (the `_clang_tidy_compile_commands` target).
# Results are written to `clang-tidy/compile_commands.json`.

file(STRINGS ${BINARY_DIR}/compile_commands.json commands NEWLINE_CONSUME ENCODING UTF-8)
string(JSON len LENGTH ${commands})
math(EXPR last "${len} - 1")
set(clang_tidy_commands "[]")

foreach(i RANGE ${last})
    string(JSON json GET ${commands} ${i})
    string(JSON output GET ${json} "output")
    string(FIND ${output} "_clang_tidy_compile_commands" pos)
    if(${pos} GREATER_EQUAL  0)
        string(JSON clang_tidy_commands SET ${clang_tidy_commands} ${i} ${json})
    endif()
endforeach()

file(WRITE ${BINARY_DIR}/clang-tidy/compile_commands.json ${clang_tidy_commands})