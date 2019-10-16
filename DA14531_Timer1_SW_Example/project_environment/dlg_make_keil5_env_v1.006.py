#! /usr/bin/python
'''
###########################################################################################
# @file		:: dlg_make_keil5_env_v1.006.py
#
# @brief	:: This script is used to create DA145 85/86/31 BLE application build environment in KEIL5. 
#			   You need DA14531/DA14585/DA14586 hardware and SDK6. We always recommeded to take the latest SDK
#			   from the support website for the new application development.
#			   Create your project only in the user application space.
#			   Do not touch any code in the SDK6.
#			   Store and run this script from the same KEIL5 project folder location.
#			   Your Keil5 uvprojx file will be ready to run the KEIL5 IDE as an ouput.
#			   
#			   Change(s) from v1.004:
#			   - Code added to parse file path changes in new SDK (6.0.11.933 and up).
#			   - Characters being read as Unicode was generating errors: culprit files now being read as UTF-8.
#				
#
# Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
#
# This software ("Software") is owned by Dialog Semiconductor. By using this Software
# you agree that Dialog Semiconductor retains all intellectual property and proprietary
# rights in and to this Software and any use, reproduction, disclosure or distribution
# of the Software without express written permission or a license agreement from Dialog
# Semiconductor is strictly prohibited. This Software is solely for use on or in
# conjunction with Dialog Semiconductor products.
#
# EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
# REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
# IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
# SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
# CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
# PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
# ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
#
###########################################################################################  
'''

import sys
import os.path
import glob
import re
import argparse
import xml.etree.ElementTree as ET

'''
Globals and constants
'''
DLG_SDK_ROOT_DIRECTORY = 'C:\\Dialog_workspace\\' # default just for example it will be filled up by sys.argv[1] input
DLG_WORKING_PROJECT_DIRECTORY = '.'
UVOPTX_FILE_EXTENSION = ".uvoptx"
UVPROJX_FILE_EXTENSION = ".uvprojx"
DLG_UVOPTX_NAME = "test" + UVOPTX_FILE_EXTENSION
DLG_UVPROJX_NAME = "test" + UVPROJX_FILE_EXTENSION
#SOC_ID = 585
SOC_ID_LIST = ['585','586','531']

SHARED_FOLDER_PATH = 'projects\\target_apps\\peripheral_examples\\shared\\'

SCATTER_FILE_PATH = ['sdk\\common_project_files\\scatterfiles\\scatterfile_common.sct', 'sdk\\common_project_files\\scatterfiles\\DA14531.sct']
SUB_STR_PATTERN_STACK_CONFIG = '.\\..\\..\\..\\..\\..\\sdk\\common_project_files\\'
DA1458X_STACK_CONFIG = 'sdk\\common_project_files\\'

COPIED_SCATTER_FILE_PATH = ['.\\..\\src\\config\\copied_scatter_585_586.sct', '.\\..\\src\\config\\copied_scatter_531.sct']
COPIED_SCATTER_FILE_PATH_1 = ''

XML_TAG = ['IncludePath', 'Misc', 'ScatterFile', 'FilePath', 'tIfile']
DLG_FIND_STR_PATTERN = ['\\sdk\\' , '\\third_party\\', '\\shared\\']
DLG_SPLIT_STR_PATTERN = [';' , ' ', '', '=']
DLG_FIND_OTHER_PATTERN = ['--symdefs']

XML_PATTERN_VARIOUS_CONTROLS = 'Targets/Target/TargetOption/TargetArmAds/Cads/VariousControls'
XML_PATTERN_LDADS = 'Targets/Target/TargetOption/TargetArmAds/LDads'

XML_PATTERN_LDADS_SCATTERFILE = 'Targets/Target/TargetOption/TargetArmAds/LDads/ScatterFile'
XML_PATTERN_LDADS_MISC = 'Targets/Target/TargetOption/TargetArmAds/LDads/Misc'
XML_PATTERN_FILE = 'Targets/Target/Groups/Group/Files/File'
XML_PATTERN_TARGET_FILENAME = 'Targets/Target/TargetName'
XML_PATTERN_OUTPUT_FILENAME = 'Targets/Target/TargetOption/TargetCommonOption/OutputName'

XML_PATTERN_TIFILE = 'Target/TargetOption/DebugOpt/tIfile'
XML_PATTERN_OVOPTX_TARGET_FILENAME = 'Target/TargetName'

LOCATION_IDX = 0	#starting location index
MAX_LOCATION_IDX = 2 #maximum number of file locations applicable for 585 = 0,586 = 1,531 = 2 


def write_xml_file(xml_tree, xml_filename):
	'''
	Write the given ElementTree tree to an xml file.
	'''
	if not isinstance(xml_tree, ET.ElementTree):
		print('ERROR: XML TREE GIVEN IS INVALID')
		exit()

	#decl = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n'''
	# keep existing first lines
	with open(xml_filename, 'r', encoding="utf8") as fl:
		decl = fl.readline()

	# Keil project xml seems to prefer ending tags, so method "html" is used.
	# a specific declaration header is prepended to the file
	# method='xml' would include that, but standalone would be missing
	xml_tree.write(xml_filename, xml_declaration=False, encoding='utf-8', method='html')
	with open(xml_filename, 'r+', encoding="utf8") as fl:
		content = fl.read()
		fl.seek(0, 0)
		fl.write(decl + content)


def build_uvoptx_element_targetname(xml_sub_element):
	'''
	Update the target name in uvoptx file
	'''
	temp_text = DLG_UVPROJX_NAME
	temp_list = temp_text.split('.')
	dlg_uvoptx_file = temp_list[0] + '.uvoptx'
	#print(dlg_uvoptx_file)
	
	tree = ET.parse(dlg_uvoptx_file)
	root = tree.getroot()
	
	dlg_uvoptx_target_file = temp_list[0] + '_'	
	
	t_element_counter = 0
	
	for t_sub_element in root.findall(xml_sub_element):
		#print(t_sub_element.text)
		t_sub_element.text = dlg_uvoptx_target_file + SOC_ID_LIST[t_element_counter]
		t_element_counter = t_element_counter + 1
		#print('TARGET PROJECT NAME IN OVOPTX FILE:: ' + t_sub_element.text)
	
		
	tree.write(dlg_uvoptx_file)
	
	x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	with open(dlg_uvoptx_file, 'r+') as file_pointer:
		file_data = file_pointer.read()
		file_pointer.seek(0, 0)
		file_pointer.write(x.rstrip('\r\n') + '\n' + file_data)
	file_pointer.close()	
	

#build_uvoptx_element_debugopt
def build_uvoptx_element_debugopt(xml_sub_element):
	'''
	Update the ini file path in uvoptx file
	'''
	#print(DLG_UVOPTX_NAME)
	tree = ET.parse(DLG_UVOPTX_NAME)
	root = tree.getroot()
	
	for t_sub_element in root.findall(xml_sub_element):
		#print(t_sub_element.text)
		temp_text = t_sub_element.text
		if t_sub_element.tag == XML_TAG[4] and t_sub_element.text != None:
			my_t_list = temp_text.split(DLG_FIND_STR_PATTERN[0],1)
			#print(my_t_list)
			if(len(my_t_list)> 1):
				single_text = DLG_SDK_ROOT_DIRECTORY.rstrip('\\') + DLG_FIND_STR_PATTERN[0] + my_t_list[-1]	
				#print(single_text)
				t_sub_element.text = single_text	
			else:
				my_t_list = temp_text.split(DLG_FIND_STR_PATTERN[2],1)
				if(len(my_t_list)> 1):
					single_text = DLG_SDK_ROOT_DIRECTORY.rstrip('\\') + "\\" + SHARED_FOLDER_PATH + my_t_list[-1]
					#print(single_text)
					t_sub_element.text = single_text	
	# tree.write(DLG_UVOPTX_NAME)
	
	# x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	# with open(dlg_uvoptx_file, 'r+') as file_pointer:
		# file_data = file_pointer.read()
		# file_pointer.seek(0, 0)
		# file_pointer.write(x.rstrip('\r\n') + '\n' + file_data)
	# file_pointer.close()
	write_xml_file(tree, DLG_UVOPTX_NAME)
	
	#print("INI FILE PATH IS UPDATED ...")
	

#build_uvoptx_element_ldads
def build_uvprojx_element_output_name(xml_sub_element):
	'''
	Update the output name in uvprojx file
	'''
	temp_text = DLG_UVPROJX_NAME
	temp_list = temp_text.split('.')
	
	target_name = temp_list[0] + '_'
	
	tree = ET.parse(DLG_UVPROJX_NAME)
	root = tree.getroot()	
	t_element_counter = 0
	
	for t_sub_element in root.findall(xml_sub_element):
		#print(t_sub_element.text)
		t_sub_element.text = target_name + SOC_ID_LIST[t_element_counter]
		t_element_counter = t_element_counter + 1
		#print('EXECUTABLE FILE NAME :: ' + t_sub_element.text)
	
	tree.write(DLG_UVPROJX_NAME)
	
	x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	with open(DLG_UVPROJX_NAME, 'r+') as file_pointer:
		file_data = file_pointer.read()
		file_pointer.seek(0, 0)
		file_pointer.write(x.rstrip('\r\n') + '\n' + file_data)
	file_pointer.close()
	
	
#build_uvoptx_element_ldads
def build_uvprojx_element_target_name(xml_sub_element):
	'''
	Update the target name in uvprojx file
	'''
	temp_text = DLG_UVPROJX_NAME
	temp_list = temp_text.split('.')
	
	target_name = temp_list[0] + '_'
	
	tree = ET.parse(DLG_UVPROJX_NAME)
	root = tree.getroot()	
	t_element_counter = 0
	
	for t_sub_element in root.findall(xml_sub_element):
		#print(t_sub_element.text)
		t_sub_element.text = target_name + SOC_ID_LIST[t_element_counter]
		t_element_counter = t_element_counter + 1
		#print('TARGET PROJECT NAME :: ' + t_sub_element.text)
	
	tree.write(DLG_UVPROJX_NAME)
	
	x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	with open(DLG_UVPROJX_NAME, 'r+') as file_pointer:
		file_data = file_pointer.read()
		file_pointer.seek(0, 0)
		file_pointer.write(x.rstrip('\r\n') + '\n' + file_data)
	file_pointer.close()	
		
	
#build_uvprojx_element_file
def build_uvprojx_element_file(xml_sub_element, split_str_pattern):	
	"""
	Update the directory paths of every file 	
	"""
	tree = ET.parse(DLG_UVPROJX_NAME)
	root = tree.getroot()
	
	for t_sub_element in root.findall(xml_sub_element):
		#print(t_sub_element.tag)
		for temp_element in t_sub_element:
			temp_tag = temp_element.tag
			temp_text = t_sub_element.find(temp_tag).text			
			if(temp_tag == XML_TAG[3]):
				#print(temp_tag , ' :: ' , temp_text)
				my_t_list = temp_text.split(DLG_FIND_STR_PATTERN[0],1)
				#print(my_t_list)
				if(len(my_t_list)> 1):
					single_text = DLG_SDK_ROOT_DIRECTORY.rstrip('\\') + DLG_FIND_STR_PATTERN[0] + my_t_list[-1]
				else:
					my_t_list = temp_text.split(DLG_FIND_STR_PATTERN[1],1)
					single_text = temp_text
					#print(my_t_list)
					if(len(my_t_list)> 1):
						single_text = DLG_SDK_ROOT_DIRECTORY.rstrip('\\') + DLG_FIND_STR_PATTERN[1] + my_t_list[-1]
					else:
						my_t_list = temp_text.split(DLG_FIND_STR_PATTERN[2],1)
						single_text = temp_text
						#print(my_t_list)
						
						if(len(my_t_list)> 1):
							single_text = DLG_SDK_ROOT_DIRECTORY.rstrip('\\') + "\\" + SHARED_FOLDER_PATH + my_t_list[-1]
					#print(my_t_list)
				#print(single_text)
					
				if (os.path.exists(single_text) == True):
					t_sub_element.find(temp_tag).text = single_text
				else:
					#print("WARNING :: IT IS AN INVALID DIRECTORY PATH, THIS PATH WILL BE AUTOMATICALLY REMOVED...")
					pass
				# print(single_text)
				
	
	# my_file = open(DLG_UVPROJX_NAME,"w") 
	# x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	# my_file.write(x)
	# my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	# my_file.close()	
	write_xml_file(tree, DLG_UVPROJX_NAME)
	
					
#build_uvprojx_element_ldads_scatterfile
def build_uvprojx_element_ldads_scatterfile(xml_sub_element):	
	"""
	Modify and update UVPROJX ldads element
	"""
	
	loop_idx = location_idx = LOCATION_IDX		
	max_location_idx = MAX_LOCATION_IDX
	
	tree = ET.parse(DLG_UVPROJX_NAME)
	root = tree.getroot()
	
	for t_sub_element in root.findall(xml_sub_element):		
			if (os.path.exists(str(COPIED_SCATTER_FILE_PATH[location_idx])) == True):			
				t_sub_element.text = str(COPIED_SCATTER_FILE_PATH[location_idx])
			elif (os.path.exists(str(COPIED_SCATTER_FILE_PATH_1)) == True):
				t_sub_element.text = str(COPIED_SCATTER_FILE_PATH_1)
			
			loop_idx = loop_idx + 1
			if (loop_idx == max_location_idx):
				location_idx = location_idx + 1
	
	
	# my_file = open(DLG_UVPROJX_NAME,"w") 
	# x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	# my_file.write(x)
	# my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	# my_file.close()
	write_xml_file(tree, DLG_UVPROJX_NAME)
	
				
def build_uvprojx_element_ldads_misc(xml_sub_element, split_str_pattern):
	"""
	Modify and update UVPROJX miscellaneous element
	"""
	tree = ET.parse(DLG_UVPROJX_NAME)
	root = tree.getroot()
	
	# target_name = DLG_UVPROJX_NAME.rstrip(UVPROJX_FILE_EXTENSION) + '_symdef.txt'
	
	for t_sub_element in root.findall(xml_sub_element):
		# print(t_sub_element.tag)
		# print(t_sub_element.text)
		if(t_sub_element.text == None):
			return
		temp_text = t_sub_element.text
		temp_text = temp_text.split(DLG_FIND_STR_PATTERN[0],1)	# Split on \\sdk\\.
		unused_txt_data = temp_text[0].split(DLG_SPLIT_STR_PATTERN[1])	# Split on space.
		temp_text[0] = unused_txt_data[0]	# Old path minus old sdk path.
		temp_text[1] = DLG_SDK_ROOT_DIRECTORY + DLG_FIND_STR_PATTERN[0] + temp_text[1]	# Add new sdk path.
		joined_text = split_str_pattern.join(temp_text)
		"""		
		if(len(temp_list[2])>1):
			updated_data = ''
			updated_data = temp_list[2][:10] + target_name
			temp_list[2] = updated_data
			#print(temp_list[2])
		"""
		t_sub_element.text = joined_text
		
	# my_file = open(DLG_UVPROJX_NAME,"w") 
	# x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	# my_file.write(x)
	# my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	# my_file.close()
	
	write_xml_file(tree, DLG_UVPROJX_NAME)


#build_uvprojx_element_various_controls
def build_uvprojx_element_various_controls(xml_sub_element, xml_tag, split_str_pattern):	
	"""
	Modify and update UVPROJX various control element
	"""
	tree = ET.parse(DLG_UVPROJX_NAME)
	root = tree.getroot()
	
	DLG_WORKING_PROJECT_PARENT_DIRECTORY = os.path.abspath(os.path.join(DLG_WORKING_PROJECT_DIRECTORY, '..'))
	DLG_INCLUDE_WORKING_PROJECT_DIRECTORY = os.path.join(DLG_WORKING_PROJECT_PARENT_DIRECTORY, 'include')
	DLG_SDK_SUBFOLDER_DIRECTORY = os.path.join(DLG_SDK_ROOT_DIRECTORY, 'sdk')
	DLG_THIRD_PARTY_SUBFOLDER_DIRECTORY = os.path.join(DLG_SDK_ROOT_DIRECTORY, 'third_party')
	# DLG_UTILITIES_SUBFOLDER_DIRECTORY = os.path.join(DLG_SDK_ROOT_DIRECTORY, 'utilities')
	DLG_PROJECT_INCLUDE_SUBFOLDER_DIRECTORY = os.path.join(DLG_WORKING_PROJECT_PARENT_DIRECTORY, 'src', 'config')
	DLG_PROJECT_USER_CUST_SUBFOLDER_DIRECTORY = os.path.join(DLG_WORKING_PROJECT_PARENT_DIRECTORY, 'src', 'custom_profile')

	updated_data = os.path.join(DLG_WORKING_PROJECT_PARENT_DIRECTORY, 'src') + '\\;'

	for dirpath, dirname, filename in os.walk(DLG_INCLUDE_WORKING_PROJECT_DIRECTORY):
		updated_data = updated_data + dirpath + '\\;'
	for dirpath, dirname, filename in os.walk(DLG_PROJECT_USER_CUST_SUBFOLDER_DIRECTORY):
		#if x = os.path.basename(dirpath)
		updated_data = updated_data + dirpath + '\\;'
	for dirpath, dirname, filename in os.walk(DLG_PROJECT_INCLUDE_SUBFOLDER_DIRECTORY):
		#if x = os.path.basename(dirpath)
		updated_data = updated_data + dirpath + '\\;'
	for dirpath, dirname, filename in os.walk(DLG_SDK_SUBFOLDER_DIRECTORY):
		#if x = os.path.basename(dirpath)
		updated_data = updated_data + dirpath + '\\;'
	for dirpath, dirname, filename in os.walk(DLG_THIRD_PARTY_SUBFOLDER_DIRECTORY):
		#if x = os.path.basename(dirpath)
		updated_data = updated_data + dirpath + '\\;'
	# for dirpath, dirname, filename in os.walk(DLG_UTILITIES_SUBFOLDER_DIRECTORY):
		# if x = os.path.basename(dirpath)
		# updated_data = updated_data + dirpath + '\\;'
	# print('DEBUG: updated_data: ' + updated_data)

	for t_sub_element in root.findall(xml_sub_element):
		# print(t_sub_element.tag)
		for temp_element in t_sub_element:
			temp_tag = temp_element.tag
			if(temp_tag == xml_tag):
				t_sub_element.find(temp_tag).text = updated_data


	#print(temp_text)
	#print(updated_data[:-1])
	#if(temp_text == updated_data[:-1]):
		#print("OK")
		
	# my_file = open(DLG_UVPROJX_NAME,"w")
	# x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	# my_file.write(x)
	# my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	# my_file.close()	
	write_xml_file(tree, DLG_UVPROJX_NAME)
	

#create the project and setup the project directory
def setup_keil5_project_environment():
	"""
	Keil5 project environment creation and configuration API
	This API will create the entire the link between the SDK files
    and the user application	
	"""
	global root, tree, COPIED_SCATTER_FILE_PATH_1
	subString = SUB_STR_PATTERN_STACK_CONFIG
	temp_flag = False
	idx = LOCATION_IDX
	max_idx = MAX_LOCATION_IDX

	if(os.path.isdir(".\\..\\src\\config\\") == False):
		COPIED_SCATTER_FILE_PATH_1 = DLG_SDK_ROOT_DIRECTORY + "\\" + SHARED_FOLDER_PATH + "peripheral_examples.sct"
		#print("SCATTER FILE :: ", COPIED_SCATTER_FILE_PATH_1)
		temp_flag = True		
	else:
		while(idx < max_idx):
			curScatterFilePath = DLG_SDK_ROOT_DIRECTORY + '\\' + SCATTER_FILE_PATH[idx]

			with open(curScatterFilePath) as my_file:
				newText = my_file.read().replace(subString, DLG_SDK_ROOT_DIRECTORY + '\\' + DA1458X_STACK_CONFIG)
				# print('NewText string : ' + newText)
			my_file.close()

			with open(COPIED_SCATTER_FILE_PATH[idx], "w") as my_file:
				my_file.write(newText)
				print("SCATTER FILE IS COPIED ...")
				print("     FROM LOCATION :: ", curScatterFilePath)
				print("     TO LOCATION :: ", COPIED_SCATTER_FILE_PATH[idx])			
			my_file.close()
			idx = idx + 1
			

		if(idx == 2):
			temp_flag = True
		else:
			print("ERROR: SCATTER FILE COPY PROCESS FAILED")
			exit()
		
	
	if (temp_flag):
	
		build_uvprojx_element_various_controls(XML_PATTERN_VARIOUS_CONTROLS, XML_TAG[0], DLG_SPLIT_STR_PATTERN[0])		
		build_uvprojx_element_ldads_scatterfile(XML_PATTERN_LDADS_SCATTERFILE)		
		build_uvprojx_element_ldads_misc(XML_PATTERN_LDADS_MISC, DLG_SPLIT_STR_PATTERN[1])	
		build_uvprojx_element_file(XML_PATTERN_FILE, DLG_SPLIT_STR_PATTERN[2])					
		#build_uvprojx_element_target_name(XML_PATTERN_TARGET_FILENAME)	
		#build_uvprojx_element_output_name(XML_PATTERN_OUTPUT_FILENAME)
		build_uvoptx_element_debugopt(XML_PATTERN_TIFILE)	
		#build_uvoptx_element_targetname(XML_PATTERN_OVOPTX_TARGET_FILENAME)
		
		print(DLG_UVPROJX_NAME + " IS SUCCESSFULLY UPDATED WITH PROPER SDK PATH ...")

	return


#verify it is a dialog keil applicaiton project
def verify_dlg_keil_app_project(path):	
	"""
	Verify there exist valid keil5 project for Dialog DA14531/DA14585/DA14586 application 
	software development.
	Multiple KEIL5 files in the same folder will generate error
	"""
	global DLG_UVOPTX_NAME, DLG_UVPROJX_NAME
	uvprojx_file_extension_counter = 0

	# Check if .uvprojx & .uvoptx file names are identical.
	files = os.listdir(path)
	for name in files:
		if name.endswith(UVPROJX_FILE_EXTENSION):
			DLG_UVPROJX_NAME = name
			uvprojx_file_extension_counter += 1
		if name.endswith(UVOPTX_FILE_EXTENSION):
			DLG_UVOPTX_NAME = name
	if(DLG_UVPROJX_NAME.replace(UVPROJX_FILE_EXTENSION,"") != DLG_UVOPTX_NAME.replace(UVOPTX_FILE_EXTENSION,"")):
		print("ERROR		:	FILE NAME OF .UVOPTX AND .UVPROJX FILES ARE NOT IDENTICAL.\n" + DLG_UVOPTX_NAME + "\n" + DLG_UVPROJX_NAME)
		return False

	if uvprojx_file_extension_counter == 1:		
		print('KEIL PROJECT NAME :: ' + path + DLG_UVPROJX_NAME + ' IS A VALID PROJECT DIRECTORY...')
	elif uvprojx_file_extension_counter > 1:
		print("ERROR		:	MULTIPLE FILES WITH " + UVPROJX_FILE_EXTENSION + " EXIST ...")
		print("RESOLUTION	:	ONLY ONE FILE WITH " + UVPROJX_FILE_EXTENSION + " IS EXPECTED INSIDE KEIL PROJECT FOLDER ...")
		return False
	else:
		print("ERROR		:	NO FILES WITH " + UVPROJX_FILE_EXTENSION + " EXIST ...")
		print("RESOLUTION 	:	THIS SCRIPT SHOULD BE EXECUTED INSIDE A PROJECT DIRECTORY ...")
		return False
	return True


#verify dialog sdk root directory path
def verify_dlg_sdk_root_directory(path):
	"""
	Verify that the dialog SDK path is having proper folder path
	"""
	files = os.listdir(path)
	file_name_counter = 0
	for name in files:
		#print(name)
		if (name == 'sdk' or name == 'third_party'):
			file_name_counter += 1
	if (file_name_counter != 2 ):
		print("ERROR 		:	SDK IS AN INVALID DIRECTORY PATH ... ...")
		print("RESOLUTION 	: 	VERIFY THE SDK DIRECTORY PATH ...")
		return False
	return True


def run_application(sdk_path):
	"""
	Verify if the sdkpath is valid
	Verify if the KEIL5 uvprojx file is valid
	If both are verified then setup KEIL5 project environment
	"""
	global DLG_SDK_ROOT_DIRECTORY, DLG_WORKING_PROJECT_DIRECTORY
	#os.system('cls') #works only in windows for linux it is 'clear'			
	
	DLG_SDK_ROOT_DIRECTORY = str(sdk_path)
	
	if (verify_dlg_sdk_root_directory(DLG_SDK_ROOT_DIRECTORY) == False):
		exit()

	print('\nSDK LOCATION :: ' + DLG_SDK_ROOT_DIRECTORY + ' IS FOUND ...')
	
	DLG_WORKING_PROJECT_DIRECTORY = str(os.getcwd()) + "\\"
	
	if (verify_dlg_keil_app_project(DLG_WORKING_PROJECT_DIRECTORY) == False):
		print("ABORTING.")
		exit()
	
	setup_keil5_project_environment()
	

# start application
if __name__ == "__main__":	
	# construct the argument parse and parse the arguments
	ap = argparse.ArgumentParser()
	ap.add_argument("-sdkpath", "--sdkpath", required=True,	help="FULL SDK PATH INSIDE DOUBLE QUOTATION")
	args = vars(ap.parse_args())
	 
	# display a friendly message to the user
	#print("SDK location {},is the sdk path".format(args["sdkpath"]))	
	run_application(args["sdkpath"])