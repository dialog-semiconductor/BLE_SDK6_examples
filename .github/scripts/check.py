"""This script checks the output of the build process."""
import json
import os
import shutil
import sys

from common import Project, Target, bashexec

if __name__ == "__main__":
    # set variables
    targets = []
    workdir = os.getcwd()
    projdir = workdir + "/projects"
    artifactsdir = workdir + "/artifacts"
    targetsfile = bashexec("find . -name targets.json")[0].decode("utf-8").rstrip()
    buildlistfile = bashexec("find . -name build-list.txt")[0].decode("utf-8").rstrip()

    # cd into workdir
    os.chdir(workdir)

    # read intended targets
    f = open(targetsfile)
    targetsData = json.load(f)
    for tD in targetsData:
        targets.append(Target(tD["name"], tD["acronym"]))

    # read buildlist
    with open(buildlistfile) as f:
        exlist = f.read().splitlines()

    # print debug info
    print("=== env info ===")
    print("pwd:")
    print(bashexec("pwd")[0].decode("utf-8").rstrip())
    print("targetsfile: " + targetsfile)
    print("workdir:" + workdir)
    print("buildlistfile:" + buildlistfile)
    print("ls -la:")
    print(bashexec("ls -la")[0].decode("utf-8").rstrip())
    print("exlist:")
    print(*exlist, sep="\n")
    print("targetsData:")
    print(*targetsData, sep="\n")

    # scan build outputs for passed builds
    for d in exlist:
        exfolder = bashexec("dirname " + d)[0].decode("utf-8").rstrip()
        print("exfolder: " + exfolder)
        exname = bashexec("basename " + exfolder)[0].decode("utf-8").rstrip()
        print("exname: " + exname)
        for t in targets:
            if (
                bashexec(
                    [
                        "grep",
                        "-q",
                        "set(BUILD_FOR_" + t.acronym + " TRUE)",
                        projdir + exfolder[1:] + "/CMakeLists.txt",
                    ]
                )[1]
                == 0
            ):
                if (
                    bashexec(
                        "test -f "
                        + projdir
                        + exfolder[1:]
                        + "/build/"
                        + exname
                        + "_"
                        + t.acronym
                        + ".elf"
                    )[1]
                    == 0
                ):
                    t.passed.append(Project(exfolder))
                else:
                    t.failed.append(Project(exfolder))

    # print output
    for t in targets:
        print("\npassed " + t.name + ":")
        for p in t.passed:
            print(p.path)
        print("\nfailed " + t.name + ":")
        for f in t.failed:
            print(f.path)
        print("\n---------------")
        print("| PASSED: " + str(len(t.passed)) + " ")
        print("| FAILED: " + str(len(t.failed)) + " ")
        print("---------------")

    # produce meta data (projectData.json)
    if os.path.exists(artifactsdir):
        shutil.rmtree(artifactsdir)
    os.mkdir(artifactsdir, mode=0o777)
    for t in targets:
        for p in t.passed:
            t.metadata.append(p.toDict())
        os.mkdir(artifactsdir + "/" + t.name)
        with open(artifactsdir + "/" + t.name + "/projectData.json", "w") as output:
            output.write(json.dumps({"examples": t.metadata}, indent=3))

    sys.exit(0)
