"""This file contains the common functions and classes used by the build process."""
import json
import os
import pathlib
import subprocess


def bashexec(command):
    """Run a bash command."""
    if type(command) is str:
        command = command.split()
    else:
        command = list(command)
    process = subprocess.Popen(command, stdout=subprocess.PIPE)
    return process.communicate()[0], process.returncode


class Target:
    """Build targets are devices for which the build is intended."""

    def __init__(self, name, acronym):
        """Initialize the target using only the name and acronym."""
        self.name = name
        self.acronym = acronym
        self.metadata = []
        self.passed = []
        self.failed = []


def getTargetsFromFile(file):
    """Get a list with target devices."""
    targetlist = []
    f = open(file)
    targetsData = json.load(f)
    for tD in targetsData:
        targetlist.append(Target(tD["name"], tD["acronym"]))
    return targetlist


class Project:
    """Projects are all of the individual cmake project projects in this repository."""

    def __init__(self, path, verbose=False):
        """Initialize the Project using the uvprojx path."""
        self.path = pathlib.Path(path)
        self.basedir = self.path.parents[1]
        self.title = self.path.parents[1].name
        self.group = self.path.parents[2].name
        self.uvprojxFile = self.path
        self.uvisionLogFile = self.title + "_log.txt"
        self.cmakelistsFile = "none"
        for f in pathlib.Path(self.basedir).rglob("*.txt"):
            self.cmakelistsFile = f.relative_to(self.basedir)
            break
        self.builddir = self.basedir.joinpath("build")
        self.buildsPassed = []
        if verbose:
            print("initialized ", end="")
            print(self)

    def __repr__(self):
        """Representation of the class when printing."""
        return (
            "Project:\n\tbasedir: "
            + str(self.basedir)
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
            + "\n\tbuilddir: "
            + str(self.builddir)
            + "\n\tbuildsPassed: "
            + str(self.buildsPassed)
        )

    def toDict(self):
        """Get the project as a dictionary."""
        return {"title": self.title, "group": self.group}


def findProjectFiles(directory, verbose=False):
    """Get a list with uvprojx files in a directory."""
    filelist = []
    for dirpath, _, filenames in os.walk(directory):
        for f in filenames:
            if str(f).endswith(".uvprojx"):
                fullpath = os.path.abspath(os.path.join(dirpath, f))
                if verbose:
                    print("found project file: " + fullpath)
                filelist.append(Project(fullpath, verbose))
    return filelist
