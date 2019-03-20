#include<stdio.h>
#include<stdlib.h>
#include<GL\glew.h>

GLuint set_program(GLuint shader[5]) {
	//create program
	GLuint program;
	if ((program = glCreateProgram()) == 0) {
		fprintf(stderr, "Create program failed.\n");
		system("pause");
		exit(0);
	}
	//attach shaders
	for (int i = 0; i < 5; i++) {
		glAttachShader(program, shader[i]);
	}
	//link program
	glLinkProgram(program);
	//verify the program
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			GLchar* log = (GLchar*)calloc(logLen, sizeof(GLchar));
			GLsizei written;
			glGetProgramInfoLog(program, logLen, &written, log);
			fprintf(stderr, "Program log:\n%s", log);
			free(log);
		}

	}
	return program;

}

char* read_file(const char*);
GLuint set_shader(GLenum type, const char* name) {
	//create shader
	GLuint shader;
	if ((shader = glCreateShader(type)) == 0) {
		fprintf(stderr,"Create shader %s failed.\n",name);
		system("pause");
		exit(0);
	}
	//copy source code into shader object
	const GLchar* sourceCode = read_file(name);
	const GLchar* codeArray[] = { sourceCode };
	glShaderSource(shader, 1, codeArray, NULL);
	//compile shader object
	glCompileShader(shader);
	//verify status
	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		fprintf(stderr, "Shader %s compile failed.\n", name);
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length > 0) {
			GLchar* log = (GLchar*)calloc(length, sizeof(GLchar));
			GLsizei written;
			glGetShaderInfoLog(shader, length, &written, log);
			
			fprintf(stderr, "Shader log:\n%s", log);
			free(log);
		
		}
	}
	return shader;
}

char* read_file(const char* name) {
	FILE* fp = fopen(name, "r");
	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	rewind(fp);
	char* content = (char*)calloc(fsize, sizeof(char));
	fread(content, sizeof(char), fsize, fp);
	fclose(fp);
	printf("----------- Shader -----------\n");
	printf("%s\n", content);
	
	return content;
}
