#pragma	once

typedef float REAL;

extern void vfOptimizeParameters(vector<vector<REAL> > &A, vector<vector<REAL> > &B, vector<REAL> &C, vector<REAL> &D, REAL dEpsilon = REAL(0.001), REAL dAlpha = REAL(1.0), REAL dBeta = REAL(0.01), u32 dwMaxIterationCount = 10000);
