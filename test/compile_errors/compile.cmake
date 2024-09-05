# Tries to compile given TARGET and writes its success and outputs to RESULTS_FILE.
#
# Variables needed:
#  BINARY_DIR, TARGET, CONFIG, RESULTS_FILE
cmake_minimum_required(VERSION 3.20)

execute_process(
    COMMAND cmake --build ${BINARY_DIR} --target ${TARGET} --config ${CONFIG}
    RESULT_VARIABLE exit_code
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)
set(compilation_success FALSE)
if(exit_code EQUAL 0)
    set(compilation_success TRUE)
endif()
file(WRITE ${RESULTS_FILE} "${compilation_success}\n${output}")