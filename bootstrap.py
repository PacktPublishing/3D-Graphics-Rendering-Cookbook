#!/usr/bin/python3

import os
import sys
import platform
import subprocess

Python = "python" if platform.system() == "Windows" else "python3"

def executeCommandSilent(command):
	out = open(os.devnull, 'w')
	err = subprocess.STDOUT
	return subprocess.call(command, shell = True, stdout=out, stderr=err);

def runPythonScript(Script, Params):
	if os.system( Python + " " + Script + " " + Params ) != 0:
		print( "Unable to run " + Script )
		exit(255)

if executeCommandSilent(Python + " --version") != 0:
	print( "Make sure Python can be started from the command line (add path to `python.exe` to PATH on Windows)" )
	exit(255)

if executeCommandSilent("cmake --version") != 0:
	print( "Install CMake first" )
	exit(255)

if executeCommandSilent("git --version") != 0:
	print( "Install Git first" )
	exit(255)

if executeCommandSilent("hg --version") != 0:
	print( "Install Mercurial first" )
	exit(255)

# run Tools/Bootstrap

runPythonScript( os.path.join("deps", "bootstrap.py"), "-b deps" )

print( "" )
print( "Bootstrapping done." )
