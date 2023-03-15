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
from common import bcolors, bashexec


def getBuildSystem(buildsystem, examplesdir=False, gccPath=False, sdkDir=False, verbose=False):
    if buildsystem is "CMake":
        return CMake(examplesdir, gccPath, sdkDir, verbose=verbose)
    if buildsystem is "Keil":
        return Keil(verbose=verbose)

class CMake:
    """Build system for CMake."""
    def __init__(self, examplesdir, gccPath, sdkDir, verbose=False):
        self.name = "CMake"
        self.verbose = verbose
        if not any([examplesdir, gccPath, sdkDir]):
            raise Exception("To create a CMake build system, provide the following parameters: examplesdir, gccPath, sdkDir")
        self.examplesdir = str(examplesdir)
        self.gccPath = str(gccPath)
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
                "-DDEVICE_NAME=" + project.title,
                "-DCMAKE_BUILD_TYPE=DEBUG",
                "-DCMAKE_TOOLCHAIN_FILE="
                + self.examplesdir
                + "/build_utils/gcc/arm-none-eabi.cmake",
                "-DGCC_TOOLCHAIN_PATH=" + self.gccPath,
                "-DDIALOG_SDK_PATH=" + self.sdkDir,
                "-DDIALOG_EXAMPLE_PATH=" + self.examplesdir,
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
                    project.addBuildStatus("CMake", target, True, binPath)
                else:
                    project.addBuildStatus("CMake", target, False, binPath)
        return 0 

class Keil:
    """Build system for Keil."""
    def __init__(self):
        self.name = "Keil"

    def build(self, project ):
        """Build a project."""
        print(bcolors.OKBLUE + "building " + project.title + "..." + bcolors.ENDC)
        returncode = subprocess.call(["C:/Keil_v5/UV4/UV4.exe", "-b", project.path, "-z", "-o", project.logfile])
        # Keil returns 0 if build is ok, 1 if there are warnings, and 2-20 if there are errors
        colors = [bcolors.OKGREEN, bcolors.WARNING] + [bcolors.FAIL] * 18
        with open(project.basedir + project.logfile, "r") as f:
            print(colors[returncode] + f.read() + bcolors.ENDC)
        return returncode

    def check(self, project, target):
        """Check a build."""
        with open(project.logfile) as log, open(project.uvprojxFile) as proj:
            if ("<TargetName>" + target.name + "</TargetName>") in proj.read():
                if (target.acronym + passmarker) in log.read():
                    target.passed.append(p)
                else:
                    target.failed.append(p)
        return 0 