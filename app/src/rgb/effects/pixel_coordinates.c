#include "rgb/animations.h"

typedef struct {
	int row;
	int col;
	float x;
	float y;
} pix_coord;

// clang-format off
float positions[50][2] = {
	{ 0, 0.9 }, {1, 0.9}, {2, 0.3}, {3, 0}, {4, 0.3}, {5, 0.4},                                            {11,0.4}, {12,0.3},{13,0},  {14,0.3},{15,0.9},{16,0.9},
	{ 0, 1.9 }, {1, 1.9}, {2, 1.3}, {3, 1}, {4, 1.3}, {5, 1.4},                                            {11,1.4}, {12,1.3},{13,1},  {14,1.3},{15,1.9},{16,1.9},
	{ 0, 2.9 }, {1, 2.9}, {2, 2.3}, {3, 2}, {4, 2.3}, {5, 2.4}, {5, 2.4}, {5, 2.45}, {11, 2.45},{11  ,2.4},{11,2.4}, {12,2.3},{13,2},  {14,2.3},{15,2.9},{16,2.9},
					         {2.4,3.3},{3.4,3.3},{3.4,3.3},{3.4,3.3},{3.4,3.3},      {12.6,3.3},{12.6,3.3},{12.6,3.3},{12.6,3.3},{13.6,3.3},
};


float left_positions[25][2] = {
	{ 0, 0.9 }, {1, 0.9}, {2, 0.3}, {3, 0}, {4, 0.3}, {5, 0.4},
	{ 0, 1.9 }, {1, 1.9}, {2, 1.3}, {3, 1}, {4, 1.3}, {5, 1.4},
	{ 0, 2.9 }, {1, 2.9}, {2, 2.3}, {3, 2}, {4, 2.3}, {5, 2.4}, {5, 2.4}, {5, 2.45},
                                  {2.4,3.3},{3.4,3.3},{3.4,3.3},{3.4,3.3},{3.4,3.3},
};


float right_positions[25][2] = {
	                      {11,0.4}, {12,0.3},{13,0},  {14,0.3},{15,0.9},{16,0.9},
	                      {11,1.4}, {12,1.3},{13,1},  {14,1.3},{15,1.9},{16,1.9},
	{11, 2.45},{11  ,2.4},{11,2.4}, {12,2.3},{13,2},  {14,2.3},{15,2.9},{16,2.9},
	{12.6,3.3},{12.6,3.3},{12.6,3.3},{12.6,3.3},{13.6,3.3},
};


// left
// pix_coord left_30  = { .row = 0, .col =  0, .x =    0, .y =  0.9 };
// pix_coord left_29  = { .row = 0, .col =  1, .x =    1, .y =  0.9 };
// pix_coord left_28  = { .row = 0, .col =  2, .x =    2, .y =  0.3 };
// pix_coord left_27  = { .row = 0, .col =  3, .x =    3, .y =    0 };
// pix_coord left_26  = { .row = 0, .col =  4, .x =    4, .y =  0.3 };
// pix_coord left_25  = { .row = 0, .col =  5, .x =    5, .y =  0.4 };

// pix_coord left_24  = { .row = 1, .col =  0, .x =    0, .y =  1.9 };
// pix_coord left_23  = { .row = 1, .col =  1, .x =    1, .y =  1.9 };
// pix_coord left_22  = { .row = 1, .col =  2, .x =    2, .y =  1.3 };
// pix_coord left_21  = { .row = 1, .col =  3, .x =    3, .y =    1 };
// pix_coord left_20  = { .row = 1, .col =  4, .x =    4, .y =  1.3 };
// pix_coord left_19  = { .row = 1, .col =  5, .x =    5, .y =  1.4 };

// pix_coord left_18  = { .row = 2, .col =  0, .x =    0, .y =  2.9 };
// pix_coord left_17  = { .row = 2, .col =  1, .x =    1, .y =  2.9 };
// pix_coord left_16  = { .row = 2, .col =  2, .x =    2, .y =  2.3 };
// pix_coord left_15  = { .row = 2, .col =  3, .x =    3, .y =    2 };
// pix_coord left_14  = { .row = 2, .col =  4, .x =    4, .y =  2.3 };
// pix_coord left_13  = { .row = 2, .col =  5, .x =    5, .y =  2.4 };
// pix_coord left_12  = { .row = 2, .col =  6, .x =    5, .y =  2.4 };
// pix_coord left_11  = { .row = 2, .col =  7, .x =    5, .y = 2.45 };

// pix_coord left_10  = { .row = 3, .col =  3, .x =  2.4, .y =  3.3 };
// pix_coord left_09  = { .row = 3, .col =  4, .x =  3.4, .y =  3.3 };
// pix_coord left_08  = { .row = 3, .col =  5, .x =  3.4, .y =  3.3 }; // y should be higher as well as x
// pix_coord left_07  = { .row = 3, .col =  6, .x =  3.4, .y =  3.3 };
// pix_coord left_06  = { .row = 3, .col =  7, .x =  3.4, .y =  3.3 };

// // right
// pix_coord right_25 = { .row = 0, .col = 10, .x =   11, .y =  0.4 };
// pix_coord right_26 = { .row = 0, .col = 11, .x =   12, .y =  0.3 };
// pix_coord right_27 = { .row = 0, .col = 12, .x =   13, .y =    0 };
// pix_coord right_28 = { .row = 0, .col = 13, .x =   14, .y =  0.3 };
// pix_coord right_29 = { .row = 0, .col = 14, .x =   15, .y =  0.9 };
// pix_coord right_30 = { .row = 0, .col = 15, .x =   16, .y =  0.9 };

// pix_coord right_19 = { .row = 1, .col = 10, .x =   11, .y =  1.4 };
// pix_coord right_20 = { .row = 1, .col = 11, .x =   12, .y =  1.3 };
// pix_coord right_21 = { .row = 1, .col = 12, .x =   13, .y =    1 };
// pix_coord right_22 = { .row = 1, .col = 13, .x =   14, .y =  1.3 };
// pix_coord right_23 = { .row = 1, .col = 14, .x =   15, .y =  1.9 };
// pix_coord right_24 = { .row = 1, .col = 15, .x =   16, .y =  1.9 };

// pix_coord right_11 = { .row = 2, .col =  8, .x =   11, .y = 2.45 };
// pix_coord right_12 = { .row = 2, .col =  9, .x =   11, .y =  2.4 };
// pix_coord right_13 = { .row = 2, .col = 10, .x =   11, .y =  2.4 };
// pix_coord right_14 = { .row = 2, .col = 11, .x =   12, .y =  2.3 };
// pix_coord right_15 = { .row = 2, .col = 12, .x =   13, .y =    2 };
// pix_coord right_16 = { .row = 2, .col = 13, .x =   14, .y =  2.3 };
// pix_coord right_17 = { .row = 2, .col = 14, .x =   15, .y =  2.9 };
// pix_coord right_18 = { .row = 2, .col = 15, .x =   16, .y =  2.9 };

// pix_coord right_06 = { .row = 3, .col =  8, .x = 12.6, .y =  3.3 };
// pix_coord right_07 = { .row = 3, .col =  9, .x = 12.6, .y =  3.3 };
// pix_coord right_08 = { .row = 3, .col = 10, .x = 12.6, .y =  3.3 };
// pix_coord right_09 = { .row = 3, .col = 11, .x = 12.6, .y =  3.3 };
// pix_coord right_10 = { .row = 3, .col = 12, .x = 13.6, .y =  3.3 };

// clang-format on
