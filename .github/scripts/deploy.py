"""This script deploys the previously built artifacts and the metadata to the AWS server."""
import json
import os
import shutil
import sys

from common import Target, bashexec

if __name__ == "__main__":
    # set variables
    targets = []
    returncode = 0
    workdir = os.getcwd()
    projdir = workdir + "/projects"
    artifactsdir = workdir + "/artifacts"
    targetsfile = bashexec("find . -name targets.json")[0].decode("utf-8").rstrip()
    buildlistfile = bashexec("find . -name build-list.txt")[0].decode("utf-8").rstrip()

    # read intended targets
    f = open(targetsfile)
    targetsData = json.load(f)
    for tD in targetsData:
        targets.append(Target(tD["name"], tD["acronym"]))

    # copy binaries
    for t in targets:
        os.chdir(workdir)
        projdatafile = artifactsdir + "/" + t.name + "/projectData.json"
        f = open(projdatafile)
        projectsData = json.load(f)

        os.chdir(projdir)
        for p in projectsData["examples"]:
            binpath = (
                projdir
                + p["path"][1:]
                + "/build/"
                + p["title"]
                + "_"
                + t.acronym
                + ".bin"
            )
            shutil.copy(binpath, artifactsdir + "/" + t.name + "/")
            p["readmePath"] = p["path"][2:] + "/Readme.md"
            p["binPath"] = (
                p["path"][2:] + "/build/" + p["title"] + "_" + t.acronym + ".bin"
            )

        projectsData = json.dumps(projectsData, indent=2)

        with open(projdatafile, "w") as outfile:
            outfile.write(projectsData)

    sys.exit(returncode)

    # upload to AWS
