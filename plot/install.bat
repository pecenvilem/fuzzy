echo @off

SET install_path=%~dp0venv

echo Creating virtual environment in %install_path%
python -m venv %install_path%

echo "Activating virtual environment..."
call %install_path%\Scripts\activate

echo "Installing packages..."
@REM pip install pandas matplotlib
pip install -r %~dp0requirements.txt

echo "Deactivating virtual environment..."
call %install_path%\Scripts\deactivate

@REM Unset environment variable
SET install_path=

echo "Plotting installed successfully."
PAUSE
