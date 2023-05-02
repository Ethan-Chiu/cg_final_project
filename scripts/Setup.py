import os
import subprocess

import CheckPython

# Make sure everything we need is installed
CheckPython.ValidatePackages()

import Vulkan
import Premake

# Change from Scripts directory to root
os.chdir('../')

if (not Vulkan.CheckVulkanSDK()):
    print("Vulkan SDK not installed.")
    quit()
print("----------------------------------------")

# if (not Vulkan.CheckVulkanSDKDebugLibs()):
#     print("Vulkan SDK debug libs not found.")
#     quit()
# print("----------------------------------------")

if (not Premake.CheckPremakeExe()):
    print("Premake5.exe not installed. ")
    quit()
print("----------------------------------------")

# print("Running premake...")
# subprocess.call(["vendor/premake/bin/premake5.exe", "vs2022"])
print("----------------------------------------")

input("Program ended. Press [Enter] to close ... ")