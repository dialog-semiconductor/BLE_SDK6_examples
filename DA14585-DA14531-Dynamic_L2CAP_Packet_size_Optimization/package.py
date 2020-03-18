import sys,os
from shutil import copytree,copyfile,rmtree
import subprocess
import re
from zipfile import ZipFile

import argparse

def get_all_file_paths(directory):

    # initializing empty file paths list
    file_paths = []

    # crawling through directory and subdirectories
    for root, directories, files in os.walk(directory):
        for filename in files:
            # join the two strings in order to form the full filepath.
            filepath = os.path.join(root, filename)
            file_paths.append(filepath)

    # returning all file paths
    return file_paths




parser = argparse.ArgumentParser(description='Package examples for SDK10')
parser.add_argument('example', nargs=1,help='Path to the examples')
parser.add_argument( '-pv','--pack_version', type=str,
                    default='1.0', help='Package version')

args = parser.parse_args()




print("Packaging", args.example[0])

processor = os.path.join('..','Small_Example_Processor','source')
buildfolder = os.path.join('..','build')
# clean the processor
try:
    rmtree(os.path.join(processor,'assets'))
except:
    pass
try:
    os.remove(os.path.join(processor,'Readme.md'))
except:
    pass

copyfile(os.path.join(args.example[0],'Readme.md'),os.path.join(processor,'Readme.md'))
try:
    copytree(os.path.join(args.example[0],'assets'),os.path.join(processor,'assets'))
except:
    pass


file = open(os.path.join(args.example[0],'Readme.md'), "r")
for line in file:
    obj =  re.match(r'#(\s+)(.+)',line)
    if obj:
        break

os.environ['EXNAME'] = obj[2]
rootfolder = os.getcwd()
os.chdir(os.path.join(processor,'..'))
subprocess.run(["make", "clean"])
subprocess.run(["make", "html"])


os.chdir(rootfolder)

#clean first
try:
    rmtree(os.path.join(buildfolder,args.example[0]))
except:
    pass


copytree(args.example[0],os.path.join(buildfolder,args.example[0]))
# clean the processor
try:
    rmtree(os.path.join(buildfolder,args.example[0],'assets'))
except:
    pass
try:
    os.remove(os.path.join(buildfolder,args.example[0],'Readme.md'))
except:
    pass

copytree(os.path.join(processor,'..','build','html'),os.path.join(buildfolder,args.example[0],'doc'))

os.chdir(os.path.join(buildfolder,args.example[0]))


#Add version
fp = open('version.txt','w')
fp.write(args.pack_version)
fp.close()


allfiles = get_all_file_paths('.')
#zip everything
# writing files to a zipfile
with ZipFile('da1469x_{}.zip'.format(args.example[0]),'w') as zip:
    # writing each file one by one
    for file in allfiles :
        zip.write(file)


print(os.getcwd())
