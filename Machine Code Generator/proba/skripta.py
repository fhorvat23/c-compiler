import os
import subprocess
import re
import shutil

def print_colored(text, color, bold=False):
    colors = {
        "green": "\033[92m",  # Green
        "bright_green": "\033[32;1m",
        "dark_green": "\033[32m",
        "red": "\033[91m",    # Red
        "yellow": "\033[93m",  # Yellow
        "bold": "\033[1m",    # Bold
        "reset": "\033[0m"    # Reset to default color
    }
    style = colors["bold"] if bold else ""
    print(f"{colors[color]}{text}{colors['reset']}")

ROOT_DIR = os.getcwd()
TESTS_DIR = os.path.join(ROOT_DIR, "tests")
GENERATOR_SRC = os.path.join(ROOT_DIR, "generator.cpp")
GENERATOR_EXE = os.path.join(ROOT_DIR, "generator.exe")
FRISC_FILE = os.path.join(ROOT_DIR, "a.frisc")

# Step 1: Compile generator.cpp
print("Compiling generator.cpp...")
compile_result = subprocess.run(["g++", "-O2", GENERATOR_SRC, "-o", GENERATOR_EXE], capture_output=True, text=True)

if compile_result.returncode != 0:
    print("Compilation failed:")
    print(compile_result.stderr)
    exit(1)

print("Compilation successful.")

# Tracking test results
total_tests = 0
passed_tests = 0

# Step 2: Run tests
for test_folder in os.listdir(TESTS_DIR):
    test_path = os.path.join(TESTS_DIR, test_folder)
    
    if not os.path.isdir(test_path):
        continue  # Skip if not a directory
    
    total_tests += 1  # Increment total test count

    test_in = os.path.join(test_path, "test.in")
    test_out = os.path.join(test_path, "test.out")
    my_test_out = os.path.join(test_path, "my_test.out")
    test_frisc = os.path.join(test_path, "a.frisc")

    print(f"\nRunning test: {test_folder}")

    # Step 2.1: Run generator.exe with test.in to produce a.frisc
    with open(test_in, "r") as infile:
        gen_result = subprocess.run([GENERATOR_EXE], stdin=infile, capture_output=True, text=True)

    if gen_result.returncode != 0:
        print(f"Error running generator.exe for {test_folder}:")
        print(gen_result.stderr)
        continue
    
    # Step 2.2: Copy the generated a.frisc to the test folder
    shutil.copy(FRISC_FILE, test_frisc)

    # Step 2.3: Run "node main.js a.frisc" and capture output
    node_result = subprocess.run(["node", "main.js", FRISC_FILE], capture_output=True, text=True)

    # Combine stdout and stderr
    full_output = node_result.stdout.strip() + "\n" + node_result.stderr.strip()

    # Improved regex to match the single line and extract R6 value
    match = re.search(r"Status of CPU R6:\s*(-?\d+)$", full_output, re.MULTILINE)

    if match:
        r6_value = match.group(1)
    else:
        print(f"❌ Could not extract R6 value for {test_folder}!")
      #   print("---- Full Output ----")
      #   print(full_output)  # Debugging output
      #   print("---------------------")
        continue

    # Write extracted R6 value to my_test.out
    with open(my_test_out, "w") as outfile:
        outfile.write(r6_value + "\n")

    # Step 2.4: Compare my_test.out with test.out
    with open(test_out, "r") as expected_file:
        expected_value = expected_file.read().strip()

    if r6_value == expected_value:
        print_colored(f"✅ Test {test_folder} passed!", "green")
        passed_tests += 1  # Increment passed test count
    else:
        print_colored(f"❌ Test {test_folder} failed! Expected {expected_value}, but got {r6_value}", "red")

# Final Summary
print("\n===================================")
if passed_tests == total_tests:
    print_colored(f"🎉 ALL {passed_tests}/{total_tests} TESTS PASSED! 🎉", "bright_green", bold=True)
else:
    print_colored(f"⚠️ {passed_tests}/{total_tests} tests passed. ⚠️", "yellow", bold=True)
print("===================================")