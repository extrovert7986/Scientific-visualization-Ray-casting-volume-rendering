#include<GL\glew.h>
#include<GL\freeglut.h>
#include<stdio.h>
#include"CallbackFunc.h"

void main(int argv, char** argc) {
	
	glutInit(&argv, argc);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100 );
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Ray Casting Volume Rendering");

	if (glewInit() != GLEW_OK) {
		printf("glewInit failed.\n");
	}
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

}