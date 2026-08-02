@echo off
setlocal
cd /d "%~dp0\.."
set VENV_DIR=.venv-platformio
where py >nul 2>nul
if %errorlevel%==0 (set PYTHON_CMD=py -3) else (set PYTHON_CMD=python)
if not exist "%VENV_DIR%" (
    echo Creating local virtualenv in %VENV_DIR% using: %PYTHON_CMD%
    %PYTHON_CMD% -m venv "%VENV_DIR%"
    if not exist "%VENV_DIR%\Scripts\python.exe" (
        echo.
        echo ERROR: Could not create a Python 3 virtualenv.
        echo Make sure Python 3 is installed from https://www.python.org/downloads/
        exit /b 1
    )
    "%VENV_DIR%\Scripts\python.exe" -m pip install --upgrade pip
    "%VENV_DIR%\Scripts\pip.exe" install platformio
)
echo Running build (no upload) ...
"%VENV_DIR%\Scripts\pio.exe" run
if errorlevel 1 (echo Build FAILED. & exit /b 1)
echo Build finished successfully.
endlocal
