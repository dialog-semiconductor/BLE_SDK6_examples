"""This file contains the common functions and classes used by the build process."""
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


class Project:
    """Projects are all of the individual cmake project projects in this repository."""

    def __init__(self, path):
        """Initialize the Project using the path."""
        self.path = path
        self.group = path.split("/")[1]
        self.title = path.split("/")[len(path.split("/")) - 1]

    def toDict(self):
        """Get the project as a dictionary."""
        return {"path": self.path, "group": self.group, "title": self.title}
