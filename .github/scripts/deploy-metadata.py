# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
# #####################################################################################
"""This script deploys the previously built artifacts and the metadata to the AWS server."""
import argparse
import json
import os
import pathlib
import shutil

from common import getTargetsFile, bashexec
from project import ProjectList

shieldsIoPrefix = '''<img src="https://img.shields.io/badge/'''
shieldsIoSuffix = '''?style=flat-square" alt="banner">'''

def parseArgs():
    """Get the arguments passed to script."""
    parser = argparse.ArgumentParser(
        prog="Deploy metadata",
        description="deploys metadata made by build script.",
    )
    parser.add_argument(
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
        "--datafile",
        default="projectData.json",
        help="The projects definition file. default='projectData.json'",
    )
    parser.add_argument(
        "--metadata-dir",
        default="metadata",
        help="The projects definition file. default='metadata'",
    )
    args = parser.parse_args()

    return args


def setVars():
    """Set the variables used in script."""
    projects = ProjectList(jsonFile=args.datafile,verbose=args.verbose)
    targets = getTargetsFile(args.targets)
    examplesdir = pathlib.Path(__file__).parents[2].resolve()
    startdir = pathlib.Path(os.getcwd())
    metadatadir = startdir.joinpath(args.metadata_dir)

    return projects, targets, examplesdir, startdir, metadatadir

def makeBadgeBanner(project, filePath, allBuildSystems, allTargets):
    bannerText = ""
    for buildSystem in allBuildSystems:
        print(buildSystem)
        bannerText += shieldsIoPrefix
        bannerText += buildSystem + '%20builds-'
        for target in allTargets:
            targetFound = False
            for build in project.buildStatus:
                if (buildSystem == build["buildsystem"])and(target == build["target"]["name"]):
                    if not bannerText.endswith(buildSystem + '%20builds-'):
                        bannerText += shieldsIoPrefix + "-"
                    bannerText += build["target"]["name"] + "-"
                    bannerText += "brightgreen" if build["passed"] is True else "red"
                    bannerText += shieldsIoSuffix
                    targetFound = True
            if not targetFound:
                if not bannerText.endswith(buildSystem + '%20builds-'):
                    bannerText += shieldsIoPrefix + "-"
                bannerText += target + "-"
                bannerText += "inactive"
                bannerText += shieldsIoSuffix
        bannerText += "\n"

    with open(filePath, "w") as outfile:
        outfile.write(str(bannerText))

    return "<p>"+str(project.title) + ": " + bannerText + "</p>"

def findAllBuildSystems(projects):
    buildSystems = []
    for project in projects:
        for build in project.buildStatus:
            buildSystems.append(build["buildsystem"])
    return set(buildSystems)

def findAllTargets(projects):
    targets = []
    for project in projects:
        for build in project.buildStatus:
            targets.append(build["target"]["name"])
    return set(targets)


if __name__ == "__main__":
    args = parseArgs()
    projects, targets, examplesdir, startdir, metadatadir = setVars()
    allBuildSystems = findAllBuildSystems(projects)
    allTargets = findAllTargets(projects)
    debugBannerText = ""

    for project in projects:
        bannerDirPath = metadatadir.joinpath(project.path)
        bannerDirPath.mkdir(parents=True,exist_ok=True)
        bannerFilePath = bannerDirPath.joinpath("banner.html")
        debugBannerText += makeBadgeBanner(project, bannerFilePath, allBuildSystems, allTargets)
    
    with open(metadatadir.joinpath("debugBanner.html"), "w") as outfile:
        outfile.write(str(debugBannerText))

    os.chdir(startdir)