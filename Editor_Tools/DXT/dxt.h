#ifndef __DXT_H__
#define __DXT_H__

extern "C" DLL_API 
bool DXTCompress(const char* out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch, 
				 CompressionOptions * options, DWORD depth, MIPcallback callback = 0);


#endif