"""This script deploys the previously built artifacts and the metadata to the AWS server."""
import argparse
import json
import os
import pathlib
import shutil

from common import getProjectsFile, getTargetsFile


def parseArgs():
    """Get the arguments passed to script."""
    parser = argparse.ArgumentParser(
        prog="CMakeDeploy",
        description="deploys artifacts made by build and check script.",
    )
    parser.add_argument(
        "-t",
        "--targets",
        default=".github/config/targets.json",
        help="The targets definition file. default='.github/config/targets.json'",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="verbose logging",
    )
    parser.add_argument(
        "-f",
        "--datafile",
        default="projectData.json",
        help="The projects definition file. default='artifacts/projectData.json'",
    )
    parser.add_argument(
        "-a",
        "--artifacts-dir",
        default="artifacts",
        help="The projects definition file. default='artifacts'",
    )
    args = parser.parse_args()

    return args


def setVars():
    """Set the variables used in script."""
    projects = getProjectsFile(args.datafile)
    targets = getTargetsFile(args.targets)
    examplesdir = projects[0].absPath.parents[1].resolve()
    startdir = pathlib.Path(os.getcwd())
    artifactsdir = startdir.joinpath(args.artifacts_dir)

    return projects, targets, examplesdir, startdir, artifactsdir


def sortProjectData():
    """Sort all project data into the correct target folder with the proper format."""
    if os.path.exists(artifactsdir):
        if args.verbose:
            print("removing " + str(artifactsdir) + " directory")
        shutil.rmtree(artifactsdir)

    artifactsdir.mkdir()

    for t in targets:
        targetdir = artifactsdir.joinpath(t.name)
        projdatajson = targetdir.joinpath("projectData.json")
        targetdir.mkdir()
        for p in projects:
            for b in p.buildStatus:
                if (b["target"]["name"] == t.name) and (b["passed"] is True):
                    t.metadata.append(p.toDictAws())
        with open(projdatajson, "w") as output:
            output.write(json.dumps({"examples": t.metadata}, indent=3))


def copyFiles():
    """Copy the files to artifacts folder."""
    for t in targets:
        for m in t.metadata:
            p = next((i for i in projects if str(i.title) == m["title"]), None)
            binpath = p.absPath.joinpath(p.builddir).joinpath(
                p.title.name + "_" + t.acronym + ".bin"
            )
            artifactpath = artifactsdir.joinpath(t.name).joinpath(p.path)
            artifactpath.mkdir(parents=True)
            print(artifactpath)
            shutil.copy(binpath, artifactpath)


if __name__ == "__main__":
    args = parseArgs()
    projects, targets, examplesdir, startdir, artifactsdir = setVars()

    sortProjectData()
    copyFiles()

    # # copy binaries
    # for t in targets:
    #     os.chdir(workdir)
    #     projdatafile = artifactsdir + "/" + t.name + "/projectData.json"
    #     f = open(projdatafile)
    #     projectsData = json.load(f)

    #     os.chdir(projdir)
    #     for p in projectsData["examples"]:
    #         binpath = (
    #             projdir
    #             + p["path"][1:]
    #             + "/build/"
    #             + p["title"]
    #             + "_"
    #             + t.acronym
    #             + ".bin"
    #         )
    #         shutil.copy(binpath, artifactsdir + "/" + t.name + "/")
    #         p["readmePath"] = p["path"][2:] + "/Readme.md"
    #         p["binPath"] = (
    #             p["path"][2:] + "/build/" + p["title"] + "_" + t.acronym + ".bin"
    #         )

    #     projectsData = json.dumps(projectsData, indent=2)

    #     with open(projdatafile, "w") as outfile:
    #         outfile.write(projectsData)

    # sys.exit(returncode)

    # upload to AWS
