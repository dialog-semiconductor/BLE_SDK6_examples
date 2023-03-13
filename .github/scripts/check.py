"""This script checks the output of the build process."""
import argparse
import json
import os
import pathlib

from common import bashexec, getProjectsFile, getTargetsFile, printReport


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
    examplesdir = pathlib.Path(__file__).parents[2].resolve()
    startdir = pathlib.Path(os.getcwd())
    artifactsdir = startdir.joinpath(args.artifacts_dir)

    return projects, targets, examplesdir, startdir, artifactsdir


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
                        t.passed.append(p)
                    else:
                        p.addBuildStatus("CMake", t, False, binPath)
                        t.failed.append(p)


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
    projects, targets, examplesdir, startdir, artifactsdir = setVars()
    checkProjects()
    printReport(targets)
    writeOutput()

    os.chdir(startdir)
