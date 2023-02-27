import os
import subprocess

class Target:
    def __init__(self, name):
        self.name = name
        self.passed = []
        self.failed = []

targets=[Target("531"), Target("585"), Target("586")]

def bash(command):
    if type(command) is str:
        command = command.split()
    else:
        command = list(command)
    process = subprocess.Popen(command, stdout=subprocess.PIPE)
    return process.communicate()[0], process.returncode

if __name__ == "__main__":
    workdir = os.getenv('GITHUB_WORKSPACE/projects',".")

    with open(workdir+"/.github/config/build-list.txt") as f:
        exlist = f.read().splitlines()

    for d in exlist:
        exfolder = bash('dirname '+d)[0].decode("utf-8").rstrip()
        exname = bash('basename '+exfolder)[0].decode("utf-8").rstrip()

        for t in targets:
            if (bash(["grep","-q","set(BUILD_FOR_"+t.name+" TRUE)",exfolder+"/CMakeLists.txt"])[1] == 0):
                if(bash("test -f "+exfolder+"/build/"+exname+"_"+t.name+".elf")[1] == 0):
                    t.passed.append(exfolder)
                else:
                    t.failed.append(exfolder)

    for t in targets:
        print("\npassed "+t.name+":")
        for p in t.passed:
            print(p)
        print("\nfailed "+t.name+":")
        for p in t.failed:
            print(p)
        print("\n---------------")
        print("| PASSED: "+str(len(t.passed))+" ")
        print("| FAILED: "+str(len(t.failed))+" ")
        print("---------------")