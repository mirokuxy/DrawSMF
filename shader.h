#pragma once

// ----- include headers ----------

// C++
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
// GLEW
#include <GL/glew.h>

using namespace std;

// -------Define Shader Class------------

class Shader
{
public:
	// The program ID
	GLuint Program;
	// Constructor: reads and builds our shaders
	Shader(void){ this->Program = 0; }
	Shader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath);
	// reload operator =
	Shader operator = (const Shader& s){ this->Program = s.Program; return *this; }
	// Use This program
	void Use(); 
};

Shader::Shader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath){
	// 1. Rerieve the vertex/fragment source code from filePath
	string vShaderString;
	string fShaderString;
	try{
		// Open files
		ifstream vShaderFile(vertexShaderPath);
		ifstream fShaderFile(fragmentShaderPath);
		// Read file's bufer contents into streams
		stringstream vShaderStream,fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// Close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into GLchar array
		vShaderString = vShaderStream.str();
		fShaderString = fShaderStream.str();
	}
	catch(exception e){
		cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULL_READ" << endl;
	}
	const GLchar* vShaderCode = vShaderString.c_str();
	const GLchar* fShaderCode = fShaderString.c_str();

	// 2. Compile shaders
	GLuint vShader,fShader;
	GLint success; // whether compile success
	GLchar infoLog[512]; // Shader compile info
	// Compile vertex shader
	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader,1,&vShaderCode,NULL);
	glCompileShader(vShader);
	// Print compile errors if any
	glGetShaderiv(vShader,GL_COMPILE_STATUS,&success);
	if(!success){
		glGetShaderInfoLog(vShader,512,NULL,infoLog);
		cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}
	// Compile fragment shader
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader,1,&fShaderCode,NULL);
	glCompileShader(fShader);
	// Print compile errors if any
	glGetShaderiv(fShader,GL_COMPILE_STATUS,&success);
	if(!success){
		glGetShaderInfoLog(fShader,512,NULL,infoLog);
		cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	// 3. Create and link Program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vShader);
	glAttachShader(this->Program, fShader);
	glLinkProgram(this->Program);
	// Print linking errors if any
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(this->Program,512,NULL,infoLog);
		cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog <<endl;
	}

	// 4. Delete the shaders since they're linked into the program now and on longer neccessary.
	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

void Shader::Use(){
	glUseProgram(this->Program);
}
