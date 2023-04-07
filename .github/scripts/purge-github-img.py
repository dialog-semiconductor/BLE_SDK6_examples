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
"""A script to send a purge request to github to purge repo images."""
import argparse
import pathlib
import os
import urllib.request

from project import ProjectList
from common import bashexec

#https://dev.to/jcubic/github-action-to-clear-cache-on-images-in-readme-5g1n

def parseArgs():
    """Get the arguments passed to script."""
    parser = argparse.ArgumentParser(
        prog="Purge git images",
        description="A script to send a purge request to github to purge repo images. only for linux",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="verbose logging",
    )
    parser.add_argument(
        "-u",
        "--url",
        required=True,
        help="The base url of the github repo",
    )
    parser.add_argument(
        "-b",
        "--branch",
        required=True,
        help="The branch where the images need to be purged",
    )
    parser.add_argument(
        "-f",
        "--datafile",
        default="projectData.json",
        help="The projects definition file. default='projectData.json'",
    )
    args = parser.parse_args()

    return args

def setVars():
    """Set the variables used in script."""
    projects = ProjectList(jsonFile=args.datafile,verbose=args.verbose)
    examplesdir = pathlib.Path(__file__).parents[2].resolve()
    startdir = pathlib.Path(os.getcwd())

    return projects, examplesdir, startdir

def purgeGithubCache(project):
    readmeUrl = str(args.url)+"/blob/"+str(args.branch)+"/"+str(project.path)+"/Readme.md"
    if args.verbose:
        print("purging readme "+readmeUrl)
    os.system("curl -s "+readmeUrl+" > readme.html")
    os.system('''grep -Eo '<img src="[^"]+"' readme.html | grep camo | grep -Eo 'https[^"]+' | xargs -I {} curl -w "\n" -s -X PURGE {}''')


if __name__ == "__main__":
    args = parseArgs()
    projects, examplesdir, startdir = setVars()

    for project in projects:
        purgeGithubCache(project)