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
"""This file contains a class to make an svg banner for a readme file."""
from urllib.request import Request, urlopen
import xml.etree.ElementTree as ET

def getShieldText(label, message, color):
        req = Request(
            url='https://img.shields.io/badge/Renesas-banner-blue?style=flat-square', 
            headers={'User-Agent': 'Mozilla/5.0'}
        )
        return str(urlopen(req).read(),'utf-8')

class shieldsBanner(str):
    def __init__(self):
        self.svgString = getShieldText("Renesas", "banner", "blue")
        self.tree = ET.fromstring(self.svgString)
    
    def __str__(self):
        return str(self.tree)

    def getEndXCoord(self):
        lastRectAttrib = self.tree.findall('.//{http://www.w3.org/2000/svg}rect')[-1].attrib
        return int(lastRectAttrib['x']) + int(lastRectAttrib['width'])

    def setFirstShield(self, label, message, color):
        shieldTree = ET.fromstring(getShieldText(label, message, color))
        self.tree[1][0] = shieldTree[1][0]
        self.tree[1][1] = shieldTree[1][0]

    def save(self, filepath):
        print(str(self.tree))
        # with open(filepath, mode='w', encoding='utf-8') as out_file:
        #     ET.canonicalize(self.tree, out=out_file)
        ET.ElementTree(self.tree).write(filepath)

        

if __name__ == "__main__":
    b = shieldsBanner()
    print(b.getEndXCoord())
    b.setFirstShield("SDK", "6.0.18", "blue")
    b.save("generated.svg")
        