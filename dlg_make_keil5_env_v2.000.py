#! /usr/bin/python
'''
###########################################################################################
# @file		:: dlg_make_keil5_env_v2.001.py
#
# @brief	:: Last modified: Nov 11 2021.
#			   
# 			   This script sets up the software development environment links with Dialog Semiconductor's SDK6 and copies the project
#			   folders to SDK location under projects_github.
#              This script is applied only on DA14585/86/31 published small examples in KEIL5.          
#			   The user needs DA14531/DA14585/DA14586 hardware to run any of these examples on.
#              
#              The user is requested to download the latest SDK6 from https://www.dialog-semiconductor.com/.
#			   Application development on the aforementioned Dialog Semiconductor SoCs are easy to maintain.
#              The recommendation is to create a project in the user application space.
#			   This way, the user hardly needs to touch any software code in the SDK6.
# 			   As a proof to this concept, small example application snippets are created to help users get started.
#
#			   Store and run this script in the same location as the KEIL5 *.uvprojx and *.uvoptx files.
#			   The script can be run from a command line on a windows OS (Power shell or DOS cmd).
#			   Example cmd:
#			   python dlg_make_keil5_env_v2.000.py -sdkpath "<user_specific_SDK6_location>"
#
#
#			   NOTE:
#			   - This script can create application environment only for SDK6.0.14.xxxx.
#			   - During the time of release the script was tested on SDK6.0.14.1114.
#			   - Supported commands:
#			   	python dlg_make_keil5_env_v2.000.py -sdkpath "<user_specific_SDK6_location>"
#			   
#			   After successful execution of the above command the Keil5 environment (example *.uvprojx) 
#			   files will be ready to run the KEIL5 IDE as an output. This command will also copy project files to SDK6
#			   folder projects_github.
#
#              OR,
#			   python dlg_make_keil5_env_v2.000.py -sdkpath "clean" 
#
#			   After successful execution of the above command the Keil5 environment (example *.uvprojx) 
#			   files will be unlinked from the SDK6.
#			    
#			   This "clean" command is shared to make sure no intermadiate file paths are shared with internal and external
#			   application users.
#				
#
# Copyright (c) 2020 Dialog Semiconductor. All rights reserved.
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
import os, shutil
import os.path
import glob
import re
import argparse
import xml.etree.ElementTree as ET
from typing import NamedTuple

'''
Classes
'''


class Soc_data(NamedTuple):
    ID: str
    sct_file_name: str
    sct_file_path: str
    copied_sct_file_name: str
    copied_sct_file_path: str


'''
Globals and constants
'''
DLG_SDK_ROOT_DIRECTORY = ".\\..\\..\\..\\..\\..\\"  # default just for example it will be filled up by sys.argv[1] input
DLG_SDK_ROOT_DIRECTORY_TO_WRITE = ".\\..\\..\\..\\..\\..\\"
DLG_WORKING_PROJECT_DIRECTORY = '.'
CLEAN_PROJ_ENV = False
IS_PROJ_ENV_IN_SDK = False
UVOPTX_FILE_EXTENSION = ".uvoptx"
UVPROJX_FILE_EXTENSION = ".uvprojx"
DLG_UVOPTX_NAME = "test" + UVOPTX_FILE_EXTENSION
DLG_UVPROJX_NAME = "test" + UVPROJX_FILE_EXTENSION
SOC_ID_LIST = ['585', '586', '531', "codeless_boot"]

DLG_DEFAULT_INCLUDE_PATHS =  ".\\..\\..\\..\\..\\..\\sdk\\app_modules\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack" \
                                  "\\controller\\em;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\controller\\llc" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\controller\\lld;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\controller\\llm;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\ea\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\em\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\hci" \
                                  "\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\hci\\src;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\host\\att;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\att\\attc" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\att\\attm;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\host\\att\\atts;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\gap" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\gap\\gapc;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\host\\gap\\gapm;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\gatt" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\gatt\\gattc;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\host\\gatt\\gattm;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\l2c\\l2cc" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\l2c\\l2cm;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\host\\smp;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\smp\\smpc" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\host\\smp\\smpm;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\anc" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\anc\\ancc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\anp;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\anp\\anpc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles" \
                                  "\\anp\\anps\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\bas\\basc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\bas\\bass\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\bcs;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\bcs\\bcsc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles" \
                                  "\\bcs\\bcss\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\blp" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\blp\\blpc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\blp\\blps\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\bms;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\bms\\bmsc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles" \
                                  "\\bms\\bmss\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\cpp" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\cpp\\cppc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\cpp\\cpps\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\cscp;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\cscp\\cscpc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack" \
                                  "\\profiles\\cscp\\cscps\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\cts" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\cts\\ctsc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\cts\\ctss\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\custom;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\custom\\custs\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack" \
                                  "\\profiles\\dis\\disc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\dis\\diss" \
                                  "\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\find;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\find\\findl\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack" \
                                  "\\profiles\\find\\findt\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\gatt" \
                                  "\\gatt_client\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\glp" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\glp\\glpc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\glp\\glps\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\hogp;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\hogp\\hogpbh\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack" \
                                  "\\profiles\\hogp\\hogpd\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\hogp" \
                                  "\\hogprh\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\hrp" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\hrp\\hrpc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\hrp\\hrps\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\htp;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\htp\\htpc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles" \
                                  "\\htp\\htpt\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\lan" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\lan\\lanc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\lan\\lans\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\pasp;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\pasp\\paspc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack" \
                                  "\\profiles\\pasp\\pasps\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\prox" \
                                  "\\proxm\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\prox\\proxr\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\rscp;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\rscp\\rscpc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack" \
                                  "\\profiles\\rscp\\rscps\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\scpp" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\scpp\\scppc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\scpp\\scpps\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\suota\\suotar\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\tip;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\tip\\tipc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles" \
                                  "\\tip\\tips\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\uds" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\uds\\udsc\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\uds\\udss\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles\\wss;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\profiles\\wss\\wssc\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\profiles" \
                                  "\\wss\\wsss\\api;.\\..\\..\\..\\..\\..\\sdk\\ble_stack\\rwble;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\ble_stack\\rwble_hl;.\\..\\..\\..\\..\\..\\sdk\\common_project_files" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\arch;.\\..\\..\\..\\..\\..\\sdk\\platform\\arch" \
                                  "\\boot;.\\..\\..\\..\\..\\..\\sdk\\platform\\arch\\boot\\ARM;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\platform\\arch\\boot\\GCC;.\\..\\..\\..\\..\\..\\sdk\\platform\\arch\\compiler" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\arch\\compiler\\ARM;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\platform\\arch\\compiler\\GCC;.\\..\\..\\..\\..\\..\\sdk\\platform\\arch\\ll" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\arch\\main;.\\..\\..\\..\\..\\..\\sdk\\platform" \
                                  "\\core_modules\\arch_console;.\\..\\..\\..\\..\\..\\sdk\\platform\\core_modules" \
                                  "\\common\\api;.\\..\\..\\..\\..\\..\\sdk\\platform\\core_modules\\crypto" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\core_modules\\dbg\\api;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\platform\\core_modules\\gtl\\api;.\\..\\..\\..\\..\\..\\sdk\\platform\\core_modules" \
                                  "\\gtl\\src;.\\..\\..\\..\\..\\..\\sdk\\platform\\core_modules\\h4tl\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\core_modules\\ke\\api;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\platform\\core_modules\\ke\\src;.\\..\\..\\..\\..\\..\\sdk\\platform\\core_modules" \
                                  "\\nvds\\api;.\\..\\..\\..\\..\\..\\sdk\\platform\\core_modules\\rf\\api" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\core_modules\\rwip\\api;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\platform\\driver\\adc;.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\battery" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\ble;.\\..\\..\\..\\..\\..\\sdk\\platform" \
                                  "\\driver\\dma;.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\gpio" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\hw_otpc;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\platform\\driver\\i2c;.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\i2c_eeprom" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\pdm;.\\..\\..\\..\\..\\..\\sdk\\platform" \
                                  "\\driver\\reg;.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\rtc;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\platform\\driver\\spi;.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\spi_flash" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\spi_hci;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\platform\\driver\\syscntl;.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\systick" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\timer;.\\..\\..\\..\\..\\..\\sdk\\platform" \
                                  "\\driver\\trng;.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\uart" \
                                  ";.\\..\\..\\..\\..\\..\\sdk\\platform\\driver\\wkupct_quadec;.\\..\\..\\..\\..\\..\\sdk" \
                                  "\\platform\\include;.\\..\\..\\..\\..\\..\\sdk\\platform\\system_library\\include" \
                                  ";.\\..\\..\\..\\..\\..\\third_party\\hash;.\\..\\..\\..\\..\\..\\third_party\\rand;.\\..\\..\\.." \
                                  "\\..\\..\\third_party\irng;.\\..\\src;.\\..\\src\\config;.\\..\\src\\custom_profile;" \
                                  ".\\..\\..\\..\\..\\..\\sdk"  \
                                  "\\platform\\utilities\\otp_hdr;.\\..\\..\\..\\..\\..\\sdk\\platform\\utilities\\otp_cs;" \
                                  ".\\..\\..\\..\\..\\..\\sdk\\platform\\include\\CMSIS\\5.6.0\\Include; .\\..\\..\\..\\..\\.." \
                                  "\\sdk\\common_project_files\\scatterfiles\\; .\\..\\src\\user_app; .\\..\\src\\user_config;" \
                                  ".\\..\\src\\user_drivers; .\\..\\src\\platform; .\\..\\..\\..\\..\\..\\sdk\\platform\\" \
                                  "driver\\wifi; .\\..\\src\\user_platform; .\\..\\src\\user_drivers\\bmi270; .\\..\\src" \
                                  "\\modules_lib\\app\\wkup_keys; .\\..\\src\\modules_lib\\port; .\\..\\src\\user_drivers\\" \
                                  "platform\\bus;..\\src\\user_drivers\\platform\\console;..\\src\\user_drivers\\sensor\\accel" \
                                  "\\mc36xx;..\\src\\user_drivers\\sensor\\accel"

SCATTER_FILE_NAME = ["DA14585_586.sct", "DA14531.sct"]
SCATTER_FILE_PATH = [('\\sdk\\common_project_files\\scatterfiles\\' + SCATTER_FILE_NAME[0]),
                     ('\\sdk\\common_project_files\\scatterfiles\\' + SCATTER_FILE_NAME[1])]
COPIED_SCATTER_FILE_NAME = ["copied_scatter_585_586.sct", "copied_scatter_531.sct"]
COPIED_SCATTER_FILE_PATH = [('.\\..\\src\\config\\' + COPIED_SCATTER_FILE_NAME[0]),
                            ('.\\..\\src\\config\\' + COPIED_SCATTER_FILE_NAME[1])]

DA1458x = Soc_data("DA1458x", SCATTER_FILE_NAME[0], SCATTER_FILE_PATH[0], COPIED_SCATTER_FILE_NAME[0],
                   COPIED_SCATTER_FILE_PATH[0])
DA14531 = Soc_data("DA14531", SCATTER_FILE_NAME[1], SCATTER_FILE_PATH[1], COPIED_SCATTER_FILE_NAME[1],
                   COPIED_SCATTER_FILE_PATH[1])
SOC_DATA_LIST = [DA1458x, DA14531]
SOC_LIST = [(SOC_ID_LIST[0], SOC_DATA_LIST[0]), (SOC_ID_LIST[1], SOC_DATA_LIST[0]), (SOC_ID_LIST[2], SOC_DATA_LIST[1])]

TARGET_SOCS = []  # List of IDs of SoCs encountered in project targets.
UNIQUE_SOCS = []  # List of IDs of unique SoCs encountered in project targets.

SHARED_FOLDER_PATH = '\\projects\\target_apps\\peripheral_examples\\shared\\'
SUB_STR_PATTERN_STACK_CONFIG = '.\\..\\..\\..\\..\\..\\sdk\\common_project_files\\'
DA1458X_STACK_CONFIG = '\\sdk\\common_project_files\\'
SDK_PERIPH_EX_SCATTER_FILE_PATH = ""

XML_TAG = ['IncludePath', 'Misc', 'ScatterFile', 'FilePath', 'tIfile', 'PathWithFileName']
DLG_FIND_STR_PATTERN = ['\\sdk\\', '\\third_party\\', '\\shared\\']
DLG_SPLIT_STR_PATTERN = [';', ' ', '', '=']
DLG_FIND_OTHER_PATTERN = ['--symdefs']  # Currently unused.

XML_PATTERN_TARGET = 'Targets/Target'
XML_PATTERN_VARIOUS_CONTROLS = XML_PATTERN_TARGET + '/TargetOption/TargetArmAds/Cads/VariousControls'
XML_PATTERN_LDADS = XML_PATTERN_TARGET + '/TargetOption/TargetArmAds/LDads'

XML_PATTERN_LDADS_SCATTERFILE = XML_PATTERN_LDADS + '/ScatterFile'
XML_PATTERN_LDADS_MISC = XML_PATTERN_LDADS + '/Misc'
XML_PATTERN_FILE = XML_PATTERN_TARGET + '/Groups/Group/Files/File'
XML_PATTERN_TARGET_FILENAME = XML_PATTERN_TARGET + '/TargetName'
XML_PATTERN_OUTPUT_FILENAME = XML_PATTERN_TARGET + '/TargetOption/TargetCommonOption/OutputName'

XML_PATTERN_TIFILE = 'Target/TargetOption/DebugOpt/tIfile'
XML_PATTERN_PATHWITHFILENAME = 'Group/File/PathWithFileName'
XML_PATTERN_OVOPTX_TARGET_FILENAME = 'Target/TargetName'

'''
Functions
'''


def split_path(path, compare_string):
    """
    Returns type: (boolean,string). 
    Function searches for "compare_string" in "path" with the highest index.
    if "compare_string" is found in "path" split_path will remove everything in front of it and return (True,Remaining string).
    For example "path" = 6.0.12.1020/sdk/platform and "compare_string" = /sdk/, (True,"/sdk/platform") will be returned 
    If "compare_string" was not found in "path" split_path will return (False,path) 
    """

    index = path.rfind(compare_string)

    if index == -1:
        return (False, path)

    return (True, path.replace(path[:index], ""))


def write_xml_file(xml_tree, xml_filename):
    '''
    Write the given ElementTree tree to an xml file.
    '''
    if not isinstance(xml_tree, ET.ElementTree):
        print('ERROR: XML TREE GIVEN IS INVALID')
        exit()

    # decl = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n'''
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
    # print(dlg_uvoptx_file)

    tree = ET.parse(dlg_uvoptx_file)
    root = tree.getroot()

    dlg_uvoptx_target_file = temp_list[0] + '_'

    t_element_counter = 0

    for t_sub_element in root.findall(xml_sub_element):
        # print(t_sub_element.text)
        t_sub_element.text = dlg_uvoptx_target_file + SOC_ID_LIST[t_element_counter]
        t_element_counter = t_element_counter + 1
    # print('TARGET PROJECT NAME IN OVOPTX FILE:: ' + t_sub_element.text)

    tree.write(dlg_uvoptx_file)

    x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
    with open(dlg_uvoptx_file, 'r+') as file_pointer:
        file_data = file_pointer.read()
        file_pointer.seek(0, 0)
        file_pointer.write(x.rstrip('\r\n') + '\n' + file_data)
    file_pointer.close()


def build_uvoptx_change_element_adresses(xml_sub_element, xml_tag):
    '''
    Update file paths in .uvoptx file.
    '''
    # print(DLG_UVOPTX_NAME)
    tree = ET.parse(DLG_UVOPTX_NAME)
    root = tree.getroot()

    for t_sub_element in root.findall(xml_sub_element):  # find all elements of a specific tag
        temp_text = t_sub_element.text
        if t_sub_element.tag == xml_tag and t_sub_element.text != None:
            (found_sdk, my_t_list_sdk) = split_path(temp_text, DLG_FIND_STR_PATTERN[0])
            (found_shared, my_t_list_shared) = split_path(temp_text, DLG_FIND_STR_PATTERN[2])
            (found_third_party, my_t_list_third_party) = split_path(temp_text, DLG_FIND_STR_PATTERN[1])

            if found_sdk and found_shared:  # If user named a folder in path sdk or shared
                if len(my_t_list_sdk) < len(my_t_list_shared):
                    found_shared = False
                else:
                    found_sdk = False

            if (found_sdk):
                single_text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + my_t_list_sdk
                # print(single_text)
                t_sub_element.text = single_text
            elif (found_shared):
                single_text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + SHARED_FOLDER_PATH + my_t_list_shared.replace(
                    DLG_FIND_STR_PATTERN[2], "")
                # print(single_text)
                t_sub_element.text = single_text
            elif (found_third_party):
                single_text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + my_t_list_third_party
                t_sub_element.text = single_text
    # tree.write(DLG_UVOPTX_NAME)

    # x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
    # with open(dlg_uvoptx_file, 'r+') as file_pointer:
    # file_data = file_pointer.read()
    # file_pointer.seek(0, 0)
    # file_pointer.write(x.rstrip('\r\n') + '\n' + file_data)
    # file_pointer.close()
    write_xml_file(tree, DLG_UVOPTX_NAME)


# print("INI FILE PATH IS UPDATED ...")


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
        # print(t_sub_element.text)
        t_sub_element.text = target_name + SOC_ID_LIST[t_element_counter]
        t_element_counter = t_element_counter + 1
    # print('EXECUTABLE FILE NAME :: ' + t_sub_element.text)

    tree.write(DLG_UVPROJX_NAME)

    x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''
    with open(DLG_UVPROJX_NAME, 'r+') as file_pointer:
        file_data = file_pointer.read()
        file_pointer.seek(0, 0)
        file_pointer.write(x.rstrip('\r\n') + '\n' + file_data)
    file_pointer.close()


def build_uvprojx_element_target_name(xml_sub_element):
    '''
    Update the target name in uvprojx file
    '''
    temp_text = +DLG_UVPROJX_NAME
    temp_list = temp_text.split('.')

    target_name = temp_list[0] + '_'

    tree = ET.parse(DLG_UVPROJX_NAME)
    root = tree.getroot()
    t_element_counter = 0

    for t_sub_element in root.findall(xml_sub_element):
        # print(t_sub_element.text)
        t_sub_element.text = target_name + SOC_ID_LIST[t_element_counter]
        t_element_counter = t_element_counter + 1
    # print('TARGET PROJECT NAME :: ' + t_sub_element.text)

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

    print("UPDATE DIRECTORY PATHS OF ALL FILES ...")
    for t_sub_element in root.findall(xml_sub_element):
        # print(t_sub_element.tag)
        for temp_element in t_sub_element:
            temp_tag = temp_element.tag
            temp_text = t_sub_element.find(temp_tag).text
            if (temp_tag == xml_tag):
                # print(temp_tag , ' :: ' , temp_text)

                (found_sdk, sdk_string) = split_path(temp_text, DLG_FIND_STR_PATTERN[0])
                (found_third_party, third_party_string) = split_path(temp_text, DLG_FIND_STR_PATTERN[1])
                (found_shared, shared_string) = split_path(temp_text, DLG_FIND_STR_PATTERN[2])

                if (found_sdk + found_shared + found_third_party) == 0:
                    # WARNING no sdk path
                    continue

                end_of_path = min([sdk_string, third_party_string, shared_string], key=len)
                single_text = ""

                if end_of_path.find(DLG_FIND_STR_PATTERN[2]) != -1:
                    single_text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + SHARED_FOLDER_PATH + end_of_path.replace(
                        DLG_FIND_STR_PATTERN[2], "")
                else:
                    single_text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + end_of_path

                if ((os.path.exists(single_text) == True) or (CLEAN_PROJ_ENV == True)):
                    t_sub_element.find(temp_tag).text = single_text
                else:
                    # print("WARNING :: IT IS AN INVALID DIRECTORY PATH, THIS PATH WILL BE AUTOMATICALLY REMOVED...")
                    pass
                print(single_text)
    print("")

    # my_file = open(DLG_UVPROJX_NAME,"w")
    # x = '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n'''

    # my_file.write(x)
    # my_file.write((ET.tostring(root, encoding='UTF-8').decode('utf8')))
    # my_file.close()
    write_xml_file(tree, DLG_UVPROJX_NAME)


def soc_id_to_soc_data(soc_id):
    """
    Using input SOC_ID loop through SOC_LIST to find data for this SoC needed in this script.
    Input: soc_id. For example "531".
    Return: Class 'struct' containing data for SoC needed in this script. Return -1 if soc_id not in SOC_LIST.
    """
    for i in range(0, len(SOC_LIST), 1):
        if (soc_id in SOC_LIST[i][0]):
            return SOC_LIST[i][1]

    return -1


def build_uvprojx_element_ldads_scatterfile(xml_sub_element):
    """
    Modify and update UVPROJX ldads scatterfile element
    """
    loop_idx = 0

    tree = ET.parse(DLG_UVPROJX_NAME)
    root = tree.getroot()

    target_names_dict = root.findall('Targets/Target/TargetName')
    target_names = []
    for target_name in target_names_dict:
        target_names.append(target_name.text)

    for index, t_sub_element in enumerate(root.findall(xml_sub_element)):
        print("I am here 0")
        if (CLEAN_PROJ_ENV == True):
            if (t_sub_element.text.endswith("peripheral_examples.sct")):  # .sct file in SDK used.
                t_sub_element.text = ".\\..\\..\\..\\..\\..\\projects\\target_apps\peripheral_examples\shared\peripheral_examples.sct"
            else:
                if 'DA14531' in target_names[index].upper():
                    t_sub_element.text = ".\\..\\..\\..\\..\\..\\sdk\\common_project_files\\scatterfiles\\DA14531.sct"
                else:
                    t_sub_element.text = ".\\..\\..\\..\\..\\..\\sdk\\common_project_files\\scatterfiles\\DA14585_586.sct"
            #else:  # .sct file copied from SDk.
            #    t_sub_element.text = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + "\\" + soc_id_to_soc_data(
            #        TARGET_SOCS[loop_idx]).copied_sct_file_name
        elif (os.path.exists(str(SDK_PERIPH_EX_SCATTER_FILE_PATH)) == True):
            t_sub_element.text = str(SDK_PERIPH_EX_SCATTER_FILE_PATH)
        elif (os.path.exists(str(soc_id_to_soc_data(TARGET_SOCS[loop_idx]).copied_sct_file_path))):
            t_sub_element.text = (str(soc_id_to_soc_data(TARGET_SOCS[loop_idx]).copied_sct_file_path))

        loop_idx += 1

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

    print("UPDATING LDad Misc ELEMENT ...")
    for t_sub_element in root.findall(xml_sub_element):
        if (t_sub_element.text == None):
            return
        # print(t_sub_element.tag)
        print("CURRENT LDad Misc: " + t_sub_element.text)
        misc_element = t_sub_element.text

        (found_sdk, divided_misc) = split_path(misc_element, DLG_FIND_STR_PATTERN[0])

        if (found_sdk) == 0:
            # WARNING no sdk path
            pass  # continue

        feedback_and_path_begin = misc_element.replace(divided_misc, "")
        # print("ldads_misc: Feedback + previous user path = " + feedback_and_path_begin)

        (found_drive, drive_path) = split_path(feedback_and_path_begin, check_strs[0])
        (found_relative, relative_path) = split_path(feedback_and_path_begin, check_strs[1])
        (found_dot, dot_path) = split_path(feedback_and_path_begin, check_strs[2])

        if (found_drive + found_relative + found_dot) == 0:
            # error
            pass  # continue

        if found_relative:
            # print("ldads_misc: Relative path = " + relative_path)
            (found_relative, relative_path) = split_path(feedback_and_path_begin, check_strs[3])
            feedback = feedback_and_path_begin.replace(relative_path, "") + " "
        elif found_drive:
            feedback = feedback_and_path_begin.replace(drive_path, "")
            # print("ldads_misc: Drive_path = " + drive_path)
            feedback = feedback[:-1]
        else:
            # print("ldads_misc: Dot path = " + dot_path)
            feedback = feedback_and_path_begin.replace(dot_path, "")

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
        print("NEW LDad Misc    : " + t_sub_element.text)
    print("")
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

    if (CLEAN_PROJ_ENV == True):
        updated_data = DLG_DEFAULT_INCLUDE_PATHS
    else:
        DLG_WORKING_PROJECT_PARENT_DIRECTORY = "..\\"
        DLG_SDK_SUBFOLDER_DIRECTORY = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + DLG_FIND_STR_PATTERN[0][:-1]
        DLG_THIRD_PARTY_SUBFOLDER_DIRECTORY = DLG_SDK_ROOT_DIRECTORY_TO_WRITE + DLG_FIND_STR_PATTERN[1][:-1]
        DLG_PROJECT_SOURCE_SUBFOLDER_DIRECTORY = os.path.join(DLG_WORKING_PROJECT_PARENT_DIRECTORY, 'src')

        end_ele_char = ";"
        updated_data = ""

        for dirpath, dirname, filename in os.walk(DLG_PROJECT_SOURCE_SUBFOLDER_DIRECTORY):
            # if x = os.path.basename(dirpath)
            updated_data = updated_data + dirpath + end_ele_char
        for dirpath, dirname, filename in os.walk(DLG_SDK_SUBFOLDER_DIRECTORY):
            # if x = os.path.basename(dirpath)
            updated_data = updated_data + dirpath + end_ele_char
        for dirpath, dirname, filename in os.walk(DLG_THIRD_PARTY_SUBFOLDER_DIRECTORY):
            # if x = os.path.basename(dirpath)
            updated_data = updated_data + dirpath + end_ele_char

    for t_sub_element in root.findall(xml_sub_element):
        # print(t_sub_element.tag)
        for temp_element in t_sub_element:
            temp_tag = temp_element.tag
            if (temp_tag == xml_tag):
                t_sub_element.find(temp_tag).text = updated_data

    # Print updated_data in a readable format.
    updated_data = updated_data.replace(";", ";\r\n")
    print("IncludePath ELEMENT TEXT:\r\n" + updated_data + "END OF IncludePath ELEMENT TEXT.\r\n")

    # print(temp_text)
    # print(updated_data[:-1])
    # if(temp_text == updated_data[:-1]):
    # print("OK")

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

    if relative_path == "":  # If project environment is same as SDK root.
        relative_path = ".\\"

    print(
        "USING RELATIVE PATH \"" + relative_path + "\" INSTEAD OF ABSOLUTE \"" + DLG_SDK_ROOT_DIRECTORY + "\" AS SDK ROOT ...")
    DLG_SDK_ROOT_DIRECTORY = relative_path[:-1]


def determine_proj_env_in_SDK():
    """
    set IS_PROJ_ENV_IN_SDK to True if project environment is located inside of SDK.
    """
    global IS_PROJ_ENV_IN_SDK

    if (DLG_WORKING_PROJECT_DIRECTORY.startswith(os.path.abspath(DLG_SDK_ROOT_DIRECTORY))):
        print("PROJECT ENVIRONMENT LOCATED INSIDE OF SDK ...")
        IS_PROJ_ENV_IN_SDK = True
    else:
        # print("PROJECT ENVIRONMENT LOCATED OUTSIDE OF SDK ...")
        pass


def update_soc_lists():
    """
    Loop through each <Target> in .uvprojx .xml project file.
    For each <Target>:
    1) Add SoC to list.
    2) Add SoC unique SoC list, if SoC is not already in list.
    """
    global TARGET_SOCS, UNIQUE_SOCS

    tree = ET.parse(DLG_UVPROJX_NAME)
    root = tree.getroot()

    for t_sub_element in root.findall(XML_PATTERN_TARGET_FILENAME):
        for i in range(0, len(SOC_LIST), 1):
            if (SOC_LIST[i][0] in t_sub_element.text):
                TARGET_SOCS.append(SOC_LIST[i][0])
                if (SOC_LIST[i][0] not in UNIQUE_SOCS):
                    UNIQUE_SOCS.append(SOC_LIST[i][0])
                break

            if (i == len(SOC_ID_LIST) - 1):  # If no suitable SoC targets found.
                sys.stdout.write(
                    "ERROR: Target name \"" + t_sub_element.text + "\" does not include name of supported SoC. Please include one of the following in your target name:")

                for j in range(0, len(SOC_ID_LIST), 1):
                    sys.stdout.write(" " + SOC_ID_LIST[j])
                    if (j < len(SOC_ID_LIST) - 1):
                        sys.stdout.write(",")
                    elif (j == len(SOC_ID_LIST) - 1):
                        sys.stdout.write(".\r\n")
                exit()


def update_scatter_file(xml_sub_element):
    """
    If scatter file(s) present in project environment, copy scatter file(s) data from SDK to proj env.
    If scatter file(s) not present in proj env, prepare path to scatter file(s) in SDK.

    Exit if scatter file(s) copy process unsuccessful.
    Return True if successful.
    """
    global SDK_PERIPH_EX_SCATTER_FILE_PATH
    global DLG_SDK_ROOT_DIRECTORY
    update_soc_lists()

    tree = ET.parse(DLG_UVPROJX_NAME)
    root = tree.getroot()

    for t_sub_element in root.findall(xml_sub_element):
        if (t_sub_element.text.endswith("peripheral_examples.sct")):
            SDK_PERIPH_EX_SCATTER_FILE_PATH = str(
                DLG_SDK_ROOT_DIRECTORY + SHARED_FOLDER_PATH + "peripheral_examples.sct")
            return True
        break

    # Scatter file path not to .sct in SDK. Copy .sct file to project environment.
    if (os.path.isdir(".\\..\\src\\config\\") == True):
        # Update/Create scatter file(s) in proj env with data from SDK scatter file(s).
        if (CLEAN_PROJ_ENV == True):
            print(f"CLEANING SCATTER FILES ...")
            for i in range(0, len(SOC_LIST), 1):
                if (os.path.exists(SOC_LIST[i][1].copied_sct_file_path) == True):
                    os.remove(SOC_LIST[i][1].copied_sct_file_path)
                    print(f"SCATTER FILE {SOC_LIST[i][1].copied_sct_file_path} HAS BEEN CLEANED.")
            print("")
        else:
            for i in range(0, len(UNIQUE_SOCS), 1):
                cur_scatter_file_path = DLG_SDK_ROOT_DIRECTORY + soc_id_to_soc_data(UNIQUE_SOCS[i]).sct_file_path
                cur_copied_sct_file_path = soc_id_to_soc_data(UNIQUE_SOCS[i]).copied_sct_file_path
                new_text = ""

                with open(cur_scatter_file_path) as my_file:
                    new_text = my_file.read().replace(SUB_STR_PATTERN_STACK_CONFIG,
                                                      DLG_SDK_ROOT_DIRECTORY_TO_WRITE + DA1458X_STACK_CONFIG, 1)
                # print('NewText string : ' + new_text)
                my_file.close()

                with open(cur_copied_sct_file_path, "w") as my_file:
                    my_file.write(new_text)
                    print("SCATTER FILE IS COPIED ...")
                    print("     FROM LOCATION :: ", cur_scatter_file_path)
                    print("     TO LOCATION   :: ", cur_copied_sct_file_path)
                my_file.close()

            print(f"SCATTER FILE COPY PROCESS SUCCEEDED FOR {len(UNIQUE_SOCS)} TARGET(S).\r\n")

        return True

    return False


# create the project and setup the project directory
def setup_keil5_project_environment(sdk_path):
    """
    Keil5 project environment creation and configuration API
    This API will create the entire the link between the SDK files
    and the user application
    """
    global DLG_SDK_ROOT_DIRECTORY_TO_WRITE, DLG_SDK_ROOT_DIRECTORY


    if (CLEAN_PROJ_ENV == False):
        if (IS_PROJ_ENV_IN_SDK == True):
            calc_proj_env_to_SDK_path()
        DLG_SDK_ROOT_DIRECTORY_TO_WRITE = sdk_path
        DLG_SDK_ROOT_DIRECTORY = sdk_path
    else:
        pass

    update_scatter_file(XML_PATTERN_LDADS_SCATTERFILE)
    build_uvprojx_element_various_controls(XML_PATTERN_VARIOUS_CONTROLS, XML_TAG[0])
    build_uvprojx_element_ldads_scatterfile(XML_PATTERN_LDADS_SCATTERFILE)
    build_uvprojx_element_ldads_misc(XML_PATTERN_LDADS_MISC, DLG_SPLIT_STR_PATTERN[1])
    build_uvprojx_element_file(XML_PATTERN_FILE, XML_TAG[3])
    # build_uvprojx_element_target_name(XML_PATTERN_TARGET_FILENAME)
    # build_uvprojx_element_output_name(XML_PATTERN_OUTPUT_FILENAME)
    # build_uvoptx_element_debugopt(XML_PATTERN_TIFILE, XML_TAG[4])
    build_uvoptx_change_element_adresses(XML_PATTERN_PATHWITHFILENAME, XML_TAG[5])
    build_uvoptx_change_element_adresses(XML_PATTERN_TIFILE, XML_TAG[4])
    # build_uvoptx_element_targetname(XML_PATTERN_OVOPTX_TARGET_FILENAME)

    if (CLEAN_PROJ_ENV):
        print("PROJECT ENVIRONMENT HAS BEEN CLEANED SUCCESSFULLY ...")
    else:
        print(DLG_UVPROJX_NAME + " IS SUCCESSFULLY UPDATED WITH PROPER SDK PATH ...")
    return


# verify it is a dialog keil application project
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
    if (DLG_UVPROJX_NAME.replace(UVPROJX_FILE_EXTENSION, "") != DLG_UVOPTX_NAME.replace(UVOPTX_FILE_EXTENSION, "")):
        print(
            "ERROR		:	FILE NAME OF .UVOPTX AND .UVPROJX FILES ARE NOT IDENTICAL.\n" + DLG_UVOPTX_NAME + "\n" + DLG_UVPROJX_NAME)
        return False

    if uvprojx_file_extension_counter == 1:
        print('KEIL PROJECT NAME :: ' + path + "\\" + DLG_UVPROJX_NAME + ' IS A VALID PROJECT DIRECTORY...\r\n')
    elif uvprojx_file_extension_counter > 1:
        print("ERROR		:	MULTIPLE FILES WITH " + UVPROJX_FILE_EXTENSION + " EXIST ...")
        print(
            "RESOLUTION	:	ONLY ONE FILE WITH " + UVPROJX_FILE_EXTENSION + " IS EXPECTED INSIDE KEIL PROJECT FOLDER ...")
        return False
    else:
        print("ERROR		:	NO FILES WITH " + UVPROJX_FILE_EXTENSION + " EXIST ...")
        print("RESOLUTION 	:	THIS SCRIPT SHOULD BE EXECUTED INSIDE A PROJECT DIRECTORY ...")
        return False
    return True


# verify dialog sdk root directory path
def verify_dlg_sdk_root_directory(path):
    """
    Verify that the dialog SDK path is having proper folder path
    """
    files = os.listdir(path)
    file_name_counter = 0

    for name in files:
        # print(name)
        if (name == 'sdk' or name == 'third_party'):
            file_name_counter += 1

    if (file_name_counter != 2):
        print("ERROR 		:	SDK IS AN INVALID DIRECTORY PATH ...")
        print("RESOLUTION 	: 	VERIFY THE SDK DIRECTORY PATH ...")
        return False

    return True


# Verify project environment directory path.
def verify_dlg_sdk_proj_env_directory(path):
    """
    Drive letter is not allowed to be project environment.
    Script checks in parent of 'path': a drive letter like C: doesn't have a parent.
    """
    if (path.find(":\\") + 2 >= len(path)):  # If no more characters after drive letter (f.e. "C:\\").
        return False
    else:
        return True


def handle_space_in_path(path):
    """
    For every directory in given path with a space in the string, this function will replace the
    string of that directory with the DOS short path of this directory.
    If directory path does not contain a space this function will return path without any changes.
    """
    if path.find(" ") != -1:
        path_variable = ""
        path_list = list(filter(None, path.split("\\")))
        path = ""
        i = 0  # dont forget to add up

        while (i < len(path_list)):
            path_to_dir = ""

            if path_list[i].find(" ") != -1:
                x = 0

                while (x < i): path_to_dir += path_list[x] + "\\"; x += 1

                os.chdir(path_to_dir)
                output = os.popen('dir /X').read().split('\n')

                for y in output:

                    if y.find(path_list[i]) != -1:
                        y = y.replace(path_list[i], "")
                        index = len(y) - 1

                        while index >= 0:
                            if y[index] != " ":
                                break
                            y = y[:-1]
                            index -= 1

                        index = len(y) - 1

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
    if (path.endswith("\\")):
        return path[:-1]
    else:
        return path


def run_individual_project_file(working_dir_path, sdk_path):
    """
    Verify if the sdkpath is valid
    Verify if the KEIL5 uvprojx file is valid
    If both are verified then setup KEIL5 project environment
    """
    global DLG_SDK_ROOT_DIRECTORY, DLG_WORKING_PROJECT_DIRECTORY, CLEAN_PROJ_ENV
    # os.system('cls') #works only in windows for linux it is 'clear'

    os.chdir(working_dir_path)

    DLG_WORKING_PROJECT_DIRECTORY = working_dir_path

    if (verify_dlg_sdk_proj_env_directory(DLG_WORKING_PROJECT_DIRECTORY) == False):
        print(
            "SCRIPT DOES NOT SUPPORT PROJECT ENVIRONMENT TO BE DRIVE LETTER. \r\nPlease move the project environment into a subfolder of the drive.")
        exit()

    if (verify_dlg_keil_app_project(DLG_WORKING_PROJECT_DIRECTORY) == False):
        print("ABORTING.")
        exit()

    if (sdk_path == "clean"):
        CLEAN_PROJ_ENV = True
        print("CLEANING PROJECT ENVIRONMENT ...\r\n")
    else:
        DLG_SDK_ROOT_DIRECTORY = str(sdk_path)
        determine_proj_env_in_SDK()

        if (verify_dlg_sdk_root_directory(DLG_SDK_ROOT_DIRECTORY) == False):
            exit()

        print('SDK LOCATION :: ' + DLG_SDK_ROOT_DIRECTORY + ' IS FOUND ...\r\n')

        if (IS_PROJ_ENV_IN_SDK == False):
            DLG_SDK_ROOT_DIRECTORY = str(handle_space_in_path(sdk_path))

    setup_keil5_project_environment(sdk_path)


def copytree(src, dst, symlinks=False, ignore=None):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            shutil.copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)


def copy_projects_to_sdk(sdkpath, proj_dir_names):
    dir_names = []

    for proj_dir_name in proj_dir_names:
        split_address = proj_dir_name.split("\\")
        dir_name = ''
        for item in split_address[:-2]:
            dir_name = dir_name + "\\" + item
        dir_name = dir_name[1:]
        dir_names.append(dir_name)

    dir_names = list(set(dir_names))

    dst_path = sdkpath + "\\projects_github\\"

    if not os.path.isdir(dst_path):
        os.mkdir(dst_path)

    for dir_name in dir_names:
        dst_path_copy = dst_path + dir_name.split("\\")[-1]
        if not os.path.isdir(dst_path_copy):
            os.mkdir(dst_path_copy)
        copytree(dir_name, dst_path_copy)


def link_projects_to_sdk(sdkpath, proj_dir_names):
    for proj_dir_name in proj_dir_names:
        try:
            run_individual_project_file(proj_dir_name, sdkpath)
        except Exception as e:
            print(e)


# print(dir_names)

def run_all_project_files(pathname, sdkpath):
    uvoptx_pathnames = glob.glob(pathname + "/**/**/*.uvoptx", recursive=True)
    proj_dir_names = []
    for uvoptx_pathname in uvoptx_pathnames:
        proj_dir_names.append(os.path.dirname(uvoptx_pathname))

    dir_names = []
    for proj_dir_name in proj_dir_names:
        dir_names.append(os.path.split(os.path.split(proj_dir_name)[0])[0])

    link_projects_to_sdk(sdkpath, proj_dir_names)


def run_application(sdkpath):
    pathname = os.getcwd()

    if glob.glob('*.uvoptx'):
        run_individual_project_file(pathname, sdkpath)

    else:
        run_all_project_files(pathname, sdkpath)


# start application
if __name__ == "__main__":
    # construct the argument parse and parse the arguments
    ap = argparse.ArgumentParser()
    ap.add_argument("-sdkpath", "--sdkpath", required=True,
                    help="FULL SDK PATH INSIDE DOUBLE QUOTATION OR \"CLEAN\" TO CLEAN PROJECT ENVIRONMENT")
    args = vars(ap.parse_args())

    # print("SDK location {},is the sdk path".format(args["sdkpath"]))
    run_application(args["sdkpath"])
# run_individual_project_file(dirnames,args["sdkpath"])
