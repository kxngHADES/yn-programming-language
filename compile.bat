@echo off
echo Compiling YN Language Engine...
gcc ync.c compiler.c parser.c symbol_table.c -o ync.exe
echo Compilation Finished!
echo Run 'ync.exe script.yn' to test.
