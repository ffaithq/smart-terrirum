@echo off
REM Set up a virtual environment
echo Creating a virtual environment...
python -m venv SmartTerrarium

REM Activate the virtual environment
call SmartTerrarium\Scripts\activate

REM Install required packages
echo Installing required packages...
pip install -r requirements.txt
