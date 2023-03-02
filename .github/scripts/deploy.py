"""This script deploys the previously built artifacts and the metadata to the AWS server."""
import json
import os

from common import Target, bashexec

if __name__ == "__main__":
    # set variables
    targets = []
    returncode = 0
    workdir = os.getenv("GITHUB_WORKSPACE", os.getcwd())
    artifactsdir = workdir + "/artifacts"
    targetsfile = workdir + "/.github/config/targets.json"
    buildlistfile = workdir + "/.github/config/build-list.txt"

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
            binpath = (
                workdir
                + p["path"][1:]
                + "/build/"
                + p["title"]
                + "_"
                + t.acronym
                + ".bin"
            )
            returncode |= bashexec(["cp", binpath, artifactsdir + "/" + t.name + "/"])[
                1
            ]
            print(artifactsdir)
            print(binpath)

    # upload to AWS
