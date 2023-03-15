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
"""This script checks the output of the build process."""
import argparse
import json
import os
import pathlib

from common import bashexec, getTargetsFile
from project import ProjectList
from buildSystems import getBuildSystem



def parseArgs():
    """Get the arguments passed to script."""
    parser = argparse.ArgumentParser(
        prog="CMakeCheck",
        description="checks build output in repository produced by build script.",
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
        "-s",
        "--sdkdir",
        default=os.path.dirname(
            os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
        ),
        help="the location of the SDK that is to be used for the build. default = ./build.py/../../../..",
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
    gccPath = bashexec("which arm-none-eabi-gcc")[0].decode("utf-8").rstrip()
    projects = ProjectList(jsonFile=args.datafile,verbose=args.verbose)
    targets = getTargetsFile(args.targets)
    examplesdir = pathlib.Path(__file__).parents[2].resolve()
    startdir = pathlib.Path(os.getcwd())
    artifactsdir = startdir.joinpath(args.artifacts_dir)
    buildSystem = getBuildSystem(args.build_system, examplesdir, gccPath, args.sdkdir, args.verbose)

    return projects, targets, examplesdir, startdir, artifactsdir, buildSystem


def checkProjects():
    """Check all project directories if the builds that are configured have passed."""
    for p in projects:
        os.chdir(p.absPath)
        for t in targets:
            if p.cmakelistsFile:
                binPath = (p.builddir).joinpath(
                    p.title.name + "_" + str(t.acronym) + ".bin"
                )
                if (
                    bashexec(
                        [
                            "grep",
                            "-q",
                            "set(BUILD_FOR_" + t.acronym + " TRUE)",
                            p.cmakelistsFile,
                        ]
                    )[1]
                    == 0
                ):
                    if bashexec("test -f " + str(binPath))[1] == 0:
                        p.addBuildStatus("CMake", t, True, binPath)
                    else:
                        p.addBuildStatus("CMake", t, False, binPath)


def writeOutput():
    """Write the output to the standard projectData.json."""
    if args.verbose:
        print("writing output")

    os.chdir(startdir)
    projdat = []
    for p in projects:
        projdat.append(p.toDictComplete())
    jsonProjdat = json.dumps(projdat, indent=2)

    with open(args.datafile, "w") as outfile:
        outfile.write(jsonProjdat)


if __name__ == "__main__":
    args = parseArgs()
    projects, targets, examplesdir, startdir, artifactsdir, buildSystem = setVars()
    checkProjects()
    #printReport(targets)
    for target in targets:
        projects.printReport(target, buildSystem)
    writeOutput()

    os.chdir(startdir)
