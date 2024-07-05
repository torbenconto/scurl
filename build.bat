@echo off

REM Compile each .cpp file into an object file
g++ -c main.cpp -o main.o
g++ -c http.cpp -o http.o
g++ -c strings.cpp -o strings.o

REM Link all the object files together
g++ main.o http.o strings.o -o scurl.exe

REM Clean up the object files
del main.o
del http.o
del strings.o