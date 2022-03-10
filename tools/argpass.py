#!/usr/bin/python3
#@bin ARG PASS
#@desc Pass version number to OS

import datetime

# Read Version Number
version = int(open("../src/Kernel/BUILD.h").read().split("\n")[0].split(" ")[2]) + 1

# Write Version Number
open("../src/Kernel/BUILD.h", "w").write("#define BUILD " + str(version) + "\n" + "#define BUILD_DATE \"" + str(datetime.datetime.now()) + "\"" + "\n")