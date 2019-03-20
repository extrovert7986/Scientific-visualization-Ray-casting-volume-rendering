#include"RawInfoType.h"
#include"Color.h"
#include<stdlib.h>
#include<GL\glew.h>
#include<math.h>

void setting_3D_texture(rawInf_t rawinf,float* data,float* gradient) {
	//populate 3D texture
	float* tex = (float*)calloc(rawinf.res.x*rawinf.res.y*rawinf.res.z*4, sizeof(float));
	float max;
	switch (rawinf.smplTyp) {
	case UNSIGNED_CHAR_T:
		max = 255.0;
		break;
	case UNSIGNED_SHORT_T:
		max = 65535.0;
		break;
	}
	for (int k = 0; k < rawinf.res.z; k++)
		for (int j = 0; j < rawinf.res.y; j++)
			for (int i = 0; i < rawinf.res.x; i++) {
				int index = i + j*rawinf.res.x + k*rawinf.res.x*rawinf.res.y;
				float mag = sqrtf(gradient[index * 3] * gradient[index * 3] + gradient[index * 3 + 1] * gradient[index * 3 + 1] + gradient[index * 3 + 2] * gradient[index * 3 + 2]);
				if (mag < 0.0001) { mag = 1.0; }
				tex[index * 4] = gradient[index * 3]/mag;
				tex[index * 4 + 1] = gradient[index * 3 + 1]/mag;
				tex[index * 4 + 2] = gradient[index * 3 + 2]/mag;
				tex[index * 4 + 3] = data[index] / max;
			}
	//enable texture unit
	glActiveTexture(GL_TEXTURE0);
	GLuint textureID;
	//create texture object
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	//allocate the texture content memory
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, rawinf.res.x, rawinf.res.y, rawinf.res.z);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, rawinf.res.x, rawinf.res.y, rawinf.res.z, GL_RGBA, GL_FLOAT, tex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//free data
	free(tex);
}

void setting_1D_texture() {
	GLuint textureID;
	//enable texture unit
	glActiveTexture(GL_TEXTURE1);
	//create texture object
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_1D, textureID);
	//bind current texture object and set the data
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGBA32F, 256);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, GL_RGBA, GL_FLOAT, color);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
}