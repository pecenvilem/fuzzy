echo @off

SET install_path=%~dp0

echo "Deleting virtual environment..."
rmdir %install_path%venv /s /q

echo "Plotting uninstalled successfully."
PAUSE