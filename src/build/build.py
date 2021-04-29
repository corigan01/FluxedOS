import sys
import os
import time
import datetime
import threading

begin_time = datetime.datetime.now()

#print ("      ________                    __   ______          "    )
#print ("     / ____/ /_  ___  _____  ____/ /  / ____/___  ____ "    )
#print ("    / /_  / / / / / |/_/ _ \/ __  /  / /   / __ \\/ __ \\"  )
#print ("   / __/ / / /_/ />  </  __/ /_/ /  / /___/ /_/ / /_/ /"    )
#print ("  /_/   /_/\__,_/_/|_|\___/\__,_/   \____/ .___/ .___/ "    )
#print ("                                        /_/   /_/      "    )
#print ("\e[0;34m-- The easy auto \e[0;31mC++\e[0;34m compiler that runs in your terminal! --")
#print ("            A \e[0;35mMain Menu\e[0;34m aka \e[0;35mcorigan01\e[0;34m project!")

BuildType = []

for Argument in sys.argv:
    # check if the arg is the name of the program
    if Argument != sys.argv[0]:
        BuildType.append(Argument)


path ="../../"
#we shall store all the file names in this list
filelist = []

for root, dirs, files in os.walk(path):
	for file in files:
        #append the file name to the list
		filelist.append(os.path.join(root,file))


Cpp_List = []
C_List = []
H_list = []
S_list = []

O_list = []

for name in filelist:
    if name[-4:] == ".cpp":
        Cpp_List.append(name)

    elif (".h" in name[-4:]):
        H_list.append(name)

    elif (".c" in name[-2:]):
        C_List.append(name)

    elif (".S" in name[-2:]):
        S_list.append(name)


def outputFilename(i, cropsize):
    return i[:-(cropsize + 1)] + "_" + i[-cropsize:]+ ".o"

def BuildASM(i):
    os.system("nasm -f elf " + i + " -o " + outputFilename(i, 1))

def BuildCpp(i):
    os.system("g++ -m32 -elf_i386 -std=c++2a -O -fstrength-reduce -fomit-frame-pointer -O2 -finline-functions -nostdinc -fno-builtin -fdiagnostics-color=always -c " + i)
    
def BuildC(i):
    os.system(" gcc -m32 -elf_i386  -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -fno-builtin -fdiagnostics-color=always -c " + i)

threadList = []

for i in Cpp_List:
    x = threading.Thread(target=BuildCpp, args=(i,))
    threadList.append(x)
    x.start()
for i in C_List:
    x = threading.Thread(target=BuildC, args=(i,))
    threadList.append(x)
    x.start()
for i in S_list:
    x = threading.Thread(target=BuildASM, args=(i,))
    threadList.append(x)
    x.start()

for i in threadList:
    i.join()

filelist = []

for root, dirs, files in os.walk(path):
	for file in files:
		filelist.append(os.path.join(root,file))
    
for name in filelist:
    if ".o" in name[-2:]:
        O_list.append(name)

os.system("mkdir ../../obj")
for i in O_list:
    os.system("mv " + i + " ../../obj")

os.system("g++ -m32 -lstdc++ -nostartfiles -nostdinc -T linker.ld  ../../obj/*.o -o FluxedOS.bin ")


for root, dirs, files in os.walk(path):
	for file in files:
            if (".o" in os.path.join(root,file)[-2:]):
                os.system("rm " + os.path.join(root,file))
        
    

print(str(round((datetime.datetime.now() - begin_time).total_seconds() * 1000, 4)) + "ms")