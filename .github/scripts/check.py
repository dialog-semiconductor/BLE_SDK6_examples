import os
import shutil
import subprocess
import json

class Target:
    def __init__(self, name, acronym):
        self.name = name
        self.acronym = acronym
        self.metadata = []
        self.passed = []
        self.failed = []

class Example:
    def __init__(self, path):
        self.path = path
        self.group = path.split("/")[1]
        self.title = path.split("/")[len(path.split("/")) - 1]
    
    def toDict(self):
        return {
            "path" : self.path,
            "group" : self.group,
            "title" : self.title
        }

targets=[Target("DA14531","531"), Target("DA14585","585"), Target("DA14586","586")]

def bash(command):
    if type(command) is str:
        command = command.split()
    else:
        command = list(command)
    process = subprocess.Popen(command, stdout=subprocess.PIPE)
    return process.communicate()[0], process.returncode

if __name__ == "__main__":
    
    # set variables
    workdir = os.getenv('GITHUB_WORKSPACE',".")
    if (workdir != "."):
        workdir += "/projects/"
    artifactsdir = workdir+"/.github/artifacts"

    # read buildlist
    with open(workdir+"/.github/config/build-list.txt") as f:
        exlist = f.read().splitlines()

    # scan build outputs for passed builds
    for d in exlist:
        exfolder = bash('dirname '+d)[0].decode("utf-8").rstrip()
        exname = bash('basename '+exfolder)[0].decode("utf-8").rstrip()
        for t in targets:
            if (bash(["grep","-q","set(BUILD_FOR_"+t.acronym+" TRUE)",exfolder+"/CMakeLists.txt"])[1] == 0):
                if(bash("test -f "+exfolder+"/build/"+exname+"_"+t.acronym+".elf")[1] == 0):
                    t.passed.append(Example(exfolder))
                else:
                    t.failed.append(Example(exfolder))
        
    # print output
    for t in targets:
        print("\npassed "+t.name+":")
        for p in t.passed:
            print(p.path)
        print("\nfailed "+t.name+":")
        for f in t.failed:
            print(f.path)
        print("\n---------------")
        print("| PASSED: "+str(len(t.passed))+" ")
        print("| FAILED: "+str(len(t.failed))+" ")
        print("---------------")

    # produce meta data (projectData.json)
    if (os.path.exists(artifactsdir)):
        shutil.rmtree(artifactsdir)
    os.mkdir(artifactsdir, mode=0o777)
    for t in targets:
        for p in t.passed:
            t.metadata.append(p.toDict())
        os.mkdir(artifactsdir+"/"+t.name)
        with open(artifactsdir+"/"+t.name+"/projectData.json", "w") as output:
            output.write(json.dumps({"examples": t.metadata}, indent=3))