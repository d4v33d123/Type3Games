@echo off
::Make sure your ndk folder is one folder up from this batch file's location
echo Compiling, please wait...
echo ---

::move to ndk directory
cd ../android-ndk-r10e

::ndk-build in the specified path
call ndk-build -C ..\Type3Games\app\src\main\jni

echo ---
pause