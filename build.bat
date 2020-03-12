@echo off
windres puck.rc -O coff -o puck.res
g++ -o chrome main.cpp puck.res