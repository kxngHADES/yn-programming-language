@echo off
echo ==============================================
echo        Building YN Language Engine...
echo ==============================================

echo [1/3] Compiling Main Parser Modules...
gcc ync.c compiler.c parser.c symbol_table.c -o ync.exe

echo [2/3] Packing Native Engine Byte Array...
gcc packer.c -o packer.exe
packer.exe ync.exe ync_payload.h

echo [3/3] Building Standalone GUI Installer...
gcc install.c -mwindows -o install.exe

echo.
echo ==============================================
echo Compilation Finished! 
echo Share 'install.exe' with anyone to install YN.
echo ==============================================
