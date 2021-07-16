###### Top
# Project to SDK Python Link Script
Repo used for Python script to link projects downloaded from Dialog site to Dialog SDK.

## Contents
1. [Background](#Background).
2. [Problem Analysis](#Problem-Analysis).
3. [Goal](#Goal).
4. [Requirements](#Requirements).
5. [Git Structure](#Git-Structure).
6. [Quick To Do List](#Quick-To-Do-List).

## Background
Dialog has a Python script that allows users to link their project environments (for example the ones that can be downloaded from the official Dialog site) to a Dialog SDK. This makes using Dialog's projects and SDK relatively easy: writing one line into a terminal achieves the desired result.\
However, it was noted that the current script leaves potential security issues; the script needs to be updated.

## Problem Analysis
The current Python linker script uses absolute paths to link the project environment to the Dialog SDK. Let's take this absolute path for example:\
`C:\Users\jbracht\585-531Workspace\6.0.11.1002\projects\target_apps\peripheral_examples\shared\DA14585_586.ini`\
Malicious parties might be able to use this personal folder structure to attack the user.

In this case the project environment is located at `C:\Users\jbracht\585-531Workspace\blinky\Keil_5`. Using a relative path, this could become `..\..\6.0.11.1002\projects\target_apps\peripheral_examples\shared\DA14585_586.ini`. This only shows the SDK structure, which is available to any user and thus no problem.

## Goal
- Inform the user of the creation of absolute file paths which will reveal personal data structures.
- Give the user the option to remove these absolute paths by calling the Python script with another parameter using the terminal. This could be desirable when the user wants to share their project online. This unlinking will 'reset' the project environment to using relative paths.
- ~~Create a user GUI: the user no longer needs to use the terminal to link the proj env & SDK.~~\
Scratched because a GUI would need an .exe and .exe files might be blocked on the computers of certain users, for example due to company security policy.

## Requirements
_Using MoSCoW._

### Script
1. __MUST__ The new script must have at least the same functionalities as the previous script: linking the project environment to the SDK.
2. __MUST__ Generated paths by the link function are relative when the SDK and project are located on the same drive.
3. __MUST__ The script can run a clean function after passing in the right parameter to the terminal which will 'reset' the xml project files to contain no absolute/only relative paths.
4. __Could__ The SDK version must be saved in the project file. This allows a user to check the file structure of the noted SDK.
5. __Could__ The script will extract the SDK version from the SDK.
6. __MUST__ The script must link the project environment to the SDK when they are on a different drive.
7. __MUST__ Script will inform user in some way about dangers of using relative paths. An example of this would be: prompt when trying to link if project env & SDK are on different drives: _"Linking proj env & SDK on different drives will add personal folder structure to linker paths, continue? (y/n)"._
7. __NOT__ The script does not have to account for changes in the SDK structure.

## Git Structure
The Git structure currently used is based on two branches: "develop" and "release". A visual representation can be seen in _Figure 1_ below.
- __develop__ is used to improve the script, with the final goal of a working script for the new SDK.
    - __develop__ has sub-branches based on SDK releases.\
    For example `SDK` 5 & `SDK 6`.
    - Each sub-branch will develop the script for that SDK, with further sub-branches to add new features.
- __release__ is used for release versions of the script only.
    - __release__ has sub-branches based on new _public_ SDK releases.\
    For example `6.0.10.511`.
    - Each sub branch will contain the version of the script that will be released to the public at the same time of the SDK.\
    For example: `dlg_make_keil5_env_v1.002.py` was released at the same time as `SDK 6.0.10.511`.

![Git structure visualization](Git%20Structure%20Visualization.png)
_Fig. 1 - Git Structure visualization._

Finally, if a branch is no longer used, it is recommended to mark this in the branch using either a commit or in the readme file.\
For example: "Branch no longer supported."

## Quick To Do List
1. Current script does not work properly when either SDK or proj env path contains one or multiple spaces!
    1. Three types of errors when having spaces in file paths:
        > __a)__ Linking with space in SDK path gives errors:\
        `Error: C4065E: type of input file '6.0.11.1009\sdk\app_modules' unknown`

        > __b)__ Linking with space in project environment path gives errors:\
        `Fatal error: C3907U: Via file '.\out_da14531\objects\system_da14531.__i' command too long for buffer.`\
        `Fatal error: C3901U: Missing argument for option 'I'.`

        > __c)__ Linking with space in both SDK path & project environment path gives errors:\
        `Fatal error: C3906U: Malformed via file '.\out_da14531\objects\arch_main.__i'.`
2. Improve `user security` by removal or limitation of use of `absolute paths` created on linking.
    1. User should be able to easily remove absolute paths. Will add `"unlink"` functionality to script that removes all absolute paths from project environment files (`.uvoptx & .uvprojx`).
    2. File should inform user about `absolute path security danger`. Could be done using prompt when linking, for example: _"Linking proj env & SDK on different drives will add personal folder structure to linker paths, continue? (y/n)"_
    3. If project environment & SDK are on same drive, linker should use relative paths to link project env & SDK.
    4. In case proj env & SDK are on different drive, linker should link using absolute paths.


[^ Back to top. ^](#Top)
