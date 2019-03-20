#pragma once
#include"RawInfoType.h"

rawInf_t read_vol(const char* fileName);
float* read_raw(const rawInf_t rawinf);
float* compute_gradient(const float* data, const rawInf_t rawinf);