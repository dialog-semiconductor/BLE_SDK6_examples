"""This script deploys the previously built artifacts and the metadata to the AWS server."""
import argparse
import json
import os
import pathlib
import shutil

from common import getProjectsFromFile, getTargetsFromFile


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
    projects = getProjectsFromFile(args.datafile)
    targets = getTargetsFromFile(args.targets)
    examplesdir = projects[0].basedir.parents[1].resolve()
    startdir = pathlib.Path(os.getcwd())
    artifactsdir = startdir.joinpath(args.artifacts_dir)

    return projects, targets, examplesdir, startdir, artifactsdir


# def cleanProjectData():
#     for t in targets:
#         projdatafile = str(artifactsdir.joinpath(t.name).joinpath("projectData.json"))
#         with open(projdatafile) as f:
#             projectsData = json.load(f)


#         for p in projectsData['examples']:
#             p.pop('basedir',None)
#             p.pop('uvprojxFile',None)
#             p.pop('uvisionLogFile',None)
#             p.pop('buildStatus',None)
#             p.pop('cmakelistsFile',None)
#             p.pop('builddir',None)

#         projectsData = json.dumps(projectsData, indent=2)

#         with open(projdatafile, "w") as f:
#             f.write(projectsData)


def sortProjectData():
    """Sort all project data into the correct folder with the proper format."""
    if os.path.exists(artifactsdir):
        if args.verbose:
            print("removing " + str(artifactsdir) + " directory")
        shutil.rmtree(artifactsdir)

    pathlib.Path(artifactsdir).mkdir()
    for t in targets:
        for p in t.passed:
            t.metadata.append(p.toDictAws())
        os.mkdir(artifactsdir.joinpath(t.name))
        with open(artifactsdir.joinpath(t.name + "/projectData.json"), "w") as output:
            output.write(json.dumps({"examples": t.metadata}, indent=3))


# def copyFiles():
#     for t in targets:
#         for p in projects:
#             binpath = p.basedir.joinpath(p.builddir).joinpath(p.title.name+"_"+t.name+".bin")
#             shutil.copy(binpath, artifactsdir + "/" + t.name + "/")


if __name__ == "__main__":
    args = parseArgs()
    projects, targets, examplesdir, startdir, artifactsdir = setVars()

    sortProjectData()

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
