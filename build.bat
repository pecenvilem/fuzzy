echo @off

@REM Enter file locations:
SET compiler_path=C:\msys64\mingw64\bin\g++.exe
SET main_folder_path=D:\vs\predmety\14PAM\programovani\paper

@REM Assemble compiler invocation...
SET envocation=%compiler_path% -g %main_folder_path%\include\*.cpp %main_folder_path%\main.cpp -o %main_folder_path%\main.exe

call %envocation%