# ####################################################################################
# @file build.py
#
# @brief Builds projects in repository using Cmake.
#
# Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
#
# This software ("Software") is owned by Renesas Electronics Corporation. By using this
# Software you agree that Renesas Electronics Corporation retains all intellectual
# property and proprietary rights in and to this Software and any use, reproduction,
# disclosure or distribution of the Software without express written permission or a
# license agreement from Renesas Electronics Corporation is strictly prohibited. This
# Software is solely for use on or in conjunction with Renesas Electronics Corporation
# products.
#
# EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
# REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
# IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL RENESAS
# ELECTRONICS CORPORATION BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
# CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
# PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
# ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE."""
#
# #####################################################################################

"""Builds projects in repository using Cmake."""

import argparse
import os
import shutil

from common import bashexec, findProjectFiles

parser = argparse.ArgumentParser(
    prog="cMakeBuild",
    description="Builds projects in repository using Cmake.",
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
    help="A list of directories to exclude from the build. Can be a file in any format, a string, or a list",
)
args = parser.parse_args()

projectFiles = findProjectFiles(args.projdir, verbose=args.verbose)

gccPath = bashexec("which arm-none-eabi-gcc")

if args.exclude:
    args.exclude = open(args.exclude, "r", errors="ignore").read()

for p in projectFiles:
    if p.basedir.name in args.exclude:
        if args.verbose:
            print("Excluding project from build (specified with '-x' option)")
        p.cmakelistsFile = ""

    if p.cmakelistsFile:
        shutil.rmtree(p.builddir)
        p.builddir.mkdir()

        print(p.basedir.parents[1].resolve())

        # bashexec(["cmake",
        #           "-DDEVICE_NAME="+p.title,
        #           "-DCMAKE_BUILD_TYPE=DEBUG",
        #           "-DCMAKE_TOOLCHAIN_FILE="+p.basedir.parents[1].resolve()+"/build_utils/gcc/arm-none-eabi.cmake",
        #           "-DGCC_TOOLCHAIN_PATH="+gccPath,
        #           "-DDIALOG_SDK_PATH="+args.sdkdir,
        #           "-DDIALOG_EXAMPLE_PATH="+p.basedir.parents[1].resolve(),
        #           "-S",".",
        #           "-B",p.builddir
        #           ])

# print(projectFiles)
