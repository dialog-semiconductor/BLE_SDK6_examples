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
"""This file contains a class to handle different build systems."""
import subprocess
import os
import shutil
import sys
import pathlib

from common import bcolors, bashexec
sys.path.append(str(pathlib.Path(os.path.abspath(__file__)).parents[2]))

def getBuildSystem(buildsystem, examplesdir=False, sdkDir=False, verbose=False):
    if (buildsystem == "CMake") and all([examplesdir, sdkDir]):
        return CMake(examplesdir, sdkDir, verbose)
    elif (buildsystem == "Keil") and sdkDir:
        return Keil(sdkDir, verbose)
    else:
        raise Exception("To create a CMake build system, provide the following parameters: examplesdir, gccPath, sdkDir or To create a Keil build system provide sdkDir")


class CMake:
    """Build system for CMake."""
    def __init__(self, examplesdir, sdkDir, verbose=False):
        self.name = "CMake"
        self.verbose = verbose
        if not any([examplesdir, sdkDir]):
            raise Exception("To create a CMake build system, provide the following parameters: examplesdir, gccPath, sdkDir")
        self.examplesdir = str(examplesdir)
        self.gccPath = bashexec("which arm-none-eabi-gcc")[0].decode("utf-8").rstrip()
        self.sdkDir = str(sdkDir)

    def build(self, project):
        """Build a project."""
        startdir = os.getcwd()
        if not project.cmakelistsFile:
            print(bcolors.WARNING + "Skipping build due to missing CMakeLists.txt (skipping can also be caused by soon to be deprecated --exclude option): " + str(project) + bcolors.ENDC)
            return 1
        print(bcolors.HEADER + "Building: " + str(project) + bcolors.ENDC)
        os.chdir(project.absPath)
        if os.path.exists(project.builddir):
            shutil.rmtree(project.builddir)
        project.builddir.mkdir()
        bashexec(
            [
                "cmake",
                "-DDEVICE_NAME=" + str(project.title),
                "-DCMAKE_BUILD_TYPE=DEBUG",
                "-DCMAKE_TOOLCHAIN_FILE="
                + str(self.examplesdir)
                + "/build_utils/gcc/arm-none-eabi.cmake",
                "-DGCC_TOOLCHAIN_PATH=" + str(self.gccPath),
                "-DDIALOG_SDK_PATH=" + str(self.sdkDir),
                "-DDIALOG_EXAMPLE_PATH=" + str(self.examplesdir),
                "-S",
                ".",
                "-B",
                str(project.builddir),
            ],
            prnt=self.verbose,
        )
        os.chdir(project.builddir)
        if bashexec("make -j 7", prnt=self.verbose)[1] != 0:
            print(bcolors.FAIL+ str(project)+bcolors.ENDC)
            os.chdir(startdir)
            return 1
        os.chdir(startdir)
        return 0

    def check(self, project, target):
        """Check a build."""
        os.chdir(project.absPath)
        if project.cmakelistsFile:
            binPath = (project.builddir).joinpath(project.title.name + "_" + str(target.acronym) + ".bin")
            if (
                bashexec(
                    [
                        "grep",
                        "-q",
                        "set(BUILD_FOR_" + target.acronym + " TRUE)",
                        project.cmakelistsFile,
                    ]
                )[1]
                == 0
            ):
                if bashexec("test -f " + str(binPath))[1] == 0:
                    project.addBuildStatus(self.name, target, True, binPath)
                else:
                    project.addBuildStatus(self.name, target, False, binPath)
        return 0 

class Keil:
    """Build system for Keil."""
    def __init__(self, sdkDir, verbose=False):
        self.name = "Keil"
        self.sdkDir = sdkDir
        self.verbose = verbose
        self.passmarker = '.axf" - 0 Error(s),'

    def build(self, project ):
        """Build a project."""
        if project.uvprojxFile:
            print(bcolors.OKBLUE + "building: " + str(project) + bcolors.ENDC)
        else:
            print(bcolors.WARNING + "not building " + project.title + "... (excluded with -x option)" + bcolors.ENDC)
            return 0
        os.chdir(project.absPath)
        keilCommand = ["C:/Keil_v5/UV4/UV4.exe", "-b", str(project.uvprojxFile.resolve()), "-z", "-o", project.uvisionLogFile.name]
        if self.verbose:
            print("executing Keil command: "+str(keilCommand))
        returncode = subprocess.call(keilCommand)
        # Keil returns 0 if build is ok, 1 if there are warnings, and 2-20 if there are errors
        colors = [bcolors.OKGREEN, bcolors.WARNING] + [bcolors.FAIL] * 18
        if returncode >= len(colors): # this is to handle undocumented Keil return codes
            returncode = 3 
        with open(project.uvisionLogFile, "r") as f:
            print(colors[returncode] + f.read() + bcolors.ENDC)
        return returncode

    def check(self, project, target):
        """Check a build."""
        if os.path.isfile(project.uvisionLogFile) and os.path.isfile(project.uvprojxFile):
            with open(project.uvisionLogFile) as log, open(project.uvprojxFile) as proj:
                if ("<TargetName>" + target.name + "</TargetName>") in proj.read():
                    binPath = project.uvprojxFile.parent.joinpath("out_"+target.name+"/Objects/"+os.path.splitext(project.title.name)[0] + "_" + str(target.acronym) + ".bin")
                    if ((target.acronym + self.passmarker) in log.read()):# and (os.path.isfile(binPath)):
                        project.addBuildStatus(self.name, target, True, binPath)
                    else:
                        project.addBuildStatus(self.name, target, False, binPath)
        return 0 