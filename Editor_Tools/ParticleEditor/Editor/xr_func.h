#ifndef __XR_FUNC_H__
#define __XR_FUNC_H__

//-----------------------------------------------------------------------------------------------------------
LPSTR							_ChangeFileExt			( LPSTR name, LPSTR ext, LPSTR dest );
LPSTR							_GetFileExt				( LPSTR name, LPSTR ext );
LPSTR							_GetFileName			( LPSTR full, LPSTR name );

// dest = S1+S2
IC char*						strconcat				( char* dest, const char* S1, const char* S2)
{	return strcat(strcpy(dest,S1),S2); }

// dest = S1+S2+S3
IC char*						strconcat				( char* dest, const char* S1, const char* S2, const char* S3)
{	return strcat(strcat(strcpy(dest,S1),S2),S3); }

// dest = S1+S2+S3+S4
IC char*						strconcat				( char* dest, const char* S1, const char* S2, const char* S3, const char* S4)
{	return strcat(strcat(strcat(strcpy(dest,S1),S2),S3),S4); }

// return pointer to ".ext"
IC char*						strext					( const char* S )
{	return strchr(const_cast<char*> (S),'.'); }

extern DWORD subst_a(DWORD val, BYTE a){
    BYTE r, g, b;
    b = (BYTE) (val >>  0);
    g = (BYTE) (val >>  8);
    r = (BYTE) (val >> 16);
    return ((DWORD)(a<<24) | (r<<16) | (g<<8) | (b));
}
DWORD bgr2rgb(DWORD val){
    BYTE r, g, b;
    r = (BYTE) (val >>  0);
    g = (BYTE) (val >>  8);
    b = (BYTE) (val >> 16);
    return ((DWORD)(r<<16) | (g<<8) | (b));
}
DWORD rgb2bgr(DWORD val){
    BYTE r, g, b;
    r = (BYTE) (val >> 16);
    g = (BYTE) (val >>  8);
    b = (BYTE) (val >> 0);
    return ((DWORD)(b<<16) | (g<<8) | (r));
}
#endif //__XR_DEF_H__
