#ifndef _XRIMAGE_RESAMPLER_H_
#define _XRIMAGE_RESAMPLER_H_

#pragma once

enum	EIMF_Type
{
	imf_filter=0,
	imf_box,
	imf_triangle,
	imf_bell,
	imf_b_spline,
	imf_lanczos3,
	imf_mitchell,

	imf_FORCEDWORD=0xffffffff
};

ENGINE_API	extern	void imf_Process(LPDWORD dst, DWORD dstW, DWORD dstH, LPDWORD src, DWORD srcW, DWORD srcH, EIMF_Type FILTER);
#endif