# #####################################################################################
# The MIT License (MIT)
# Copyright (c) 2023 Renesas Electronics Corporation and/or its affiliates
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
# #####################################################################################
"""This script deploys the previously built artifacts and the metadata to the AWS server."""
import argparse
import json
import os
import pathlib
import shutil

from common import getTargetsFile, bashexec
from project import ProjectList


def parseArgs():
    """Get the arguments passed to script."""
    parser = argparse.ArgumentParser(
        prog="Deploy Artifacts",
        description="deploys artifacts made by build script.",
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
        help="The projects definition file. default='projectData.json'",
    )
    parser.add_argument(
        "-a",
        "--artifacts-dir",
        default="artifacts",
        help="The projects definition file. default='artifacts'",
    )
    parser.add_argument(
        "-b",
        "--build-system",
        default = "CMake",
        choices= ["CMake","Keil"],
        help="The build system used for the build. default='CMake'",
    )
    args = parser.parse_args()

    return args


def setVars():
    """Set the variables used in script."""
    projects = ProjectList(jsonFile=args.datafile,verbose=args.verbose)
    targets = getTargetsFile(args.targets)
    examplesdir = pathlib.Path(__file__).parents[2].resolve()
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
            pDict = p.toDictAws(examplesdir, t)
            if pDict:
                t.metadata.append(pDict)
        with open(projdatajson, "w") as output:
            output.write(json.dumps({"examples": t.metadata}, indent=3))


def copyFiles():
    """Copy the files to artifacts folder."""
    for t in targets:
        print("Copying "+t.name)
        for m in t.metadata:
            p = next((i for i in projects if str(i.title) == m["title"]), None)
            if args.verbose:
                print("Copying "+str(p.title))
            binpath = p.absPath.joinpath(p.builddir).joinpath(
                p.title.name + "_" + t.acronym + ".bin"
            )
            artifactpath = artifactsdir.joinpath(t.name).joinpath(p.path)
            artifactpath.mkdir(parents=True)
            shutil.copy(binpath, artifactpath)

def synchFilesAws():
    for target in targets:
        print("deploying "+target.name)
        command = ["aws","s3","sync","--delete",str(artifactsdir.joinpath(target.name)),"s3://lpccs-docs.renesas.com/examples_arfitacts/"+target.name]
        if args.verbose:
            print("Executing "+str(command))
        bashexec(command)


if __name__ == "__main__":
    args = parseArgs()
    projects, targets, examplesdir, startdir, artifactsdir = setVars()

    sortProjectData()
    copyFiles()
    synchFilesAws()

    os.chdir(startdir)
