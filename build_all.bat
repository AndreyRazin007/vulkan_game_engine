@ECHO OFF
REM Build Everything

IF EXIST "build_windows" (
    ECHO "The folder or subfolder already exists."
) ELSE (
    MD build_windows
)

ECHO "Building everything..."

PUSHD engine
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

PUSHD editor
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "All assemblies built successfully."
