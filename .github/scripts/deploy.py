"""This script deploys the previously built artifacts and the metadata to the AWS server."""
import json
import os

from common import Target, bashexec

if __name__ == "__main__":
    # set variables
    targets = []
    returncode = 0
    workdir = os.getenv("GITHUB_WORKSPACE", os.getcwd())
    if workdir != os.getcwd():
        workdir += "/projects"
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

    # copy binaries
    for t in targets:
        f = open(artifactsdir + "/" + t.name + "/projectData.json")
        projectsData = json.load(f)

        for p in projectsData["examples"]:
            binpath = p["path"] + "/build/" + p["title"] + "_" + t.acronym + ".bin"
            returncode |= bashexec(["cp", binpath, artifactsdir + "/" + t.name + "/"])[
                1
            ]
            print(binpath)
            # readmepath = bashexec(["find"],p['path'],)

    # upload to AWS
