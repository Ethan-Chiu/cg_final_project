#!/bin/sh
python Setup.py
cd ..
./vendor/premake/bin/premake5 xcode4
./vendor/premake/bin/premake5 gmake2