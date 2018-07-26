#! /usr/bin/python
'''
	file name 	:: xxxxxxxxxxxxx.py
	author 		:: m alam
	version 	:: sw_v_0000.00003
	date		:: 2018.June.22
	python ver  :: 3.5.1
	description :: __________________________________
	reference   :: 
	disclaimer  :: This script is free to use, any modification can be done at your own risk. 
				   
	Instruction :: 
'''



import sys
import os.path
import glob
import re
import xml.etree.ElementTree as ET

DLG_SDK_ROOT_DIRECTORY = 'C:\\Dialog_workspace\\' # default just for example it will be filled up by sys.argv[1] input
DLG_WORKING_PROJECT_DIRECTORY = '.'
DLG_WORKING_PROJECT_NAME = 'test.uvprojx'
FILE_EXTENSION = 'uvprojx'	

SCATTER_FILE_PATH = 'sdk\\common_project_files\\scatterfiles\\scatterfile_common.sct'
SUB_STR_PATTERN_STACK_CONFIG = '.\\..\\..\\..\\..\\..\\sdk\\common_project_files\\'
DA1458X_STACK_CONFIG = 'sdk\\common_project_files\\'

COPIED_SCATTER_FILE_PATH = '.\\..\\src\\config\\copied_scatter.sct'

XML_TAG = ['IncludePath', 'Misc', 'ScatterFile', 'FilePath']
DLG_FIND_STR_PATTERN = ['\\sdk\\' , '\\third_party\\']
DLG_SPLIT_STR_PATTERN = [';' , ' ', '', '=']
DLG_FIND_OTHER_PATTERN = ['--symdefs']

XML_PATTERN_VARIOUS_CONTROLS = 'Targets/Target/TargetOption/TargetArmAds/Cads/VariousControls'
XML_PATTERN_LDADS = 'Targets/Target/TargetOption/TargetArmAds/LDads'
XML_PATTERN_FILE = 'Targets/Target/Groups/Group/Files/File'
XML_PATTERN_TARGET_FILENAME = 'Targets/Target/TargetName'
XML_PATTERN_OUTPUT_FILENAME = 'Targets/Target/TargetOption/TargetCommonOption/OutputName'

DLG_XML_ELEMENTS = [XML_PATTERN_VARIOUS_CONTROLS, XML_PATTERN_LDADS, XML_PATTERN_FILE, XML_PATTERN_TARGET_FILENAME, XML_PATTERN_OUTPUT_FILENAME]


def build_uvprojx_element_update_filename(xml_sub_element):
	tree = ET.parse(DLG_WORKING_PROJECT_NAME)
	root = tree.getroot()
	
	updated_project_name = DLG_WORKING_PROJECT_NAME[:-8]
	#print(updated_project_name)
	
	for t_sub_element in root.findall(xml_sub_element):
		#print(t_sub_element.tag + ' :: ' +  t_sub_element.text)		
		if(t_sub_element.tag == 'TargetName' or t_sub_element.tag == 'OutputName'):
			t_sub_element.text = updated_project_name
	
	my_file = open(DLG_WORKING_PROJECT_NAME,"w") 
	x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	my_file.write(x)
	my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	my_file.close()	


#build_uvprojx_element_file
def build_uvprojx_element_file(xml_sub_element, split_str_pattern):	
	tree = ET.parse(DLG_WORKING_PROJECT_NAME)
	root = tree.getroot()
	
	updated_data = ''	
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
					single_text = DLG_SDK_ROOT_DIRECTORY + DLG_FIND_STR_PATTERN[0] + my_t_list[-1]		
				else:
					my_t_list = temp_text.split(DLG_FIND_STR_PATTERN[1],1)
					single_text = temp_text
					#print(my_t_list)
					if(len(my_t_list)> 1):
						single_text = DLG_SDK_ROOT_DIRECTORY + DLG_FIND_STR_PATTERN[1] + my_t_list[-1]
					#print(my_t_list)
				#print(single_text)
					
				if (os.path.exists(single_text) == True):
					t_sub_element.find(temp_tag).text = single_text
				else:
					#print("WARNING :: IT IS AN INVALID DIRECTORY PATH, THIS PATH WILL BE AUTOMATICALLY REMOVED...")
					pass
				print(single_text)	
				
	
	my_file = open(DLG_WORKING_PROJECT_NAME,"w") 
	x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	my_file.write(x)
	my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	my_file.close()	
	
					
				
				

#build_uvprojx_element_ldads
def build_uvprojx_element_ldads(xml_sub_element, split_str_pattern):	
	tree = ET.parse(DLG_WORKING_PROJECT_NAME)
	root = tree.getroot()
	
	updated_data = ''
	updated_data_1 = ''
	path_exist_flag = False
	
	for t_sub_element in root.findall(xml_sub_element):
		#print(t_sub_element.tag)
		for temp_element in t_sub_element:
			temp_tag = temp_element.tag
			temp_text = t_sub_element.find(temp_tag).text			
			if(temp_tag == XML_TAG[2]):				
				#print(temp_tag , ' :: ' , temp_text)
				updated_data = COPIED_SCATTER_FILE_PATH
				if (os.path.exists(updated_data) == True):
					temp_text = t_sub_element.find(temp_tag).text = updated_data
				#print(temp_tag , ' :: ' , temp_text)
			elif(temp_tag == XML_TAG[1]):
				for single_text in temp_text.split(split_str_pattern):					
					#print(single_text)					
					my_t_list = single_text.split(DLG_FIND_STR_PATTERN[0],1)
					#print(my_t_list)
					#print(len(my_t_list))					
					
					if(len(my_t_list)> 1):
						single_text = DLG_SDK_ROOT_DIRECTORY + DLG_FIND_STR_PATTERN[0] + my_t_list[-1]											
						if (os.path.exists(single_text) == True):
							path_exist_flag = True
					
					temp_str = str(my_t_list[0])
					
					if(temp_str.find(DLG_FIND_OTHER_PATTERN[0]) != -1):						
						temp_str_list = temp_str.split(DLG_SPLIT_STR_PATTERN[3],1)
						#print(temp_str_list)
						updated_project_name = DLG_WORKING_PROJECT_NAME						
						updated_project_name = updated_project_name.replace(FILE_EXTENSION, 'txt' )
						index = updated_project_name.find('.')
						updated_project_name =  updated_project_name[:index] + '_symdef' + updated_project_name[index:]
						single_text = temp_str_list[0] + DLG_SPLIT_STR_PATTERN[3] + updated_project_name
							
					updated_data_1 = updated_data_1 + single_text + DLG_SPLIT_STR_PATTERN[1]	
					
				if(path_exist_flag):
					#print(updated_data_1)	
					single_text = updated_data_1
					pass
				else:
					#print("WARNING :: IT IS AN INVALID DIRECTORY PATH, THIS PATH WILL BE AUTOMATICALLY REMOVED...")
					pass	
				
				t_sub_element.find(temp_tag).text = single_text
				print(single_text)
					
	
	
	my_file = open(DLG_WORKING_PROJECT_NAME,"w") 
	x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	my_file.write(x)
	my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	my_file.close()	
				
		
				
	

#build_uvprojx_element_various_controls
def build_uvprojx_element_various_controls(xml_sub_element, xml_tag, split_str_pattern):	
	tree = ET.parse(DLG_WORKING_PROJECT_NAME)
	root = tree.getroot()

	updated_data = ''
	for t_sub_element in root.findall(xml_sub_element):
		#print(t_sub_element.tag)
		for temp_element in t_sub_element:
			temp_tag = temp_element.tag
			temp_text = t_sub_element.find(temp_tag).text
			if(temp_tag == xml_tag):				
				for single_text in temp_text.split(split_str_pattern):
					#print(single_text)														
					my_t_list = single_text.split(DLG_FIND_STR_PATTERN[0],1)
					#print(my_t_list)
					#print(len(my_t_list))
					if(len(my_t_list)> 1):
						single_text = DLG_SDK_ROOT_DIRECTORY + DLG_FIND_STR_PATTERN[0] + my_t_list[-1]
					else:
						my_t_list = single_text.split(DLG_FIND_STR_PATTERN[1],1)
						if(len(my_t_list)> 1):
							single_text = DLG_SDK_ROOT_DIRECTORY + DLG_FIND_STR_PATTERN[1] + my_t_list[-1]
					#print(single_text)
					if (os.path.isdir(single_text) == True):
						#print("IT IS A VALID PATH...")
						updated_data = updated_data + single_text + split_str_pattern
					else:
						#print("WARNING :: IT IS AN INVALID DIRECTORY PATH, THIS PATH WILL BE AUTOMATICALLY REMOVED...")
						pass

					t_sub_element.find(temp_tag).text = updated_data
					
	print(updated_data)
	
	#print(temp_text)
	#print(updated_data[:-1])
	#if(temp_text == updated_data[:-1]):
		#print("OK")
		
	my_file = open(DLG_WORKING_PROJECT_NAME,"w") 
	x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
	
	my_file.write(x)
	my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
	my_file.close()	

	

#create the project and setup the project directory
def setup_keil5_project_environment():
	global root, tree
	subString = SUB_STR_PATTERN_STACK_CONFIG
	temp_flag = False
	
	with open(DLG_SDK_ROOT_DIRECTORY + SCATTER_FILE_PATH) as my_file:
		newText = my_file.read().replace(subString, DLG_SDK_ROOT_DIRECTORY + DA1458X_STACK_CONFIG)
		#print(newText)	
	my_file.close()

	with open(COPIED_SCATTER_FILE_PATH, "w") as my_file:
		my_file.write(newText)
		print("DA1458X SCATTER FILE IS COPIED ...")
		print("     FROM LOCATION :: ", DLG_SDK_ROOT_DIRECTORY + SCATTER_FILE_PATH)
		print("     TO LOCATION :: ", COPIED_SCATTER_FILE_PATH)
		temp_flag = True
	my_file.close()

	
	if (temp_flag):
		build_uvprojx_element_various_controls(XML_PATTERN_VARIOUS_CONTROLS, XML_TAG[0], DLG_SPLIT_STR_PATTERN[0])	
		build_uvprojx_element_ldads(XML_PATTERN_LDADS, DLG_SPLIT_STR_PATTERN[1])
		build_uvprojx_element_file(XML_PATTERN_FILE, DLG_SPLIT_STR_PATTERN[2])
		#build_uvprojx_element_update_filename(XML_PATTERN_TARGET_FILENAME)
		#build_uvprojx_element_update_filename(XML_PATTERN_OUTPUT_FILENAME)
		print(DLG_WORKING_PROJECT_NAME + " SUCCESSFULLY UPDATED WITH PROPER SDK PATH ...")
	#return	
	
	

	
	

	

#verify it is a dialog keil applicaiton project
def verify_dlg_keil_app_project(path):	
	global DLG_WORKING_PROJECT_NAME
	file_extension = FILE_EXTENSION
	file_extension_counter = 0

	
	files = os.listdir(path)
	for name in files:
		if file_extension in name:
			DLG_WORKING_PROJECT_NAME = name
			file_extension_counter = file_extension_counter + 1
	
	if file_extension_counter == 1:		
		print('DA1458X KEIL PROJECT NAME :: ' + path + DLG_WORKING_PROJECT_NAME + ' IS A VALID ...')
	else:
		print("ERROR 		:	MULTIPLE FILES WITH ." + file_extension + " EXIST ...")
		print("RESOLUTION 	: 	ONLY ONE FILE WITH ." + file_extension + " IS EXPECTED INSIDE KEIL PROJECT FOLDER ...")
		return False

	return True

#verify dialog sdk root directory path
def verify_dlg_sdk_root_directory(path):		
	files = os.listdir(path)
	file_name_counter = 0
	for name in files:
		#print(name)
		if (name == 'sdk' or name == 'third_party'):
			file_name_counter = file_name_counter + 1
	if (file_name_counter != 2 ):
		print("ERROR 		:	DA1458X SDK IS AN INVALID DIRECTORY PATH ...")
		print("RESOLUTION 	: 	VERIFY THE DA1458X SDK DIRECTORY PATH ...")
		return False	
	return True



# check script configuration is set properly or not
# If configuration is set correctly then covert hex to bin, bin to img and finally create multipart binary
def run_application():
	global DLG_SDK_ROOT_DIRECTORY, DLG_WORKING_PROJECT_DIRECTORY
	#os.system('cls') #works only in windows for linux it is 'clear'		
	
	if(len(sys.argv) - 1 != 1):
		print("ERROR 		:	INPUT ERROR ...")
		print("RESOLUTION 	: 	INPUT ONLY FULL DIRECTORY PATH OF DA1458X SDK ...")
		exit()
	
	DLG_SDK_ROOT_DIRECTORY = str(sys.argv[1]) + '\\'	
	
	res = verify_dlg_sdk_root_directory(DLG_SDK_ROOT_DIRECTORY)	
	
	if (res == False):
		exit()

	print('\nDA1458X SDK LOCATION :: ' + DLG_SDK_ROOT_DIRECTORY + ' IS FOUND ...')
	
	DLG_WORKING_PROJECT_DIRECTORY = str(os.getcwd())
	DLG_WORKING_PROJECT_DIRECTORY = DLG_WORKING_PROJECT_DIRECTORY + '\\'	
	
	res = verify_dlg_keil_app_project(DLG_WORKING_PROJECT_DIRECTORY)
	
	if (res == False):
		exit()
	
	setup_keil5_project_environment()
	

# start application
if __name__ == "__main__":	
	run_application()



