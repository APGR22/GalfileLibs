import subprocess
import platform

SYSTEM = platform.system()

compiler = "g++"
files = (
    "test/main.cpp",
)
output = "Test/test"
libs = (
    "-lssl",
    "-lcrypto",
)
options = (
    "-O3",
    # "-g",
    "-std=c++20",
)
includes = (
    "-I.",
)

if SYSTEM == "Windows":
    output += ".exe"
elif SYSTEM == "Linux":
    output += ".elf"

# os.chdir("..")
subprocess.call([
    compiler,
    *includes,
    "-o",
    output,
    *files,
    *libs,
    *options,
])