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
"""Builds projects in repository using Cmake."""
import argparse
import json
import os
import pathlib
import signal
import sys

from buildSystems import getBuildSystem
from common import bcolors, getTargetsFile
from project import ProjectList


def parseArgs():
    """Get the arguments passed to script."""
    parser = argparse.ArgumentParser(
        prog="cMakeBuild",
        description="Builds projects in repository using Cmake.",
    )
    parser.add_argument(
        "-t",
        "--targets",
        default=".github/config/targets.json",
        help="The targets definition file. default='.github/config/targets.json'",
    )
    parser.add_argument(
        "-p",
        "--projdir",
        default=".",
        help="The directory to search project files. default='.'",
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
        "-x",
        "--exclude",
        default="",
        help="A list of directories to exclude from the build. Can be a file in any format, a string, or a list",
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
        default="CMake/gcc10",
        choices=["CMake/gcc10", "Keil/armcomp6"],
        help="The build system used for the build. default='CMake'",
    )
    parser.add_argument(
        "--append",
        action="store_true",
        help="Append to an existing datafile (given with option --datafile)",
    )
    args = parser.parse_args()

    if args.exclude:
        args.exclude = open(args.exclude, "r", errors="ignore").read()

    return args


def setVars():
    """Set the variables used in script."""
    examplesdir = pathlib.Path(__file__).parents[2].resolve()
    targets = getTargetsFile(args.targets)
    startdir = pathlib.Path(os.getcwd())
    datafile = startdir.joinpath(args.datafile)
    buildSystem = getBuildSystem(
        args.build_system, examplesdir, args.sdkdir, args.verbose
    )
    if args.append:
        projectFiles = ProjectList(jsonFile=args.datafile, verbose=args.verbose)
    else:
        projectFiles = ProjectList(
            projectDirectory=args.projdir,
            examplesDirectory=examplesdir,
            verbose=args.verbose,
        )

    for p in projectFiles:
        if (str(p.path) + "/") in args.exclude:
            if args.verbose:
                print(
                    bcolors.WARNING
                    + "Excluding project from build (specified with '-x' option): "
                    + str(p)
                    + bcolors.ENDC
                )
            p.excludeBuilds.append(args.build_system)

    return projectFiles, targets, examplesdir, startdir, datafile, buildSystem


def checkProjects():
    """Use the build system to check the build result."""
    checkResult = 0 

    for project in projectFiles:
        for target in targets:
            if buildSystem.check(project, target) == 1: # 1 indicates a failed build
                checkResult = 1

    return checkResult


def buildProjects():
    """Build the projects with the configured build system."""
    buildResult = 0

    for project in projectFiles:
        project.applyPatchToSdk(args.sdkdir)
        if not buildSystem.build(project):
            buildResult = 1
        project.revertPatchToSdk(args.sdkdir)

    return buildResult


def writeOutput():
    """Write the output to the standard projectData.json."""
    if args.verbose:
        print("writing output")

    os.chdir(startdir)
    projdat = []
    for p in projectFiles:
        projdat.append(p.toDictComplete(examplesdir))
    jsonProjdat = json.dumps(projdat, indent=2)

    with open(datafile, "w") as outfile:
        outfile.write(jsonProjdat)


def handleAbort(signum, frame):
    """Handle if the script gets aborted."""
    print(bcolors.FAIL + "ABORTING BUILDS..." + bcolors.ENDC)
    writeOutput()
    sys.exit(1)


if __name__ == "__main__":
    args = parseArgs()
    projectFiles, targets, examplesdir, startdir, datafile, buildSystem = setVars()
    signal.signal(
        signal.SIGINT, handleAbort
    )  # still write output if script aborted during build
    buildProjects()
    checkResult = checkProjects()
    for target in targets:
        projectFiles.printReport(target, buildSystem)
    writeOutput()

    os.chdir(startdir)
    sys.exit(checkResult)
