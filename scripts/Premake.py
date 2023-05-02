import os
import Utils

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile
import tarfile

import platform

OUTPUT_DIRECTORY = "vendor/premake/bin"
PREMAKE_INSTALLER_URL = "https://github.com/premake/premake-core/releases/download/v5.0.0-beta1/premake-5.0.0-beta1-windows.zip"
PREMAKE_EXE_PATH = f"{OUTPUT_DIRECTORY}/premake5.exe"

if platform.system() == 'Darwin':
    print("Premake for Mac")
    PREMAKE_INSTALLER_URL = "https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-macosx.tar.gz"
    PREMAKE_EXE_PATH = f"{OUTPUT_DIRECTORY}/premake5"

def InstallPremake():
    print('Downloading {} to {}'.format(PREMAKE_INSTALLER_URL, PREMAKE_EXE_PATH))
    with urlopen(PREMAKE_INSTALLER_URL) as zipresp:
        if platform.system() == 'Darwin':
            with tarfile.open(fileobj=BytesIO(zipresp.read())) as tfile:
                tfile.extractall(OUTPUT_DIRECTORY)
        else:
            with ZipFile(BytesIO(zipresp.read())) as zfile:
                zfile.extractall(OUTPUT_DIRECTORY)
    print("premake5.exe downloaded at: ", PREMAKE_EXE_PATH)
    return True

def InstallPremakePrompt():
    print("Would you like to install the Premake.exe?")
    install = Utils.YesOrNo()
    if (install):
        return InstallPremake()
    else:
        return False

def CheckPremakeExe():
    if (not os.path.isfile(PREMAKE_EXE_PATH)):
        print("You don't have the premake installed!")
        return InstallPremakePrompt()

    print(f"Correct Premake.exe located at {PREMAKE_EXE_PATH}")
    return True