#ifndef OVERLAP_H
#define OVERLAP_H

int tri_contact (float *P1, float *P2, float *P3,
				 float *Q1, float *Q2, float *Q3);

int obb_disjoint(float B[3][3], float T[3], float a[3], float b[3]);

#endif
