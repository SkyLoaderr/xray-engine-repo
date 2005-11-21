#pragma once

#include <list> 

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a) \
    if ((a)) delete [] (a); a = NULL;
#endif

#define MAXINISTRINGS   512

using namespace std;

class vsConfig
{
private:
protected:
    char                m_pFileName[256];
    list<LPSTR>        *m_pStrings;
    void                Clear ();
    char*               GetEndString (char *str);
    char*               GetNextString (char *str);
    list<LPSTR>::iterator FindSection (LPCSTR str);
    list<LPSTR>::iterator FindParamI (list<LPSTR>::iterator i, LPCSTR str);
    BOOL                NormalizeSection (char *str);
    LPSTR               FindParam (list<LPSTR>::iterator i, LPCSTR pParam);
    void                SplitParamValue (LPSTR pString, LPSTR pParam, LPSTR pValue);
    char*               GetParamComment (LPSTR lpString);
public:
                        vsConfig            ();
                       ~vsConfig            ();
    HRESULT             LoadFromFile        (LPCSTR lpFileName);
    HRESULT             Save                (LPCSTR lpFileName = NULL);
    HRESULT             Create              (LPCSTR lpFileName);
    void                GetString           (LPCSTR pSection, LPCSTR pParam, LPSTR pValue, LPCSTR pDefValue = NULL);
    int                 GetInt              (LPCSTR pSection, LPCSTR pParam, int iDefault);
    float               GetFloat            (LPCSTR pSection, LPCSTR pParam, float fDefault);
    BYTE                GetByte             (LPCSTR pSection, LPCSTR pParam, BYTE bDefault);
    int                 GetHex              (LPCSTR pSection, LPCSTR pParam, int iDefault);
    HRESULT             SetString           (LPCSTR pSection, LPCSTR pParam, LPCSTR pValue);
    HRESULT             SetInt              (LPCSTR pSection, LPCSTR pParam, int iValue);
    HRESULT             SetFloat            (LPCSTR pSection, LPCSTR pParam, float fValue);
    HRESULT             SetByte             (LPCSTR pSection, LPCSTR pParam, BYTE cValue);
    HRESULT             SetHex              (LPCSTR pSection, LPCSTR pParam, int iValue);
    LPCSTR              GetFileName         () { return m_pFileName; }
    bool                IsSectionExist      (LPCSTR lpSection);
};

