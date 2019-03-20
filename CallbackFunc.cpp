#include<GL\glew.h>
#include<GL\freeglut.h>
#include<stdio.h>
#include<math.h>
#include"ReadFile.h"
#include"ShaderProgrm.h"
#include"Texture.h"
#include"Cube.h"

const char* fileName = "cthead.inf";
rawInf_t rawinf;
float* data = nullptr;
float* gra = nullptr;		//gradient x*y*z*3

float cubeCoord[3 * 3 * 2 * 6];
float cubeTex[3 * 3 * 2 * 6];

double zoomFactor = 1.0;
float eyePos[3] = { 500.0,50.0,0.0 };
float focus[3] = { 0,0,0 };
float vupVec[3] = { 0,1,0 };
float u[3];
float w[3];

GLuint program;
GLuint vao;
GLuint vaoBound;
GLuint tex3D;
GLuint tex1D;

float sampleRate = 0.001;

GLuint create_shader_program();
void create_vao();
void setLight();
void init() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	program = create_shader_program();
	glUseProgram(program);
	//read info and raw data
	rawinf = read_vol(fileName);
	data = read_raw(rawinf);
	//compute gradient
	gra = compute_gradient(data, rawinf);
	//create vao
	create_vao();
	//setting 3D & 1D texture
	setting_3D_texture(rawinf, data, gra);
	setting_1D_texture();
	//setting light paraneter
	setLight();
	//set sample rate
	glUniform1f(glGetUniformLocation(program, "dist"), sampleRate);

}

void display() {
	glClearColor(0.3, 0.3, 0.3, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float mtx[16];
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-150 * zoomFactor, 150 * zoomFactor, -150 * zoomFactor, 200 * zoomFactor, 1, 1000);
	glGetFloatv(GL_PROJECTION_MATRIX, mtx);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_FALSE, mtx);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyePos[0], eyePos[1], eyePos[2], focus[0], focus[1], focus[2], vupVec[0], vupVec[1], vupVec[2]);
	for (int i = 0; i < 16; i++)
		mtx[i] = 0.0;
	glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMatrix"), 1, GL_FALSE, mtx);
	
	glUseProgram(0);
	for (int i = 0; i < 12; i++) {
		glBegin(GL_LINES);
		glVertex3f((cube[bound[i][0] * 3] * 2 - 1)*rawinf.res.x, (cube[bound[i][0] * 3 + 1] * 2 - 1)*rawinf.res.y, (cube[bound[i][0] * 3 + 2] * 2 - 1)*rawinf.res.z);
		glVertex3f((cube[bound[i][1] * 3] * 2 - 1)*rawinf.res.x, (cube[bound[i][1] * 3 + 1] * 2 - 1)*rawinf.res.y, (cube[bound[i][1] * 3 + 2] * 2 - 1)*rawinf.res.z);
		glEnd();
	}
	glUseProgram(program);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES,0, 36);
	

	glutSwapBuffers();
}

void reshape(int x, int y) {
	glViewport(0, 0, x, y);
}

void mouse(int button,int state,int x,int y) {

}

void keyboard(unsigned char key, int x, int y) {
	//compute w vector
	w[0] = focus[0] - eyePos[0];
	w[1] = focus[1] - eyePos[1];
	w[2] = focus[2] - eyePos[2];
	double mag = sqrt(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
	w[0] /= mag; w[1] /= mag; w[2] /= mag;
	//compute u vector

	u[0] = w[1] * vupVec[2] - vupVec[1] * w[2];
	u[1] = w[2] * vupVec[0] - vupVec[2] * w[0];
	u[2] = w[0] * vupVec[1] - vupVec[0] * w[1];
	mag = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
	u[0] /= mag; u[1] /= mag; u[2] /= mag;
	//compute vup vector
	vupVec[0] = u[1] * w[2] - w[1] * u[2];
	vupVec[1] = u[2] * w[0] - w[2] * u[0];
	vupVec[2] = u[0] * w[1] - w[0] * u[1];
	mag = sqrt(vupVec[0] * vupVec[0] + vupVec[1] * vupVec[1] + vupVec[2] * vupVec[2]);
	vupVec[0] /= mag; vupVec[1] /= mag; vupVec[2] /= mag;

	if (key == 'w' || key == 'W') {
		GLfloat mat[16];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-focus[0], -focus[1], -focus[2]);
		glRotatef(0.5, u[0], u[1], u[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		double tmp[3];
		tmp[0] = mat[0] * eyePos[0] + mat[4] * eyePos[1] + mat[8] * eyePos[2];
		tmp[1] = mat[1] * eyePos[0] + mat[5] * eyePos[1] + mat[9] * eyePos[2];
		tmp[2] = mat[2] * eyePos[0] + mat[6] * eyePos[1] + mat[10] * eyePos[2];
		eyePos[0] = tmp[0]; eyePos[1] = tmp[1]; eyePos[2] = tmp[2];
	}
	if (key == 's' || key == 'S') {
		GLfloat mat[16];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-focus[0], -focus[1], -focus[2]);
		glRotatef(-0.5, u[0], u[1], u[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		double tmp[3];
		tmp[0] = mat[0] * eyePos[0] + mat[4] * eyePos[1] + mat[8] * eyePos[2];
		tmp[1] = mat[1] * eyePos[0] + mat[5] * eyePos[1] + mat[9] * eyePos[2];
		tmp[2] = mat[2] * eyePos[0] + mat[6] * eyePos[1] + mat[10] * eyePos[2];
		eyePos[0] = tmp[0]; eyePos[1] = tmp[1]; eyePos[2] = tmp[2];
	}
	if (key == 'a' || key == 'A') {
		GLfloat mat[16];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-focus[0], -focus[1], -focus[2]);
		glRotatef(0.5, vupVec[0], vupVec[1], vupVec[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		double tmp[3];
		tmp[0] = mat[0] * eyePos[0] + mat[4] * eyePos[1] + mat[8] * eyePos[2];
		tmp[1] = mat[1] * eyePos[0] + mat[5] * eyePos[1] + mat[9] * eyePos[2];
		tmp[2] = mat[2] * eyePos[0] + mat[6] * eyePos[1] + mat[10] * eyePos[2];
		eyePos[0] = tmp[0]; eyePos[1] = tmp[1]; eyePos[2] = tmp[2];
	}
	if (key == 'd' || key == 'D') {
		GLfloat mat[16];
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-focus[0], -focus[1], -focus[2]);
		glRotatef(-0.5, vupVec[0], vupVec[1], vupVec[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		double tmp[3];
		tmp[0] = mat[0] * eyePos[0] + mat[4] * eyePos[1] + mat[8] * eyePos[2];
		tmp[1] = mat[1] * eyePos[0] + mat[5] * eyePos[1] + mat[9] * eyePos[2];
		tmp[2] = mat[2] * eyePos[0] + mat[6] * eyePos[1] + mat[10] * eyePos[2];
		eyePos[0] = tmp[0]; eyePos[1] = tmp[1]; eyePos[2] = tmp[2];
	}
	if (key == 'z' || key == 'Z') {
		zoomFactor *= 0.9;
	}
	if (key == 'x' || key == 'X') {
		zoomFactor /= 0.9;
	}
	if (key == 'u' || key == 'U') {
		sampleRate *= 10;
		glUniform1f(glGetUniformLocation(program, "dist"), sampleRate);
	}
	if (key == 'i' || key == 'I') {
		sampleRate /= 10;
		glUniform1f(glGetUniformLocation(program, "dist"), sampleRate);
	}
	glutPostRedisplay();
}

void setLight() {
	float lightIa[3] = { 0.3,0.3,0.3 };
	float lightId[3] = { 0.7,0.7,0.7 };
	float lightIs[3] = { 0.2,0.2,0.2 };

	glUniform3fv(glGetUniformLocation(program, "Light.Ia"), 1, lightIa);
	glUniform3fv(glGetUniformLocation(program, "Light.Id"), 1, lightId);
	glUniform3fv(glGetUniformLocation(program, "Light.Is"), 1, lightIs);

	glUniform1f(glGetUniformLocation(program, "shiness"), 32.0);
}

GLuint create_shader_program() {
	GLuint shader[5];
	shader[0] = set_shader(GL_VERTEX_SHADER, "vertex.glsl");
	shader[1] = set_shader(GL_FRAGMENT_SHADER, "fragment.glsl");
	return set_program(shader);

}

void create_vao() {
	
	float resx = rawinf.res.x / 2.0;
	float resy = rawinf.res.y / 2.0;
	float resz = rawinf.res.z / 2.0;

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 3; j++) {
			//        tri    point  coord
			cubeCoord[i * 9 + j * 3 + 0] = (cube[tri[i][j] * 3 + 0] * 2 - 1)*resx*rawinf.ratio.x;
			cubeTex[i * 9 + j * 3 + 0] = cube[tri[i][j] * 3 + 0];
			cubeCoord[i * 9 + j * 3 + 1] = (cube[tri[i][j] * 3 + 1] * 2 - 1)*resy*rawinf.ratio.y;
			cubeTex[i * 9 + j * 3 + 1] = cube[tri[i][j] * 3 + 1];
			cubeCoord[i * 9 + j * 3 + 2] = (cube[tri[i][j] * 3 + 2] * 2 - 1)*resz*rawinf.ratio.z;
			cubeTex[i * 9 + j * 3 + 2] = cube[tri[i][j] * 3 + 2];
		}
	}
	GLuint vbo[2];
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeCoord), cubeCoord, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTex), cubeTex, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexBuffer(0, vbo[0], 0, sizeof(GLfloat) * 3);
	glBindVertexBuffer(1, vbo[1], 0, sizeof(GLfloat) * 3);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(1, 1);

		
}