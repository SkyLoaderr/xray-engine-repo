#pragma	once

typedef float REAL;

extern void vfOptimizeParameters(xr_vector<xr_vector<REAL> > &A, xr_vector<xr_vector<REAL> > &B, xr_vector<REAL> &C, xr_vector<REAL> &D, REAL dEpsilon = REAL(0.001), REAL dAlpha = REAL(1.0), REAL dBeta = REAL(0.01), REAL dNormaFactor = REAL(100.0), u32 dwMaxIterationCount = 10000);
