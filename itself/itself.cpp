#include "stdafx.h"

#define X void main(int a,char** b) { printf("#define X %s",#X); }
/*
#define Y(a) char* str = #a;

Y(aaaaaaaaaaaaaa)
*/
X
