#!/usr/bin/python

# GL API Generation Script (GetGLAPI)
# ----------------------------------------------------------------------
# 
# Copyright (c) 2018-2019, Sergey Kosarevsky sk@linderdaum.com
#
# All rights reserved.
# 
# Redistribution and use of this software in source and binary forms,
# with or without modification, are permitted provided that the
# following conditions are met:
# 
# * Redistributions of source code must retain the above
#   copyright notice, this list of conditions and the
#   following disclaimer.
# 
# * Redistributions in binary form must reproduce the above
#   copyright notice, this list of conditions and the
#   following disclaimer in the documentation and/or other
#   materials provided with the distribution.
# 
# * Neither the name of the assimp team, nor the names of its
#   contributors may be used to endorse or promote products
#   derived from this software without specific prior
#   written permission of the assimp team.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# ----------------------------------------------------------------------

CheckErrorFunc = "assert(apiHook.glGetError() == GL_NO_ERROR);"
WrapFuncs = []

def typeNameToFormatter(typeName):
	if typeName == "GLuint": return "%u"
	if typeName == "GLint": return "%i"
	if typeName == "GLenum": return "%s"
	if typeName == "GLfloat": return "%f"
	if typeName == "GLdouble": return "%f"
	if typeName == "GLsizei": return "%i"
	if typeName == "GLbitfield": return "%u"
	if typeName == "GLboolean": return "%u"
	if typeName == "GLbyte": return "%u"
	if typeName == "GLchar": return "%u"
	if typeName == "GLuint64": return "%zu"

	if typeName == "GLsizeiptr": return "%\" PRId64\""
	if typeName == "GLintptr": return "%\" PRId64\""
	if typeName == "GLsync": return "%x"

	if typeName == "const GLfloat": return "%f"
	if typeName == "const GLfloat*": return "%p"
	if typeName == "const GLint*": return "%p"
	if typeName == "const GLuint*": return "%p"
	if typeName == "const GLvoid*": return "%p"
	if typeName == "const void*": return "%p"
	if typeName == "const GLenum*": return "%p"
	if typeName == "const GLchar*": return "%p"
	if typeName == "const GLsizei*": return "%p"
	if typeName == "const GLuint64*": return "%p"
	if typeName == "const GLint64*": return "%p"
	if typeName == "const GLintptr*": return "%p"
	if typeName == "const GLchar* const*": return "%p"
	if typeName == "void**": return "%p"
	if typeName == "GLfloat*": return "%p"
	if typeName == "GLint*": return "%p"
	if typeName == "GLuint*": return "%p"
	if typeName == "GLvoid*": return "%p"
	if typeName == "void*": return "%p"
	if typeName == "GLenum*": return "%p"
	if typeName == "GLchar*": return "%p"
	if typeName == "GLsizei*": return "%p"
	if typeName == "GLuint64*": return "%p"
	if typeName == "GLint64*": return "%p"
	if typeName == "GLintptr*": return "%p"

	if typeName == "void": return ""
	if typeName == "": return ""
	print( "ERROR: uknown type: ", typeName )
	exit(255)

def argNameToConverter(typeName, argName):
	if typeName == "GLenum": return "E2S("+argName+")"
	if typeName == "GLbitfield": return "(unsigned int)("+argName+")"
	if typeName == "GLboolean": return "(unsigned int)("+argName+")"
	if typeName == "GLbyte": return "(unsigned int)("+argName+")"
	if typeName == "GLchar": return "(unsigned int)("+argName+")"

	return argName

def generateStub(func):
	func = func.replace("GLAPI ", "").replace("APIENTRY ", " ").replace("*", "* ").replace(" *", "*").replace("(void)", "()")
	if (func[-1] == ";"):
		func = func[0:-1]
	args = func.split("(")
	funcName = args[0].split()[-1]
	returnType = args[0].split(funcName)[0].strip()
	if not funcName in WrapFuncs:
		return
	funcArgs = args[1].split(",")
	call = funcName + "(";
	allArgs = []
	for arg in funcArgs:
		argTypeName = arg.strip().split(" ");
		argFullTypeName = " ".join(argTypeName[:-1])
		if len(argFullTypeName) > 0:
			allArgs.append([argFullTypeName, argTypeName[-1]]);
		if argTypeName[-1][-1] == ")":
			if argTypeName[-1] == "void)":
				call = call + ")"
				continue
			call = call + argTypeName[-1]
		else:
			if argTypeName[-1] == "void":
				argTypeName[-1] = ""
			call = call + argTypeName[-1] + ", "
	print(returnType + " GLTracer_" + funcName + "(" + args[1])
	print("{")
	guardString = "\""
	for a in allArgs:
		guardString = guardString + typeNameToFormatter(a[0]) + ", "
	if len(allArgs) > 0: guardString = guardString[:-2]
	guardString = guardString + ")\\n\"";
	for a in allArgs:
		guardString = guardString + ", " + argNameToConverter(a[0], a[1])
	if len(allArgs) > 0:
		print("	printf(\""+funcName + "(\" " + guardString+";")
	else:
		print("	printf(\""+funcName+"()\\n\");")
	if returnType == "void":
		print("	apiHook."+call + ";")
		print("	" + CheckErrorFunc);
	else:
		print("	"+returnType + " const r = apiHook." + call + ";")
		print("	" + CheckErrorFunc);
		print("	return r;");
	print("}")
	print("")

def parseFuncs():
	global WrapFuncs
	lines = open("funcs_list.txt").readlines()
	for l in lines:
		func = l.split()[-1]
		WrapFuncs.append(func)
	WrapFuncs.sort()

def main():
	parseFuncs()
	print("#include <string>")
	print("#include <inttypes.h>")
	print("")
	print("namespace")
	print("{")
	print("	GL4API apiHook;");
	print("} // namespace")
	print("")
	print("using PFNGETGLPROC = void* (const char*);");
	print("")
	print("#define E2S( en ) Enum2String( en ).c_str()")
	print("extern std::string Enum2String( GLenum e );")
	print("")
	lines = open("glcorearb.h").readlines()
	for l in lines:
		if l[0:5:] == "GLAPI":
			generateStub(l.strip())

	print("#define INJECT(S) api->S = &GLTracer_##S;")
	print("")
	print("void InjectAPITracer4(GL4API* api)");
	print("{")
	print("	apiHook = *api;")
	Hooks = []
	for l in lines:
		if l[0:5:] == "GLAPI":
			func = l.strip();
			args = func.split("(")
			funcName = args[0].split()[3]
			if (funcName in WrapFuncs) and (funcName != "glGetError"):
				Hooks.append("	INJECT(" + funcName + ");")
	Hooks.sort();
	for f in Hooks:
		print(f)
	print("}")
	print("")
	print("#define LOAD_GL_FUNC(func) api->func = ( decltype(api->func) )GetGLProc(#func);")
	print("")
	print("void GetAPI4(GL4API* api, PFNGETGLPROC GetGLProc)");
	print("{")
	Funcs = []
	for l in lines:
		if l[0:5:] == "GLAPI":
			func = l.strip();
			args = func.split("(")
			funcName = args[0].split()[3]
			if funcName in WrapFuncs:
				Funcs.append("	LOAD_GL_FUNC(" + funcName + ");")
	Funcs.sort();
	for f in Funcs:
		print(f)
	print("}")
	print("")
	# generate API struct
	out = open( "GLAPI.h", "wt" )
	for line in WrapFuncs:
		line.strip();
		if str.find( line, "//" ) == 0 or line == "": continue
		typeName = "PFN" + line.upper() + "PROC"
		NumTabs = 17 - int( len(typeName) / 3 )
		out.write( '\t' + typeName + '\t'*NumTabs + line + ';\n' )

if __name__ == "__main__":
    main()
