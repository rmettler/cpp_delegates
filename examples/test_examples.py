from pathlib import Path
import subprocess
import sys

example_cpp = Path(sys.argv[1])
example_exe = Path(sys.argv[2])
expected_output_file = Path(sys.argv[3])
expected_output = expected_output_file.read_text()

result = subprocess.run(
    [example_exe],
    capture_output=True,
    text=True,
)

if result.returncode != 0:
    print(
        f" [ \033[31mFAIL\033[0m ] \033[31mRunning example 'examples/{example_cpp}' returned with exit code {result.returncode}.\033[0m"
    )
    exit(-1)

if result.stdout != expected_output:
    print(
        f" [ \033[31mFAIL\033[0m ] \033[31mStdout of 'examples/{example_cpp}' does not match with the expected "
        f"output in 'examples/{expected_output_file}'.\033[0m"
    )
    print("\033[31mReceived stdout:")
    print(result.stdout)
    print("\033[31mExpected stdout:")
    print(expected_output + "\033[0m")
    exit(-1)

print(f" [  \033[32mOK\033[0m  ] examples/{example_cpp}")
