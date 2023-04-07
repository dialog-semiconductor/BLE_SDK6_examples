# #####################################################################################
# The MIT License (MIT)
# Copyright (c) 2023 Renesas Electronics Corporation and/or its affiliates
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EX    PRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
# #####################################################################################
"""This file contains a class to make an svg banner for a readme file."""
import xml.etree.ElementTree as ET
from urllib.request import Request, urlopen


def getShieldText(label, message, color):
    """Get a shield with a specific label, message and color from shields.io."""
    req = Request(
        url="https://img.shields.io/badge/"
        + label
        + "-"
        + message
        + "-"
        + color
        + "?style=flat-square",
        headers={"User-Agent": "Mozilla/5.0"},
    )
    return str(urlopen(req).read(), "utf-8")


class shieldsBanner(str):
    """Represents a shields banner in text format."""

    def __init__(self):
        """Initialize the banner."""
        self.bannerTitle = "Renesas"
        self.svgString = getShieldText(self.bannerTitle, "banner", "blue")
        self.tree = ET.fromstring(self.svgString)

    def __str__(self):
        """Get the shields banner as a string."""
        return str(self.tree)

    def getEndXCoordShapes(self):
        """Get the current maximum shapes X coordinate of the svg."""
        return int(self.tree.attrib["width"])

    def getEndXCoordText(self):
        """Get the current maximum text X coordinate of the svg."""
        return int(self.tree.attrib["width"]) * 10

    def setFirstShield(self, label, message, color):
        """Set the label, message and color of the first shield by replacing the existing label and message."""
        shieldTree = ET.fromstring(getShieldText(label, message, color))
        self.tree.set("width", shieldTree.attrib["width"])
        del self.tree[1][1]  # delete original message rectangle
        del self.tree[1][0]  # delete original label rectangle
        self.tree[1].append(shieldTree[1][0])  # add new label rectangle
        self.tree[1].append(shieldTree[1][1])  # add new message rectangle
        del self.tree[2][1]  # delete original message text
        del self.tree[2][0]  # delete original label text
        self.tree[2].append(shieldTree[2][0])  # add new label text
        self.tree[2].append(shieldTree[2][1])  # add new message text

    def appendShield(self, label, message, color):
        """Set the label, message and color of the first shield by requesting a new shield from shields.io and inserting the elements into the existing label."""
        shieldTree = ET.fromstring(getShieldText(label, message, color))
        xOffsetShapes = self.getEndXCoordShapes()
        xOffsetText = self.getEndXCoordText()
        self.tree.set(
            "width",
            str(int(self.tree.attrib["width"]) + int(shieldTree.attrib["width"])),
        )  # update total svg width
        shieldTree[1][0].set(
            "x", str(xOffsetShapes)
        )  # update label rectangle x position
        for i in range(len(shieldTree[1]) - 1):
            shieldTree[1][i + 1].set(
                "x", str(int(shieldTree[1][i + 1].attrib["x"]) + int(xOffsetShapes))
            )  # update rectangle x position
        for i in range(len(shieldTree[2])):
            shieldTree[2][i].set(
                "x", str(int(shieldTree[2][i].attrib["x"]) + int(xOffsetText))
            )  # update text x position
        for i in range(len(shieldTree[1])):
            self.tree[1].append(shieldTree[1][i])  # append new rectangle
        for i in range(len(shieldTree[2])):
            self.tree[2].append(shieldTree[2][i])  # append new text

    def addShield(self, label, message, color):
        """Add a shield by selecting the correct method depending on existing structure."""
        if (
            self.tree[2][0].text == self.bannerTitle
        ):  # check if first banner not yet set
            self.setFirstShield(label, message, color)
        else:
            self.appendShield(label, message, color)

    def addWhiteSpace(self, whiteSpace):
        """Add a white space to the banner by increasing the total svg size."""
        self.tree.set("width", str(int(self.tree.attrib["width"]) + whiteSpace))

    def save(self, filepath):
        """Save the svg into a file specified by filepath."""
        ET.ElementTree(self.tree).write(filepath)
