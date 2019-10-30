#! /usr/bin/python
'''
###########################################################################################
# @file		:: dlg_make_keil5_env_v1.006.py
#
# @brief	:: Last modified: Oct 25th 2019.
#			   
# 			   This script is used to create DA145 85/86/31 BLE application build environment in KEIL5. 
#			   You need DA14531/DA14585/DA14586 hardware and SDK6. We always recommeded to take the latest SDK
#			   from the support website for the new application development.
#			   Create your project only in the user application space.
#			   Do not touch any code in the SDK6.
#			   Store and run this script from the same KEIL5 project folder location.
#			   Your Keil5 uvprojx file will be ready to run the KEIL5 IDE as an ouput.
#			   
#			   Change(s) from v1.002:
#			   - Code added to parse file path changes in new SDK (6.0.12).
#			   - Can now provide --clean argument instead of --sdkpath to clean all SDK paths to remove sensitive information.
#				
#
# Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
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
DLG_SDK_ROOT_DIRECTORY = "C:\\Dialog_workspace\\"	# default just for example it will be filled up by sys.argv[1] input
DLG_SDK_ROOT_DIRECTORY_TO_WRITE = "C:\DLG_SDK_UNLINKED"
DLG_WORKING_PROJECT_DIRECTORY = '.'
CLEAN_PROJ_ENV = False
IS_PROJ_ENV_IN_SDK = False
UVOPTX_FILE_EXTENSION = ".uvoptx"
UVPROJX_FILE_EXTENSION = ".uvprojx"
DLG_UVOPTX_NAME = "test" + UVOPTX_FILE_EXTENSION
DLG_UVPROJX_NAME = "test" + UVPROJX_FILE_EXTENSION
#SOC_ID = 585
SOC_ID_LIST = ['585','586','531']

SHARED_FOLDER_PATH = '\\projects\\target_apps\\peripheral_examples\\shared\\'

SCATTER_FILE_NAME = ["scatterfile_common.sct", "DA14531.sct"]
SCATTER_FILE_PATH = [('\\sdk\\common_project_files\\scatterfiles\\' + SCATTER_FILE_NAME[0]), ('\\sdk\\common_project_files\\scatterfiles\\' + SCATTER_FILE_NAME[1])]
SUB_STR_PATTERN_STACK_CONFIG = '.\\..\\..\\..\\..\\..\\sdk\\common_project_files\\'
DA1458X_STACK_CONFIG = '\\sdk\\common_project_files\\'

COPIED_SCATTER_FILE_NAME = ["copied_scatter_585_586.sct", "copied_scatter_531.sct"]
COPIED_SCATTER_FILE_PATH = [('.\\..\\src\\config\\' + COPIED_SCATTER_FILE_NAME[0]), ('.\\..\\src\\config\\' + COPIED_SCATTER_FILE_NAME[1])]
SDK_PERIPH_EX_SCATTER_FILE_PATH = ''

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



def split_path(path,compare_string):
    """
    Returns type: (boolean,string). 
    Function searches for "compare_string" in "path" with the highest index.
    if "compare_string" is found in "path" split_path will remove everything in front of it and return (True,Remaining string).
    For example "path" = 6.0.12.1020/sdk/platform and "compare_string" = /sdk/, (True,"/sdk/platform") will be returned 
    If "compare_string" was not found in "path" split_path will return (False,path) 
    """

    index = path.rfind(compare_string)

    if index == -1:
        return (False,path)

    return (True,path.replace(path[:index],""))
	
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
	temp_text = DLG_UVOPTX_NAME
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
	

def build_uvoptx_element_debugopt(xml_sub_element, xml_tag):
	'''
	Update the ini file path in .uvoptx file.
	'''
	#print(DLG_UVOPTX_NAME)
	tree = ET.parse(DLG_UVOPTX_NAME)
	root = tree.getroot()
	
	for t_sub_element in root.findall(xml_sub_element):
		#print(t_sub_element.text)
		temp_text = t_sub_element.text
		if t_sub_element.tag == xml_tag and t_sub_element.text != None:
			(found_sdk,my_t_list_sdk) = split_path(temp_text,DLG_FIND_STR_PATTERN[0])
			(found_shared,my_t_list_shared) = split_path(temp_text,DLG_FIND_STR_PATTERN[2])

			if found_sdk and found_shared: #If user named a folder in path sdk or shared 
				if len(my_t_list_sdk) < len(my_t_list_shared):
					found_shared = False
				else:			
					found_sdk = False

			if(found_sdk):
				single_text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + my_t_list_sdk
				#print(single_text)
				t_sub_element.text = single_text	
			elif(found_shared):
				single_text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + SHARED_FOLDER_PATH + my_t_list_shared.replace(DLG_FIND_STR_PATTERN[2],"")
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
		
	
def build_uvprojx_element_file(xml_sub_element, xml_tag):
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
			if(temp_tag == xml_tag):
				#print(temp_tag , ' :: ' , temp_text)

				(found_sdk,sdk_string) = split_path(temp_text,DLG_FIND_STR_PATTERN[0])
				(found_third_party,third_party_string) = split_path(temp_text,DLG_FIND_STR_PATTERN[1])
				(found_shared,shared_string) = split_path(temp_text,DLG_FIND_STR_PATTERN[2])

				if (found_sdk + found_shared + found_third_party) == 0:
					#WARNING no sdk path
					continue	
				
				end_of_path = min([sdk_string,third_party_string,shared_string], key=len)
				single_text = ""

				if end_of_path.find(DLG_FIND_STR_PATTERN[2]) != -1:
					single_text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + SHARED_FOLDER_PATH + end_of_path.replace(DLG_FIND_STR_PATTERN[2],"")
				else:
					single_text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + end_of_path

				if ((os.path.exists(single_text) == True) or (CLEAN_PROJ_ENV == True)):
					t_sub_element.find(temp_tag).text = single_text
				else:
					#print("WARNING :: IT IS AN INVALID DIRECTORY PATH, THIS PATH WILL BE AUTOMATICALLY REMOVED...")
					pass
				print(single_text)
	
	# my_file = open(DLG_UVPROJX_NAME,"w") 
	# x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	# my_file.write(x)
	# my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	# my_file.close()	
	write_xml_file(tree, DLG_UVPROJX_NAME)
	
					
def build_uvprojx_element_ldads_scatterfile(xml_sub_element):
	"""
	Modify and update UVPROJX ldads scatterfile element
	"""
	loop_idx = location_idx = LOCATION_IDX		
	max_location_idx = MAX_LOCATION_IDX
	
	tree = ET.parse(DLG_UVPROJX_NAME)
	root = tree.getroot()
	
	for t_sub_element in root.findall(xml_sub_element):
		if(CLEAN_PROJ_ENV):
			if(SDK_PERIPH_EX_SCATTER_FILE_PATH == ""):	# .sct file in SDK not used.
				t_sub_element.text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + COPIED_SCATTER_FILE_NAME[location_idx]
			else:
				t_sub_element.text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + SHARED_FOLDER_PATH + "peripheral_examples.sct"
		if (os.path.exists(str(COPIED_SCATTER_FILE_PATH[location_idx])) == True):			
			t_sub_element.text = str(COPIED_SCATTER_FILE_PATH[location_idx])
		elif (os.path.exists(str(SDK_PERIPH_EX_SCATTER_FILE_PATH)) == True):
			t_sub_element.text = str(SDK_PERIPH_EX_SCATTER_FILE_PATH)

		loop_idx += 1
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
	Modify and update UVPROJX ldads miscellaneous element
	"""
	check_strs = [":", "..\\", ".\\", " "]


	tree = ET.parse(DLG_UVPROJX_NAME)
	root = tree.getroot()
	
	# target_name = DLG_UVPROJX_NAME.rstrip(UVPROJX_FILE_EXTENSION) + '_symdef.txt'
	
	for t_sub_element in root.findall(xml_sub_element):
		if(t_sub_element.text == None):
			return
		# print(t_sub_element.tag)
		print("Current LDad Misc: " + t_sub_element.text)
		misc_element = t_sub_element.text

		(found_sdk,divided_misc) = split_path(misc_element,DLG_FIND_STR_PATTERN[0])

		if (found_sdk) == 0:
			#WARNING no sdk path
			pass #continue	
		
		feedback_and_path_begin = misc_element.replace(divided_misc,"")
		# print("ldads_misc: Feedback + previous user path = " + feedback_and_path_begin)

		(found_drive,drive_path) = split_path(feedback_and_path_begin,check_strs[0])
		(found_relative,relative_path) = split_path(feedback_and_path_begin,check_strs[1])
		(found_dot,dot_path) = split_path(feedback_and_path_begin,check_strs[2])

		if (found_drive+found_relative+found_dot) == 0:
			#error
			pass #continue

		if found_relative:
			# print("ldads_misc: Relative path = " + relative_path)
			(found_relative,relative_path) = split_path(feedback_and_path_begin,check_strs[3])
			feedback = feedback_and_path_begin.replace(relative_path,"") + " "
		elif found_drive:
			feedback = feedback_and_path_begin.replace(drive_path,"")
			# print("ldads_misc: Drive_path = " + drive_path)
			feedback = feedback[:-1]
		else:
			# print("ldads_misc: Dot path = " + dot_path)
			feedback = feedback_and_path_begin.replace(dot_path,"")

		# print("ldads_misc: feedback = " + feedback)

		t_sub_element.text = feedback + DLG_SDK_ROOT_DIRECTORY_TO_WRITE + divided_misc
	# Add new sdk path.
		"""		
		if(len(temp_list[2])>1):
			updated_data = ''
			updated_data = temp_list[2][:10] + target_name
			temp_list[2] = updated_data
			#print(temp_list[2])
		"""
		print("New LDad Misc    : " + t_sub_element.text)
	# my_file = open(DLG_UVPROJX_NAME,"w") 
	# x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	# my_file.write(x)
	# my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	# my_file.close()
	
	write_xml_file(tree, DLG_UVPROJX_NAME)


def build_uvprojx_element_various_controls(xml_sub_element, xml_tag):
	"""
	Modify and update UVPROJX various control element
	"""
	tree = ET.parse(DLG_UVPROJX_NAME)
	root = tree.getroot()

	updated_data = ""
	
	if(CLEAN_PROJ_ENV == True):
		updated_data = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + ";"
	else: 
		DLG_WORKING_PROJECT_PARENT_DIRECTORY = "..\\"
		DLG_INCLUDE_WORKING_PROJECT_DIRECTORY = os.path.join(DLG_WORKING_PROJECT_PARENT_DIRECTORY, 'include')
		DLG_SDK_SUBFOLDER_DIRECTORY = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + DLG_FIND_STR_PATTERN[0][:-1]
		DLG_THIRD_PARTY_SUBFOLDER_DIRECTORY = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + DLG_FIND_STR_PATTERN[1][:-1]
		# DLG_UTILITIES_SUBFOLDER_DIRECTORY = os.path.join(DLG_SDK_ROOT_DIRECTORY, 'utilities')
		DLG_PROJECT_INCLUDE_SUBFOLDER_DIRECTORY = os.path.join(DLG_WORKING_PROJECT_PARENT_DIRECTORY, 'src', 'config')
		DLG_PROJECT_USER_CUST_SUBFOLDER_DIRECTORY = os.path.join(DLG_WORKING_PROJECT_PARENT_DIRECTORY, 'src', 'custom_profile')

		end_ele_char = ";"
		updated_data = os.path.join(DLG_WORKING_PROJECT_PARENT_DIRECTORY, 'src') + end_ele_char

		for dirpath, dirname, filename in os.walk(DLG_INCLUDE_WORKING_PROJECT_DIRECTORY):
			updated_data = updated_data + dirpath + end_ele_char
		for dirpath, dirname, filename in os.walk(DLG_PROJECT_USER_CUST_SUBFOLDER_DIRECTORY):
			#if x = os.path.basename(dirpath)
			updated_data = updated_data + dirpath + end_ele_char
		for dirpath, dirname, filename in os.walk(DLG_PROJECT_INCLUDE_SUBFOLDER_DIRECTORY):
			#if x = os.path.basename(dirpath)
			updated_data = updated_data + dirpath + end_ele_char
		for dirpath, dirname, filename in os.walk(DLG_SDK_SUBFOLDER_DIRECTORY):
			#if x = os.path.basename(dirpath)
			updated_data = updated_data + dirpath + end_ele_char
		for dirpath, dirname, filename in os.walk(DLG_THIRD_PARTY_SUBFOLDER_DIRECTORY):
			#if x = os.path.basename(dirpath)
			updated_data = updated_data + dirpath + end_ele_char
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

	# Print updated_data in a readable format.
	updated_data = updated_data.replace(";", ";\r\n")
	print("IncludePath Element Text:\r\n" + updated_data + "End of IncludePath Element Text.\r\n")

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


def calc_proj_env_to_SDK_path():
	"""
	Uses relative part of path from project environment to SDK as SDK root.
	"""
	global DLG_SDK_ROOT_DIRECTORY
	relative_path = ""
	path_diff = DLG_WORKING_PROJECT_DIRECTORY.replace(DLG_SDK_ROOT_DIRECTORY, "")

	while "\\" in path_diff:
		path_diff = path_diff.replace("\\", "", 1)
		relative_path += "..\\"

	if relative_path == "":	# If project environment is same as SDK root.
		relative_path = ".\\"

	print("USING RELATIVE PATH \"" + relative_path + "\" INSTEAD OF ABSOLUTE \"" + DLG_SDK_ROOT_DIRECTORY + "\" AS SDK ROOT ...")
	DLG_SDK_ROOT_DIRECTORY = relative_path[:-1]


def determine_proj_env_in_SDK():
	"""
	True if project environment is located inside of SDK.
	"""
	global IS_PROJ_ENV_IN_SDK

	if(DLG_WORKING_PROJECT_DIRECTORY.startswith(os.path.abspath(DLG_SDK_ROOT_DIRECTORY))):
		print("PROJECT ENVIRONMENT LOCATED INSIDE OF SDK ...")
		IS_PROJ_ENV_IN_SDK = True
	else:
		# print("PROJECT ENVIRONMENT LOCATED OUTSIDE OF SDK ...")
		pass


def update_scatter_file():
	"""
	If scatter file(s) present in project environment, copy scatter file(s) data from SDK to proj env.
	If scatter file(s) not present in proj env, prepare path to scatter file(s) in SDK.

	Exit if scatter file(s) copy process unsuccessful.
	Return True if successful.
	"""
	global SDK_PERIPH_EX_SCATTER_FILE_PATH
	sub_string = SUB_STR_PATTERN_STACK_CONFIG
	idx = LOCATION_IDX
	max_idx = MAX_LOCATION_IDX

	if(os.path.isdir(".\\..\\src\\config\\") == False):
		if(CLEAN_PROJ_ENV == True):
			print("NO SCATTER FILE(S) TO CLEAN ...")
		elif(CLEAN_PROJ_ENV == False):
			SDK_PERIPH_EX_SCATTER_FILE_PATH = DLG_SDK_ROOT_DIRECTORY + SHARED_FOLDER_PATH + "peripheral_examples.sct"
			print("SCATTER FILE :: ", SDK_PERIPH_EX_SCATTER_FILE_PATH)
	else:	# Update scatter file(s) in proj env with data from SDK scatter file(s).
		if(CLEAN_PROJ_ENV == True):
			while(idx < max_idx):
				cur_scatter_file_path = ".\\..\\src\\config\\" + COPIED_SCATTER_FILE_NAME[idx]

				if(os.path.exists(cur_scatter_file_path)):	# .sct file might not have been created yet.
					os.remove(cur_scatter_file_path)

				print("CLEANED SCATTER FILE :: " + cur_scatter_file_path)
				idx += 1
			return True
		while(idx < max_idx):
			cur_scatter_file_path = DLG_SDK_ROOT_DIRECTORY + SCATTER_FILE_PATH[idx]
			new_text = ""
			
			with open(cur_scatter_file_path) as my_file:
				new_text = my_file.read().replace(sub_string, DLG_SDK_ROOT_DIRECTORY_TO_WRITE + DA1458X_STACK_CONFIG)
				# print('NewText string : ' + new_text)
			my_file.close()

			with open(COPIED_SCATTER_FILE_PATH[idx], "w") as my_file:
				my_file.write(new_text)
				print("SCATTER FILE IS COPIED ...")
				print("     FROM LOCATION :: ", cur_scatter_file_path)
				print("     TO LOCATION :: ", COPIED_SCATTER_FILE_PATH[idx])
			my_file.close()
			idx += 1
			
		if(idx != 2):
			print("ERROR: SCATTER FILE COPY PROCESS FAILED")
			exit()
	return True


#create the project and setup the project directory
def setup_keil5_project_environment():
	"""
	Keil5 project environment creation and configuration API
	This API will create the entire the link between the SDK files
	and the user application
	"""
	global DLG_SDK_ROOT_DIRECTORY_TO_WRITE

	if(CLEAN_PROJ_ENV == False):
		if(IS_PROJ_ENV_IN_SDK == True):
			calc_proj_env_to_SDK_path()

		DLG_SDK_ROOT_DIRECTORY_TO_WRITE = DLG_SDK_ROOT_DIRECTORY

	update_scatter_file()
	build_uvprojx_element_various_controls(XML_PATTERN_VARIOUS_CONTROLS, XML_TAG[0])
	build_uvprojx_element_ldads_scatterfile(XML_PATTERN_LDADS_SCATTERFILE)
	build_uvprojx_element_ldads_misc(XML_PATTERN_LDADS_MISC, DLG_SPLIT_STR_PATTERN[1])
	build_uvprojx_element_file(XML_PATTERN_FILE, XML_TAG[3])
	#build_uvprojx_element_target_name(XML_PATTERN_TARGET_FILENAME)
	#build_uvprojx_element_output_name(XML_PATTERN_OUTPUT_FILENAME)
	build_uvoptx_element_debugopt(XML_PATTERN_TIFILE, XML_TAG[4])
	#build_uvoptx_element_targetname(XML_PATTERN_OVOPTX_TARGET_FILENAME)
	
	if(CLEAN_PROJ_ENV):
		print("PROJECT ENVIRONMENT HAS BEEN CLEANED SUCCESSFULLY ...")
	else:
		print(DLG_UVPROJX_NAME + " IS SUCCESSFULLY UPDATED WITH PROPER SDK PATH ...")
	return

#verify it is a dialog keil applicaiton project
def verify_dlg_keil_app_project(path):
	"""
	Verify there exist valid keil5 project for Dialog DA14531/DA14585/DA14586 application 
	software development.
	Multiple KEIL5 files in the same folder will generate error.
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
		print("ERROR 		:	SDK IS AN INVALID DIRECTORY PATH ...")
		print("RESOLUTION 	: 	VERIFY THE SDK DIRECTORY PATH ...")
		return False
	
	return True

#Verify project environment directory path.
def verify_dlg_sdk_proj_env_directory(path):
	"""
	Drive letter is not allowed to be project environment.
	Script checks in parent of 'path': a drive letter like C: doesn't have a parent.
	"""
	if(path.find(":\\")+2 >= len(path)):	# If no more characters after drive letter (f.e. "C:\\").
		return False
	else:
		return True

def handle_space_in_path(path):
	if path.find(" ") != -1:
		path_variable = ""
		path_list = list( filter(None,path.split("\\")) )
		path = ""
		i = 0 #dont forget to add up

		while (i < len(path_list)):
			path_to_dir = ""

			if path_list[i].find(" ") != -1:
				x = 0

				while (x < i): path_to_dir += path_list[x] + "\\"; x += 1

				os.chdir(path_to_dir)
				output = os.popen('dir /X').read().split('\n')

				for y in output:

					if y.find(path_list[i]) != -1: 
						y = y.replace(path_list[i],"")
						index = len(y)-1

						while index >= 0:
							if y[index] != " ":
								break   
							y = y[:-1]
							index -= 1

						index = len(y)-1

						while index > 0:
							if y[index] == " ":
								path_variable = path_variable[::-1]
								break
							path_variable += y[index]
							index -= 1
                        
						path_list[i] = path_variable
						break
        
			path += path_list[i] + "\\"
			i += 1
			
		os.chdir(DLG_WORKING_PROJECT_DIRECTORY)
	if(path.endswith("\\")):
		return path[:-1]
	else:
		return path


def run_application(sdk_path):
	"""
	Verify if the sdkpath is valid
	Verify if the KEIL5 uvprojx file is valid
	If both are verified then setup KEIL5 project environment
	"""
	global DLG_SDK_ROOT_DIRECTORY, DLG_WORKING_PROJECT_DIRECTORY, CLEAN_PROJ_ENV
	#os.system('cls') #works only in windows for linux it is 'clear'

	DLG_WORKING_PROJECT_DIRECTORY = str(os.getcwd())

	if (verify_dlg_sdk_proj_env_directory(DLG_WORKING_PROJECT_DIRECTORY) == False):
		print("SCRIPT DOES NOT SUPPORT PROJECT ENVIRONMENT TO BE DRIVE LETTER. \r\nPlease move the project environment into a subfolder of the drive.")
		exit()
	
	if (verify_dlg_keil_app_project(DLG_WORKING_PROJECT_DIRECTORY) == False):
		print("ABORTING.")
		exit()

	if(sdk_path == "clean"):
		CLEAN_PROJ_ENV = True
		print("CLEANING PROJECT ENVIRONMENT ...")
	else:
		DLG_SDK_ROOT_DIRECTORY = str(sdk_path)
		determine_proj_env_in_SDK()

		if (verify_dlg_sdk_root_directory(DLG_SDK_ROOT_DIRECTORY) == False):
			exit()

		print('\nSDK LOCATION :: ' + DLG_SDK_ROOT_DIRECTORY + ' IS FOUND ...')
		
		if(IS_PROJ_ENV_IN_SDK == False):
			DLG_SDK_ROOT_DIRECTORY = str(handle_space_in_path(sdk_path))

	setup_keil5_project_environment()
	

# start application
if __name__ == "__main__":
	# construct the argument parse and parse the arguments
	ap = argparse.ArgumentParser()
	ap.add_argument("-sdkpath", "--sdkpath", required=True ,help="FULL SDK PATH INSIDE DOUBLE QUOTATION OR \"CLEAN\" TO CLEAN PROJECT ENVIRONMENT")
	args = vars(ap.parse_args())

	# display a friendly message to the user
	#print("SDK location {},is the sdk path".format(args["sdkpath"]))	
	run_application(args["sdkpath"])
