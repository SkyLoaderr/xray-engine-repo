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

#endif //__XR_DEF_H__
