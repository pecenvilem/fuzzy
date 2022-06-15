echo @off
SET script_folder=%~dp0
call %script_folder%venv\Scripts\activate
@REM Pass all arguments of this script to 'plot.py'
call %script_folder%plot.py %*
call %script_folder%venv\Scripts\deactivate