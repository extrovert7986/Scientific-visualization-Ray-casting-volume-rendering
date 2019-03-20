#pragma once

unsigned char cube[] = {
	0, 0, 0,
	1, 0, 0,
	1, 0, 1,
	0, 0, 1,
	0, 1, 0,
	1, 1, 0,
	1, 1, 1,
	0, 1, 1
};

unsigned char tri[12][3] = {
	{ 0,1,2 },
	{ 0,2,3 },
	{ 1,5,6 },
	{ 1,6,2 },
	{ 2,6,7 },
	{ 2,7,3 },
	{ 3,7,4 },
	{ 3,4,0 },
	{ 0,4,5 },
	{ 0,5,1 },
	{ 4,7,6 },
	{ 4,6,5 }
};

unsigned char bound[12][2] = {
	{ 0,1 },
	{ 1,2 },
	{ 2,3 },
	{ 3,0 },
	{ 4,5 },
	{ 5,6 },
	{ 6,7 },
	{ 7,4 },
	{ 0,4 },
	{ 1,5 },
	{ 2,6 },
	{ 3,7 }
};