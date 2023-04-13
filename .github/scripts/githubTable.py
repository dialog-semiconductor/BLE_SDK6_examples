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
"""This file contains a class to make a github table and put it in an svg image. 
there are two output files, one dark- and one light-mode, these need to be embedded seperatly into the target .md like so:
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://user-images.githubusercontent.com/25423296/163456776-7f95b81a-f1ed-45f7-b7ab-8fa810d529fa.png">
  <source media="(prefers-color-scheme: light)" srcset="https://user-images.githubusercontent.com/25423296/163456779-a8556205-d0a5-45e2-ac17-42d089e3c3f8.png">
  <img alt="Shows an illustrated sun in light mode and a moon with stars in dark mode." src="https://user-images.githubusercontent.com/25423296/163456779-a8556205-d0a5-45e2-ac17-42d089e3c3f8.png">
</picture>
The css for both the dark- and light-mode versions is pulled from github and embedded in the svg."""
#from urllib.request import Request, urlopen
import requests
from bs4 import BeautifulSoup as bs
from urllib.parse import urljoin
import xml.etree.ElementTree as ET


def getCssFromUrl(url):
    session = requests.Session()
    session.headers["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/44.0.2403.157 Safari/537.36"
    html = session.get(url).content
    soup = bs(html, "html.parser")
    cssFiles = []
    for css in soup.find_all("link"):
        if css.attrs.get("href"):
            cssUrl = urljoin(url, css.attrs.get("href"))
            cssFiles.append(cssUrl)
        if css.attrs.get("data-href"):
            cssUrl = urljoin(url, css.attrs.get("data-href"))
            cssFiles.append(cssUrl)

    cssFiles = list(filter(lambda k: '.css' in k, cssFiles))
    print(*cssFiles,sep="\n")

    cssText = ""
    for cssFile in cssFiles:
        cssText += str(session.get(cssFile).content, 'utf-8')

    return cssText

class githubTable(str):
    def __init__(self):
        self.text = '''<svg xmlns="http://www.w3.org/2000/svg" >\n'''

    def defineCss(self, cssText):
        self.text += "\t<style>\n"
        self.text += cssText + "\n"
        self.text += "\t</style>\n"

    def addHeader(self, colorMode):
        self.text += '''
        <foreignObject width="100%" height="1200"> 
		<body xmlns="http://www.w3.org/1999/xhtml" lang="en" data-color-mode="'''+colorMode+'''" data-light-theme="light" data-dark-theme="dark" data-a11y-animated-images="system">
			<div class="Box mt-3 position-relative">
				<div class="Box-body readme blob js-code-block-container p-5 p-xl-6 gist-border-0" style="--data-color-mode: dark">
					<article class="markdown-body entry-content container-lg" itemprop="text">
						<table>
							<tr>
								<th>Example</th>
								<th>Target(s)</th>
								<th>Tags</th>
							</tr>
        '''


    def addRow(self, project):
        self.text += '''                                                <tr>
								<td>'''+str(project.title)+'''</td>
								<td>'''+','.join(project.getAllBuildingTargets())+'''</td>
								<td>tags</td>
							</tr>
        '''

    def addFooter(self):
        self.text += '''                                        </table>
					</article>
				</div>
			</div>
		</body>
    </foreignObject>
</svg>
        '''

    def save(self, fileName):
        f = open(fileName, "w")
        f.write(self.text)
        f.close()

        

if __name__ == "__main__":
    table = githubTable()
    table.defineCss(getCssFromUrl("https://github.com/dialog-semiconductor/BLE_SDK6_examples/blob/main/Readme.md"))
    table.addHeader("light")
    table.addRow()
    table.addFooter()
    table.save("table.svg")
    print(table.text)
