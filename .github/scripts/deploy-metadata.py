# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
# #####################################################################################
"""This script deploys the previously built artifacts and the metadata to the AWS server."""
import argparse
import os
import pathlib

from banner import shieldsBanner
from common import bashexec
from project import ProjectList


def parseArgs():
    """Get the arguments passed to script."""
    parser = argparse.ArgumentParser(
        prog="Deploy metadata",
        description="deploys metadata made by build script.",
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
    projects = ProjectList(jsonFile=args.datafile, verbose=args.verbose)
    examplesdir = pathlib.Path(__file__).parents[2].resolve()
    startdir = pathlib.Path(os.getcwd())
    metadatadir = startdir.joinpath(args.metadata_dir)

    return projects, examplesdir, startdir, metadatadir


def makeBadgeBanner(project, filePath, allBuildSystems, allTargets):
    """Make the banner with shields.io badges."""
    if args.verbose:
        print("generating banner for " + str(project.title))

    banner = shieldsBanner()
    banner.addShield("SDK", "6.0.22", "blue")

    for buildSystem in allBuildSystems:
        firstShieldAdded = False
        banner.addWhiteSpace(10)
        for target in allTargets:
            targetFound = False
            for build in project.buildStatus:
                if (buildSystem == build["buildsystem"]) and (
                    target == build["target"]["name"]
                ):
                    if not firstShieldAdded:
                        banner.addShield(
                            str(buildSystem + "%20builds"),
                            build["target"]["name"],
                            "brightgreen" if build["passed"] is True else "red",
                        )
                        firstShieldAdded = True
                    else:
                        banner.addShield(
                            "",
                            build["target"]["name"],
                            "brightgreen" if build["passed"] is True else "red",
                        )
                    targetFound = True
            if not targetFound:
                if not firstShieldAdded:
                    banner.addShield(str(buildSystem + "%20builds"), target, "inactive")
                    firstShieldAdded = True
                else:
                    banner.addShield("", target, "inactive")

    banner.save(filePath)


def findAllBuildSystems(projects):
    """Find all build systems that are present in the projects."""
    buildSystems = []
    for project in projects:
        for build in project.buildStatus:
            buildSystems.append(build["buildsystem"])
    buildSystems = set(buildSystems)
    if args.verbose:
        print("found buildSystems " + str(buildSystems))
    return buildSystems


def findAllTargets(projects):
    """Find all targets that are present in the projects."""
    targets = []
    for project in projects:
        for build in project.buildStatus:
            targets.append(build["target"]["name"])
    targets = set(targets)
    if args.verbose:
        print("found targets " + str(targets))
    return targets


def synchFilesAws():
    """Synchronize the files to AWS."""
    print("Syncing metadata to AWS...")
    command = [
        "aws",
        "s3",
        "sync",
        "--delete",
        str(metadatadir),
        "s3://lpccs-docs.renesas.com/metadata/BLE_SDK6_examples",
    ]
    if args.verbose:
        print("Executing " + str(command))
    bashexec(command)


if __name__ == "__main__":
    args = parseArgs()
    projects, examplesdir, startdir, metadatadir = setVars()
    allBuildSystems = findAllBuildSystems(projects)
    allTargets = findAllTargets(projects)

    for project in projects:
        bannerDirPath = metadatadir.joinpath(project.path)
        bannerDirPath.mkdir(parents=True, exist_ok=True)
        bannerFilePath = bannerDirPath.joinpath("banner.svg")
        makeBadgeBanner(project, bannerFilePath, allBuildSystems, allTargets)

    synchFilesAws()

    os.chdir(startdir)
