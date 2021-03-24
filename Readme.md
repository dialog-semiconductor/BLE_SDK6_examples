# Smartbond SDK6 Application Examples

This is the repository storing example for the [DA145xx family](https://www.dialog-semiconductor.com/products/bluetooth-low-energy/da14530-and-da14531).

For information about the DA145xx platform and how to bring up your development kit, please refer to the [Getting started User Manual](http://lpccs-docs.dialog-semiconductor.com/UM-B-117-DA14531-Getting-Started-With-The-Pro-Development-Kit/index.html)

## Example usage

To run any of these SW examples, the user needs:

- DA14531/DA14585/DA14586 hardware.
- Keil µvision.
- Dialog SDK 6.0.14 available
- Python script

Below is the description on how to use the python script to link the SDK and the SW example, and to clean the SW example project environment (remove absolute paths).

**Note**: *It is highly recommended the user creates files that are added in the *src* folder of the SW example. The Python script does not process user files located in other folders.*

### <ins> Linking the project environment and the Dialog 6.0.14 SDK </ins>

**Note**: *Linking the SW example project environment and Dialog 6.0.14 SDK adds absolute file paths to files in your SW example project environment path containing information about your system’s folder structure. The Python script also supports cleaning these absolute file paths. See “Cleaning the project environment”.*

Below are the steps to link the SW example project environment to the 6.0.14 SDK.

1. Python 2.7 or higher is required to run the small example environment generation script.
Python can be downloaded from Python.org.

2. Download the SW example from the Dialog support portal:
DA14585 & DA14586 software examples can be found here: https://www.dialog-semiconductor.com/product/da14585
DA14531 software examples can be found here: https://www.dialog-semiconductor.com/product/da14531

3. Extract the zip file.

4. Open a terminal and run:

```console
> cd <example folder>/project_environment
> python dlg_make_keil5_env_v2.000.py -sdkpath “<path to your sdk repository>”
```

For instance:
```console
> python dlg_make_keil5_env_v1.006.py -sdkpath “C:\dev\6.0.14”
```

5. The script should indicate successful execution. Figure 1 shows an example.  


### <ins> Cleaning the project environment </ins>

Linking the project adds absolute paths to the project files, pointing them to the 6.0.14 SDK. The clean functionality removes these absolute paths.
Since absolute paths contain intermediate directory names, the clean command makes sure these intermediate directories are not shared with internal and external application users.

Example: After linking, `C:\Users\Your_name\Upcoming_project\Keil_5` reveals the user’s name and the name of the project the user is working on.
Below are the steps to clean the SW example project environment.

1. Python 2.7 or higher is required to run the small example environment generation script.
Python can be downloaded from Python.org.

2. Download the SW example from the Dialog support portal:

DA14585 & DA14586 software examples can be found here: https://www.dialog-semiconductor.com/product/da14585

DA14531 software examples can be found here: https://www.dialog-semiconductor.com/product/da14531

3. Extract the zip file.

4. Open a terminal and run:

```console
> cd <example folder>/project_environment
> python dlg_make_keil5_env_v2.000.py -sdkpath “clean”
```

5. The script should indicate successful execution. Figure 2 shows an example.


## Example compatibility

Not all the examples will run on the latest version of the SDK6, the tested version is indicated in the Readme. If you find and example that needs porting to the latest version please report it in the issues.