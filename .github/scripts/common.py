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
