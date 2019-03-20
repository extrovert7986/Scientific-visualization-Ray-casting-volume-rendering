#pragma once

struct res_t {
	int x;
	int y;
	int z;
};

struct ratio_t {
	float x;
	float y;
	float z;
};

enum type_t { UNSIGNED_CHAR_T, UNSIGNED_SHORT_T, FLOAT_T };

enum endian_t { BIG, LITTLE };

struct rawInf_t {
	char fileName[200];
	res_t res;
	ratio_t ratio;
	type_t smplTyp;
	endian_t endian;
};