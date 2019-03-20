#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"RawInfoType.h"

void extract_name(char* infoStr, char*  target);
void extract_res(char* infoStr, res_t* res);
void extract_ratio(char* infoStr, ratio_t* ratio);
void extract_type(char* infoStr, type_t* smplTyp);
void extract_endian(char* infoStr, endian_t* endian);

rawInf_t read_vol(const char* fileName) {
	//read in a file name
	FILE *fp = fopen(fileName, "r");
	int fileSize = 0;
	char* infoStr;
	float* data = nullptr;
	rawInf_t rawinf;

	if (!fp) {
		printf("File open failed.\n");
		system("pause");
		exit(0);
	}
	//retrive size of info file
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	rewind(fp);
	//read in info file
	infoStr = (char*)calloc(fileSize, sizeof(char));
	fread(infoStr, sizeof(char), fileSize, fp);
	//close opened file
	fclose(fp);

	extract_name(infoStr,rawinf.fileName);
	printf("file name: %s.\n", rawinf.fileName);
	extract_res(infoStr, &(rawinf.res));
	printf("res: %dx%dx%d.\n", rawinf.res.x, rawinf.res.y, rawinf.res.z);
	extract_ratio(infoStr, &(rawinf.ratio));
	printf("ratio: %f:%f:%f.\n", rawinf.ratio.x, rawinf.ratio.y, rawinf.ratio.z);
	extract_type(infoStr, &(rawinf.smplTyp));
	printf("sample type: %d.\n", rawinf.smplTyp);
	extract_endian(infoStr, &(rawinf.endian));
	printf("endian: %d.\n", rawinf.endian);
	return rawinf;
}

void extract_name(char* infoStr,char*  target) {
	char* tmp = strstr(infoStr, "raw-file");
	
	if (!tmp) {
		printf("raw-file not found.\n");
		system("pause");
		exit(0);
	}
	sscanf(tmp,"%*[^=]=%s", target);
	
}

void extract_res(char* infoStr, res_t* res) {
	char* tmp = strstr(infoStr, "resolution");

	if (!tmp) {
		printf("resolution not found.\n");
		system("pause");
		exit(0);
	}
	sscanf(tmp, "%*[^=]=%dx%dx%d", &res->x, &res->y, &res->z);

}

void extract_ratio(char* infoStr, ratio_t* ratio) {
	char* tmp = strstr(infoStr, "ratio");

	if (!tmp) {
		printf("ratio not found. use default( 1.0 : 1.0 : 1.0 )\n");
		ratio->x = 1.0;
		ratio->y = 1.0;
		ratio->z = 1.0;
	}
	sscanf(tmp, "%*[^=]=%f:%f:%f", &ratio->x, &ratio->y, &ratio->z);
}

void extract_type(char* infoStr, type_t* smplTyp) {
	char* tmp = strstr(infoStr, "sample-type");
	char smp[100];

	if (!tmp) {
		printf("sample-type not found. use default unsigned char.\n");
		*smplTyp = UNSIGNED_CHAR_T;
		return;
	}
	sscanf(tmp, "%*[^=]=%[^\n]", smp);
	if (!strcmp(smp, "unsigned char"))
		*smplTyp = UNSIGNED_CHAR_T;
	else if (!strcmp(smp, "unsigned short"))
		*smplTyp = UNSIGNED_SHORT_T;

}

void extract_endian(char* infoStr, endian_t* endian) {
	char* tmp = strstr(infoStr, "endian");
	char end[100];

	if (!tmp) {
		printf("endian not found. use default little-endian.\n");
		*endian = LITTLE;
		return;
	}
	sscanf(tmp, "%*[^=]=%[^\n]", end);
	
	if (!strcmp(end, "big")) {
		*endian = BIG; 
		return;
	}
	*endian = LITTLE;

}

/////////////////////////////////////////////////////////
void read_unsigned_char(float* data, unsigned char*buffer, int size);
void read_unsigned_short(float* data, unsigned char* buffer, int size, endian_t endian);

float* read_raw(const rawInf_t rawinf) { 
	int dataSize = rawinf.res.x*rawinf.res.y*rawinf.res.z;
	float* data = (float*)calloc(dataSize, sizeof(float));
	FILE *fp = fopen(rawinf.fileName, "rb");
	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	rewind(fp);
	unsigned char* buffer = (unsigned char*)calloc(fileSize, sizeof(unsigned char));
	fread(buffer, sizeof(unsigned char), fileSize, fp);
	for (int i = 0; i < 10; i++)printf("%d ", buffer[i]);
	printf("\n");

	switch (rawinf.smplTyp) {
	case UNSIGNED_CHAR_T:
		read_unsigned_char(data, buffer, dataSize);
		for (int i = 0; i < 10; i++)printf("%f ", data[i]);
		printf("\n");
		break;
	case UNSIGNED_SHORT_T:
		read_unsigned_short(data, buffer, dataSize, rawinf.endian);
		break;
	case FLOAT_T:
		break;
	default:
		printf("Unsupport data sample type.\n");
		system("pause");
		exit(0);
		break;
	}
	free(buffer);
	return data;
}

void read_unsigned_char(float* data,unsigned char* buffer,int size) {
	for (int i = 0; i < size; i++)
		data[i] = (float)buffer[i];
}

void read_unsigned_short(float* data,unsigned char* buffer,int size,endian_t endian) {
	switch (endian) {
	case BIG:
		for (int i = 0; i < size; i++) {
			data[i] = ((float)buffer[i * 2] *256) + (float)buffer[i * 2 + 1];
		}
		break;
	case LITTLE:
		for (int i = 0; i < size; i++) {
			data[i] = ((float)buffer[i * 2 + 1] *256) + (float)buffer[i * 2];
		}
		break;
	default:
		break;
	}
}
////////////////////////////////////////////////////////
float* compute_gradient(const float* data, const rawInf_t rawinf) {
	float* gradient = (float*)calloc(rawinf.res.x*rawinf.res.y*rawinf.res.z*3, sizeof(float));
	int idx;
	int x0, x1, y0, y1, z0, z1;
	float dx, dy, dz;
	float vx0, vx1, vy0, vy1, vz0, vz1;
	for (int k = 0; k < rawinf.res.z; k++)
		for (int j = 0; j < rawinf.res.y; j++)
			for (int i = 0; i < rawinf.res.x; i++) {
				idx = i + j*rawinf.res.x + k*rawinf.res.x*rawinf.res.y;
			
				if (i == 0) {
					x0 = i;
					x1 = i + 1;
					dx = 1.0;
				}else if (i == rawinf.res.x-1) {
					x0 = i - 1;
					x1 = i;
					dx = 1.0;
				}else {
					x0 = i - 1;
					x1 = i + 1;
					dx = 2.0;
				}

				if (j == 0) {
					y0 = j;
					y1 = j + 1;
					dy = 1.0;
				}else if (j == rawinf.res.y-1) {
					y0 = j - 1;
					y1 = j;
					dy = 1.0;
				}else {
					y0 = j - 1;
					y1 = j + 1;
					dy = 2.0;
				}

				if (k == 0) {
					z0 = k;
					z1 = k + 1;
					dz = 1.0;
				}else if (k == rawinf.res.z-1) {
					z0 = k - 1;
					z1 = k;
					dz = 1.0;
				}else {
					z0 = k - 1;
					z1 = k + 1;
					dz = 2.0;
				}
				vx0 = data[x0 + j*rawinf.res.x + k*rawinf.res.x*rawinf.res.y];
				vx1 = data[x1 + j*rawinf.res.x + k*rawinf.res.x*rawinf.res.y];
				gradient[idx * 3] = (vx1 - vx0) / dx;

				vy0 = data[i + y0*rawinf.res.x + k*rawinf.res.x*rawinf.res.y];
				vy1 = data[i + y1*rawinf.res.x + k*rawinf.res.x*rawinf.res.y];
				gradient[idx * 3 + 1] = (vy1 - vy0) / dy;

				vz0 = data[i + j*rawinf.res.x + z0*rawinf.res.x*rawinf.res.y];
				vz1 = data[i + j*rawinf.res.x + z1*rawinf.res.x*rawinf.res.y];
				gradient[idx * 3 + 2] = (vz1 - vz0) / dz;
				
			}
	return gradient;
}
