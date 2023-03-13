"""This file contains the common functions and classes used by the build process."""
import json
import os
import pathlib
import subprocess


def bashexec(command, prnt=False):
    """Run a bash command."""
    if type(command) is str:
        command = command.split()
    else:
        command = list(command)
    if prnt:
        process = subprocess.Popen(command)
    else:
        process = subprocess.Popen(command, stdout=subprocess.PIPE)
    return process.communicate()[0], process.returncode


def findFirstOfGlob(path, pathGlob):
    """Get first found instance of a glob within a folder."""
    for f in pathlib.Path(path.parents[1]).rglob(pathGlob):
        return f
    return ""


class Target:
    """Build targets are devices for which the build is intended."""

    def __init__(self, name, acronym):
        """Initialize the target using only the name and acronym."""
        self.name = name
        self.acronym = acronym
        self.metadata = []
        self.passed = []
        self.failed = []

    def to_json(self):
        """Get target as json."""
        return {"name": self.name, "acronym": self.acronym}


def getTargetsFile(file):
    """Get a list with target devices."""
    targetlist = []
    f = open(file)
    targetsData = json.load(f)
    for tD in targetsData:
        targetlist.append(Target(tD["name"], tD["acronym"]))
    return targetlist


def printReport(targets):
    """Print a report of the build."""
    for t in targets:
        print("\npassed " + t.name + ":")
        for p in t.passed:
            print(p.title)
        print("\nfailed " + t.name + ":")
        for f in t.failed:
            print(f.title)
        print("\n---------------")
        print("| PASSED: " + str(len(t.passed)) + " ")
        print("| FAILED: " + str(len(t.failed)) + " ")
        print("---------------")


class Project:
    """Projects are all of the individual cmake project projects in this repository."""

    def __init__(self, source, exdir=os.getcwd(), verbose=False):
        """Initialize the Project using either a uvprojx path or a dict."""
        # multiple constructors not officially supported by python, this is a workaround
        if type(source) == str:
            self._initFromPath(source, exdir=exdir, verbose=verbose)
        elif type(source) == dict:
            self._initFromDict(source, verbose=verbose)
        else:
            raise Exception("source type not supported")

    def _initFromPath(self, path, exdir, verbose=False):
        """Initialize the Project using the uvprojx path."""
        inPath = pathlib.Path(path)
        self.absPath = inPath.parents[1]
        self.path = str(self.absPath).replace(str(exdir), "")[1:]
        self.title = inPath.parents[1].name
        self.group = str(inPath.relative_to(exdir)).split("/")[0]
        self.uvprojxFile = inPath
        self.uvisionLogFile = self.title + "_log.txt"
        self.cmakelistsFile = findFirstOfGlob(inPath, "CMakeLists.txt")
        self.readmePath = findFirstOfGlob(inPath, "Readme.md")
        self.builddir = (
            inPath.parents[1].joinpath("build").relative_to(inPath.parents[1])
        )
        self.buildStatus = []
        if verbose:
            print("initialized ", end="")
            print(self)

    def _initFromDict(self, dict, verbose=False):
        """Initialize the Project from a dict."""
        self.absPath = pathlib.Path(dict["absPath"])
        self.path = pathlib.Path(dict["path"])
        self.title = pathlib.Path(dict["title"])
        self.group = pathlib.Path(dict["group"])
        self.uvprojxFile = pathlib.Path(dict["uvprojxFile"])
        self.uvisionLogFile = dict["uvisionLogFile"]
        if dict["cmakelistsFile"] == "":
            self.cmakelistsFile = ""
        else:
            self.cmakelistsFile = pathlib.Path(dict["cmakelistsFile"])
        self.readmePath = pathlib.Path(dict["readmePath"])
        self.builddir = pathlib.Path(dict["builddir"])
        self.buildStatus = dict["buildStatus"]

    def addBuildStatus(self, buildsystem, target, passed):
        """Add a build to the project item."""
        if not self.buildStatus:
            self.buildStatus = []
        for i in range(len(self.buildStatus)):
            if (self.buildStatus[i]["buildsystem"] == buildsystem) and (
                self.buildStatus[i]["target"] == target.name
            ):
                del self.buildStatus[i]
                break

        self.buildStatus.append(
            {"buildsystem": buildsystem, "target": target.to_json(), "passed": passed}
        )

    def toDictComplete(self):
        """Get the project as a dictionary."""
        return {
            "absPath": str(self.absPath),
            "path": str(self.path),
            "title": str(self.title),
            "group": str(self.group),
            "uvprojxFile": str(self.uvprojxFile),
            "uvisionLogFile": str(self.uvisionLogFile),
            "cmakelistsFile": str(self.cmakelistsFile),
            "readmePath": str(self.readmePath),
            "builddir": str(self.builddir),
            "buildStatus": self.buildStatus,
        }

    def toDictAws(self, rootdir, fortarget):
        """Get the project as a dictionary in standard Renesas AWS artifact format."""
        return {
            "path": str(self.path),
            "group": str(self.group),
            "title": str(self.title),
            "readmePath": str(self.readmePath).replace(str(rootdir), "")[1:],
            "binPath": "placeholder",
        }

    def __repr__(self):
        """Representation of the class when printing."""
        return (
            "Project:\n\tabsPath: "
            + str(self.absPath)
            + "\n\tpath: "
            + str(self.path)
            + "\n\ttitle: "
            + str(self.title)
            + "\n\tgroup: "
            + str(self.group)
            + "\n\tuvprojxFile: "
            + str(self.uvprojxFile)
            + "\n\tuvisionLogFile: "
            + str(self.uvisionLogFile)
            + "\n\tcmakelistsFile: "
            + str(self.cmakelistsFile)
            + "\n\treadmePath: "
            + str(self.readmePath)
            + "\n\tbuilddir: "
            + str(self.builddir)
            + "\n\tbuildStatus: "
            + str(self.buildStatus)
        )


def getProjectsFile(file, verbose=False):
    """Get a list with projects from a json file."""
    file = open(file)
    projectsData = json.load(file)
    projectsList = []

    for p in projectsData:
        projectsList.append(Project(p, verbose=verbose))

    return projectsList


def findProjectFiles(directory, exdir, verbose=False):
    """Get a list with uvprojx files in a directory."""
    filelist = []
    for dirpath, _, filenames in os.walk(directory):
        for f in filenames:
            if str(f).endswith(".uvprojx"):
                fullpath = os.path.abspath(os.path.join(dirpath, f))
                if verbose:
                    print("found project file: " + fullpath)
                filelist.append(Project(fullpath, exdir, verbose=verbose))
    return filelist


class bcolors:
    """Colors for printing in terminal."""

    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"
