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
import os
import pathlib
import shutil
import subprocess
import sys

from common import bashexec, bcolors

sys.path.append(str(pathlib.Path(os.path.abspath(__file__)).parents[2]))


def getBuildSystem(buildsystem, examplesdir=False, sdkDir=False, verbose=False):
    """Get a build system based on the type specified by buildSystem."""
    if (buildsystem == "CMake/gcc10") and all([examplesdir, sdkDir]):
        return CMake(examplesdir, sdkDir, verbose)
    elif (buildsystem == "Keil/armcomp6") and sdkDir:
        return Keil(sdkDir, verbose)
    elif (buildsystem == "clang") and all([examplesdir, sdkDir]):
        return Clang(examplesdir, sdkDir, verbose)
    else:
        raise Exception(
            "To create a CMake build system, provide the following parameters: examplesdir, sdkDir or To create a Keil build system provide sdkDir"
        )


class Clang:
    """Build system for clang."""

    def __init__(self, examplesdir, sdkDir, verbose=False):
        """Initialize the Clang build system."""
        self.name = "Clang"
        self.verbose = verbose
        if not any([examplesdir, sdkDir]):
            raise Exception(
                "To create a clang build system, provide the following parameters: examplesdir, sdkDir"
            )
        self.examplesdir = str(examplesdir)
        self.gccPath = bashexec("which clang")[0].decode("utf-8").rstrip()
        self.sdkDir = str(sdkDir)

    def build(self, project):
        """Build a project."""
        startdir = os.getcwd()
        if self.name in project.excludeBuilds:
            print(
                bcolors.WARNING
                + "Skipping build excluded with --exclude option:\n"
                + str(project)
                + bcolors.ENDC
            )
            return 1
        print(bcolors.HEADER + "Building: " + str(project) + bcolors.ENDC)
        os.chdir(project.absPath)
        if bashexec("make -f /root/repos/BLE_SDK6_examples/build_utils/clang/Makefile", prnt=self.verbose)[1] != 0:
            print(bcolors.FAIL + str(project) + bcolors.ENDC)
            os.chdir(startdir)
            return 1
        os.chdir(startdir)
        return 0

    def check(self, project, target):
        """Check a build."""
        os.chdir(project.absPath)
        if project.cmakelistsFile and self.name not in project.excludeBuilds:
            binPath = (project.builddir+"_clang").joinpath(
                str(project.title) + ".bin"
            )
            if bashexec("test -f " + str(binPath))[1] == 0:
                project.addBuildStatus(self.name, target, True, binPath)
            else:
                project.addBuildStatus(self.name, target, False, binPath)
        return 0


class CMake:
    """Build system for CMake."""

    def __init__(self, examplesdir, sdkDir, verbose=False):
        """Initialize the CMake build system."""
        self.name = "CMake/gcc10"
        self.verbose = verbose
        if not any([examplesdir, sdkDir]):
            raise Exception(
                "To create a CMake build system, provide the following parameters: examplesdir, sdkDir"
            )
        self.examplesdir = str(examplesdir)
        self.gccPath = bashexec("which arm-none-eabi-gcc")[0].decode("utf-8").rstrip()
        self.sdkDir = str(sdkDir)

    def build(self, project):
        """Build a project."""
        startdir = os.getcwd()
        if not project.cmakelistsFile:
            print(
                bcolors.WARNING
                + "Skipping build due to missing CMakeLists.txt\n"
                + str(project)
                + bcolors.ENDC
            )
            return 1
        if self.name in project.excludeBuilds:
            print(
                bcolors.WARNING
                + "Skipping build excluded with --exclude option:\n"
                + str(project)
                + bcolors.ENDC
            )
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
            print(bcolors.FAIL + str(project) + bcolors.ENDC)
            os.chdir(startdir)
            return 1
        os.chdir(startdir)
        return 0

    def check(self, project, target):
        """Check a build."""
        os.chdir(project.absPath)
        if project.cmakelistsFile and self.name not in project.excludeBuilds:
            binPath = (project.builddir).joinpath(
                str(project.title) + "_" + str(target.acronym) + ".bin"
            )
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
        """Initialize the Keil build system."""
        self.name = "Keil/armcomp6"
        self.sdkDir = sdkDir
        self.verbose = verbose
        self.passmarker = '.axf" - 0 Error(s),'

    def build(self, project):
        """Build a project."""
        if self.name not in project.excludeBuilds:
            print(bcolors.OKBLUE + "building: " + str(project) + bcolors.ENDC)
        else:
            print(
                bcolors.WARNING
                + "not building "
                + str(project.title)
                + "... (excluded with -x option)"
                + bcolors.ENDC
            )
            return 0
        os.chdir(project.absPath)
        keilCommand = [
            "C:/Keil_v5/UV4/UV4.exe",
            "-b",
            str(project.uvprojxFile.resolve()),
            "-z",
            "-o",
            project.uvisionLogFile.name,
        ]
        if self.verbose:
            print("executing Keil command: " + str(keilCommand))
        returncode = subprocess.call(keilCommand)
        # Keil returns 0 if build is ok, 1 if there are warnings, and 2-20 if there are errors
        colors = [bcolors.OKGREEN, bcolors.WARNING] + [bcolors.FAIL] * 18
        if returncode >= len(
            colors
        ):  # this is to handle undocumented Keil return codes
            returncode = 3
        with open(project.uvisionLogFile, "r") as f:
            print(colors[returncode] + f.read() + bcolors.ENDC)
        return returncode

    def check(self, project, target):
        """Check a build."""
        if self.name not in project.excludeBuilds:
            with open(project.uvisionLogFile) as log, open(project.uvprojxFile) as proj:
                if ("<TargetName>" + target.name + "</TargetName>") in proj.read():
                    binPath = pathlib.Path(project.uvprojxFile.parent.name).joinpath(
                        "out_"
                        + target.name
                        + "/Objects/"
                        + os.path.splitext(project.title.name)[0]
                        + "_"
                        + str(target.acronym)
                        + ".bin"
                    )
                    if (
                        target.acronym + self.passmarker
                    ) in log.read():  # and (os.path.isfile(binPath)):
                        project.addBuildStatus(self.name, target, True, binPath)
                    else:
                        project.addBuildStatus(self.name, target, False, binPath)
        return 0
