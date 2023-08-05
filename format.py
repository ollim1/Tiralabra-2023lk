import os

file_extensions = (".c", ".h")

for root, dirs, files in os.walk("src"):
    for file in files:
        if file.endswith(file_extensions):
            print("Formatting: src/" + file)
            os.system("clang-format -i --style=\'{BasedOnStyle: llvm, IndentWidth: 4}\' src/" + file)

for root, dirs, files in os.walk("tests"):
    for file in files:
        if file.endswith(file_extensions):
            print("Formatting: tests/" + file)
            os.system("clang-format -i --style=\'{BasedOnStyle: llvm, IndentWidth: 4}\' tests/" + file)
