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
            url="https://img.shields.io/badge/"+label+"-"+message+"-"+color+"?style=flat-square", 
            headers={'User-Agent': 'Mozilla/5.0'}
        )
        return str(urlopen(req).read(),'utf-8')

class shieldsBanner(str):
    def __init__(self):
        self.bannerTitle = "Renesas"
        self.svgString = getShieldText(self.bannerTitle, "banner", "blue")
        self.tree = ET.fromstring(self.svgString)
    
    def __str__(self):
        return str(self.tree)

    def getEndXCoordShapes(self):
        # lastRectAttrib = self.tree.findall('.//{http://www.w3.org/2000/svg}rect')[-1].attrib
        # return int(lastRectAttrib['x']) + int(lastRectAttrib['width'])
        return int(self.tree.attrib['width'])

    def getEndXCoordText(self):
        # lastRectAttrib = self.tree.findall('.//{http://www.w3.org/2000/svg}rect')[-1].attrib
        # return (int(lastRectAttrib['x']) + int(lastRectAttrib['width']))*10
        return int(self.tree.attrib['width'])*10
    
    def setFirstShield(self, label, message, color):
        shieldTree = ET.fromstring(getShieldText(label, message, color))

        self.tree.set('width', shieldTree.attrib['width'])
        
        del self.tree[1][1] # delete original message rectangle
        del self.tree[1][0] # delete original label rectangle
        self.tree[1].append(shieldTree[1][0]) # add new label rectangle
        self.tree[1].append(shieldTree[1][1]) # add new message rectangle

        del self.tree[2][1] # delete original message text
        del self.tree[2][0] # delete original label text
        self.tree[2].append(shieldTree[2][0]) # add new label text
        self.tree[2].append(shieldTree[2][1]) # add new message text

    def appendShield(self, label, message, color):
        shieldTree = ET.fromstring(getShieldText(label, message, color))
        xOffsetShapes = self.getEndXCoordShapes()
        xOffsetText = self.getEndXCoordText()
        self.tree.set('width',str(int(self.tree.attrib['width']) + int(shieldTree.attrib['width']))) # update total svg width
        shieldTree[1][0].set('x',str(xOffsetShapes)) # update label rectangle x position
        for i in range(len(shieldTree[1])-1):
            shieldTree[1][i+1].set('x',str(int(shieldTree[1][i+1].attrib['x'])+int(xOffsetShapes))) # update rectangle x position
        for i in range(len(shieldTree[2])):
            shieldTree[2][i].set('x',str(int(shieldTree[2][i].attrib['x'])+int(xOffsetText))) # update text x position
        for i in range(len(shieldTree[1])):
            self.tree[1].append(shieldTree[1][i]) # append new rectangle
        for i in range(len(shieldTree[2])):
            self.tree[2].append(shieldTree[2][i]) # append new text



        # shieldTree[1][1].set('x',str(int(shieldTree[1][1].attrib['x'])+int(xOffsetShapes))) # update message rectangle x position
        # shieldTree[2][0].set('x',str(int(shieldTree[2][0].attrib['x'])+int(xOffsetText))) # update label text x position
        # shieldTree[2][1].set('x',str(int(shieldTree[2][1].attrib['x'])+int(xOffsetText))) # update message text x position
        # self.tree[1].append(shieldTree[1][0]) # append new label rectangle
        # self.tree[1].append(shieldTree[1][1]) # append new message rectangle
        # self.tree[2].append(shieldTree[2][0]) # append new label text
        # self.tree[2].append(shieldTree[2][1]) # append new message text

    def addShield(self, label, message, color):
        if(self.tree[2][0].text==self.bannerTitle): # check if first banner not yet set
            self.setFirstShield(label, message, color)
        else:
            self.appendShield(label, message, color)

    def addWhiteSpace(self, whiteSpace):
        self.tree.set('width', str(int(self.tree.attrib['width']) + whiteSpace))

    def save(self, filepath):
        ET.ElementTree(self.tree).write(filepath)

        

if __name__ == "__main__":
    b = shieldsBanner()
    b.addShield("SDK", "6.0.18", "blue")
    b.addWhiteSpace(10)
    b.addShield("gcc","DA14531","red")
    b.addShield("","DA14531_01","red")
    b.addShield("","DA14533","red")
    b.addShield("","DA14535","red")
    b.addShield("","DA14585","inactive")
    b.addShield("","DA14586","inactive")
    b.addWhiteSpace(10)
    b.addShield("Keil","DA14531","brightgreen")
    b.addShield("","DA14531_01","brightgreen")
    b.addShield("","DA14533","brightgreen")
    b.addShield("","DA14535","brightgreen")
    b.addShield("","DA14585","inactive")
    b.addShield("","DA14586","inactive")
    b.save("generated.svg")
        