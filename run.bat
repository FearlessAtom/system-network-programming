@echo off

g++ main.cpp -o main.exe -lgdi32 -lshlwapi
main.exe
del main.exe
