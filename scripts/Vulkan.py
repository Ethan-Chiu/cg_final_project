import os
import subprocess
import sys
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile
from colorama import init, Fore, Back, Style

import platform

init(convert=True)

VULKAN_SDK = os.environ.get('VULKAN_SDK')
ETHANE_VULKAN_VERSION = '1.3.204.1'
VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.3.204.1/windows/VulkanSDK-1.3.204.1-Installer.exe'
VULKAN_SDK_EXE_PATH = 'EthaneEngine/vendor/VulkanSDK/VulkanSDK.exe'

if platform.system() == 'Darwin':
    print("Vulkan for Mac")
    opener = "open"
    VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.3.204.1/mac/vulkansdk-macos-1.3.204.1.dmg'
    VULKAN_SDK_EXE_PATH = 'EthaneEngine/vendor/VulkanSDK/VulkanSDK.dmg'


def InstallVulkanSDK():
    print('Downloading {} to {}'.format(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH))
    Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
    print("Done!")
    print("Running Vulkan SDK installer...")
    if platform.system() == 'Darwin':
        subprocess.call([opener, os.path.abspath(VULKAN_SDK_EXE_PATH)])
    else:
        os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
    # print("Re-run this script after installation")
    return True

def InstallVulkanPrompt():
    print("Would you like to install the Vulkan SDK?")
    install = Utils.YesOrNo()
    if (install):
        return InstallVulkanSDK()
    else:
        return False

def CheckVulkanSDK():
    if (VULKAN_SDK is None):
        print("You don't have the Vulkan SDK installed!")
        return InstallVulkanPrompt()
    elif (ETHANE_VULKAN_VERSION not in VULKAN_SDK):
        print(f"Located Vulkan SDK at {VULKAN_SDK}")
        print(f"You don't have the correct Vulkan SDK version! (ETHANE requires {ETHANE_VULKAN_VERSION})")
        return InstallVulkanPrompt()

    print(f"Correct Vulkan SDK located at {VULKAN_SDK}")
    return True


def CheckVulkanSDKDebugLibs():
    shadercdLib = Path(f"{VULKAN_SDK}/Lib/shaderc_sharedd.lib")
    if (not shadercdLib.exists()):
        print(f"{Style.BRIGHT}{Back.YELLOW}Warning: No Vulkan SDK debug libs found. (Checked {shadercdLib})")
        print(f"{Back.RED}Debug builds are not possible.{Style.RESET_ALL}")
        return False
    return True