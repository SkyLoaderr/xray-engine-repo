#include <windows.h>
#include <stdio.h>
#pragma hdrstop
#include "config.h"

#define vsCreateFile    CreateFile
#define vsWriteFile     WriteFile
#define vsReadFile      ReadFile
#define vsCloseHandle   CloseHandle
#define vsGetFileSize   GetFileSize

//---------------------------------------------------------------------------------------
vsConfig::vsConfig ()
{
    *m_pFileName = 0;
    m_pStrings = NULL;
}
//---------------------------------------------------------------------------------------
vsConfig::~vsConfig ()
{
    Clear ();
    SAFE_DELETE (m_pStrings);
}
//---------------------------------------------------------------------------------------
void vsConfig::Clear ()
{
    if (!m_pStrings) return;
    list<LPSTR>::iterator i;
    for (i = m_pStrings->begin (); i != m_pStrings->end (); ++i)
        {
        LPSTR str = *i;
        if (str) delete [] str;
        }
    m_pStrings->clear ();
}
//---------------------------------------------------------------------------------------
HRESULT vsConfig::LoadFromFile (LPCSTR lpFileName)
{
    if (!m_pStrings) m_pStrings = new list<LPSTR> ();
    else Clear ();

    HANDLE hTest = vsCreateFile (lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    vsCloseHandle (hTest);
    if (hTest == INVALID_HANDLE_VALUE)
       if (FAILED (this->Create (lpFileName)))
          return E_FAIL;

    HANDLE hFile = vsCreateFile (lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return E_FAIL;
    int iSize = vsGetFileSize (hFile, NULL);
    char *pBuffer = NULL;
    if (!iSize || (pBuffer = new char [iSize + 1]) == NULL)
       {
       vsCloseHandle (hFile);
       return E_FAIL;
       }
    DWORD dwReaded;
    if (vsReadFile (hFile, pBuffer, iSize, &dwReaded, NULL) == FALSE || dwReaded != (DWORD)iSize)
       {
       delete [] pBuffer;
       vsCloseHandle (hFile);
       return E_FAIL;
       }
    vsCloseHandle (hFile);

    pBuffer[iSize] = 0;

    char *str = pBuffer;
    while (str)
          {
          char *endl = GetEndString (str);
          char old = *endl;
          *endl = 0;
          int ilen = (int)strlen (str);
          char *string = new char [ilen + 1];
          if (!string)
             {
             delete [] pBuffer;
             return E_FAIL;
             }
          strcpy (string, str);
          strlwr (string);
          if (*string == '[')
             if (NormalizeSection (string) == FALSE)
                {
                delete [] pBuffer;
                return E_FAIL;
                }
          m_pStrings->push_back (string);
          *endl = old;
          str = GetNextString (endl);
          }
    delete [] pBuffer;
    strcpy (m_pFileName, lpFileName);
    return S_OK;
}
//---------------------------------------------------------------------------------------
HRESULT vsConfig::Create    (LPCSTR lpFileName)
{
    if (!lpFileName || !*lpFileName) return E_FAIL;
    HANDLE hFile = vsCreateFile (lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return E_FAIL;
    char buf[4] = "\n";
    DWORD dwWritten;
    if (vsWriteFile (hFile, buf, (DWORD)strlen (buf), &dwWritten, NULL) == FALSE ||
        dwWritten != (DWORD)strlen (buf))
       {
       DeleteFile (lpFileName);
       vsCloseHandle (hFile);
       return E_FAIL;
       }
    vsCloseHandle (hFile);
    return LoadFromFile (lpFileName);
}
//---------------------------------------------------------------------------------------
BOOL vsConfig::NormalizeSection (char *str)
{
    str = strchr (str, ']');
    if (!str) return FALSE;
    str++;
    if (*str) *str = 0;
    return TRUE;
}
//---------------------------------------------------------------------------------------
char* vsConfig::GetNextString (char *str)
{
    if (!str || !*str) return NULL;
    while (*str == 0xd || *str == 0xa)
          {
          str++;
          if (!*str) return NULL;
          }
    return str;
}
//---------------------------------------------------------------------------------------
char* vsConfig::GetEndString (char *str)
{
    if (!str || !*str) return NULL;
    int ilen = (int)strlen (str);
    while (ilen > 0)
          {
          if (*str == 0xd || *str == 0xa || !*str)
             return str;
          ilen--;
          str++;
          }
    return str + ilen;
}
//---------------------------------------------------------------------------------------
HRESULT vsConfig::Save (LPCSTR lpFileName)
{
    if (!m_pStrings || m_pStrings->size () == 0) return S_OK;
    char pFileName[256];
    if (!lpFileName) strcpy (pFileName, m_pFileName);
    else strcpy (pFileName, lpFileName);
    HANDLE hFile = vsCreateFile (pFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return E_FAIL;
    char eol[2] = {0xd, 0xa};
    list<LPSTR>::iterator i;
    for (i = m_pStrings->begin (); i != m_pStrings->end (); ++i)
        {
        LPSTR str = *i;
        DWORD dwWritten;
        if (*str == '[' && i != m_pStrings->begin ()) vsWriteFile (hFile, eol, 2, &dwWritten, NULL);
        if (vsWriteFile (hFile, str, (DWORD)strlen (str), &dwWritten, NULL) == FALSE ||
            vsWriteFile (hFile, eol, 2, &dwWritten, NULL) == FALSE)
           {
           vsCloseHandle (hFile);
           DeleteFile (pFileName);
           return E_FAIL;
           }
        }
    vsCloseHandle (hFile);
    strcpy (m_pFileName, pFileName);
    return S_OK;
}
//---------------------------------------------------------------------------------------
void vsConfig::GetString (LPCSTR pSection, LPCSTR pParam,  LPSTR pValue, LPCSTR pDefValue)
{
    if (!pSection || !pParam || !pValue) return;
    if (!m_pStrings)
       {
       if (pDefValue) strcpy (pValue, pDefValue);
       return;
       }
    char *str = FindParam (FindSection (pSection), pParam);
    if (!str)
       {
       if (pDefValue) strcpy (pValue, pDefValue);
       SetString (pSection, pParam, pValue);
       return;
       }
    char param[64], value[256];
    SplitParamValue (str, param, value);
    if (!*value)
       {
       if (pDefValue) strcpy (pValue, pDefValue);
       return;
       }
    strcpy (pValue, value);
}
//---------------------------------------------------------------------------------------
int vsConfig::GetInt (LPCSTR pSection, LPCSTR pParam, int iDefault)
{
    char value[256];
    GetString (pSection, pParam, value, "");
    if (!*value) return iDefault;
    return atoi (value);
}
//---------------------------------------------------------------------------------------
float vsConfig::GetFloat (LPCSTR pSection, LPCSTR pParam, float fDefault)
{
    char value[256];
    GetString (pSection, pParam, value, "");
    if (!*value) return fDefault;
    return (float)atof (value);
}
//---------------------------------------------------------------------------------------
BYTE vsConfig::GetByte (LPCSTR pSection, LPCSTR pParam, BYTE bDefault)
{
    return (BYTE)GetInt (pSection, pParam, (int)bDefault);
}
//---------------------------------------------------------------------------------------
int vsConfig::GetHex (LPCSTR pSection, LPCSTR pParam, int iDefault)
{
    char value[256];
    GetString (pSection, pParam, value, "");
    if (!*value) return iDefault;
    int ivalue;
    if (sscanf (value, "%x", &ivalue) == 0) return iDefault;
    return ivalue;
}
//---------------------------------------------------------------------------------------
HRESULT vsConfig::SetString (LPCSTR pSection, LPCSTR pParam, LPCSTR pValue)
{
    if (!m_pStrings) m_pStrings = new list<LPSTR> ();
    if (!m_pStrings) return E_FAIL;
    if (!pSection || !*pSection || !pParam || !*pParam || !pValue || !*pValue) return S_OK;


    list<LPSTR>::iterator isection = FindSection (pSection);
    if (isection == m_pStrings->end ())
       {
       char *str = new char [64];
       if (!str) return E_FAIL;
       sprintf (str, "[%s]", pSection);
       m_pStrings->push_back (str);
       isection = FindSection (pSection);
       if (isection == NULL) return E_FAIL;
       str = new char [strlen (pParam) + strlen (pValue) + 8];
       if (!str) return E_FAIL;
       sprintf (str, "%s = %s", pParam, pValue);
       m_pStrings->insert (++isection, str);
       return S_OK;
       }

    list<LPSTR>::iterator iparam = FindParamI (isection, pParam);

    char *str = new char [256];
    if (!str) return E_FAIL;
    sprintf (str, "%s = %s", pParam, pValue);

    if (iparam == m_pStrings->end ())
       m_pStrings->insert (++isection, str);
    else
       {
       char *comment = GetParamComment (*iparam);
       if (comment)
          {
          char buf[128];
          sprintf (buf, "\t\t\t%s", comment);
          strcat (str, buf);
          }
       delete [] (*iparam);   
       *iparam = str;
       }
    return S_OK;
}
//---------------------------------------------------------------------------------------
HRESULT vsConfig::SetInt (LPCSTR pSection, LPCSTR pParam, int iValue)
{
    char param[64];
    sprintf (param, "%d", iValue);
    return SetString (pSection, pParam, param);
}
//---------------------------------------------------------------------------------------
HRESULT vsConfig::SetFloat (LPCSTR pSection, LPCSTR pParam, float fValue)
{
    char value[64];
    sprintf (value, "%f", fValue);
    return SetString (pSection, pParam, value);
}
//---------------------------------------------------------------------------------------
HRESULT vsConfig::SetByte (LPCSTR pSection, LPCSTR pParam, BYTE cValue)
{
    char value[64];
    sprintf (value, "%d", cValue);
    return SetString (pSection, pParam, value);
}
//---------------------------------------------------------------------------------------
HRESULT vsConfig::SetHex (LPCSTR pSection, LPCSTR pParam, int iValue)
{
    char value[64];
    sprintf (value, "%x", iValue);
    return SetString (pSection, pParam, value);
}
//---------------------------------------------------------------------------------------
list<LPSTR>::iterator vsConfig::FindSection (LPCSTR str)
{
    char pSection[64] = "[";
    strcat (pSection, str);
    strcat (pSection, "]");
    list<LPSTR>::iterator i;
    for (i = m_pStrings->begin (); i != m_pStrings->end (); ++i)
        if (!stricmp (pSection, *i)) return i;
    return m_pStrings->end ();
}
//---------------------------------------------------------------------------------------
LPSTR vsConfig::FindParam (list<LPSTR>::iterator i, LPCSTR pParam)
{
    if (!pParam || i == m_pStrings->end ()) return NULL;
    char param[64];
    strcpy (param, pParam);
    strlwr (param);
    ++i;
    for (list<LPSTR>::iterator a = i; a != m_pStrings->end (); ++a)
        {
        LPSTR str = *a;
        if (*str == '[') return NULL;
        char buf[64], *eol = strchr (str, 0x20);
        if (!eol) continue;
        int ilen = (int)(eol - str);
        memcpy (buf, str, ilen);
        buf[ilen] = 0;
        if (!stricmp (pParam, buf)) return str;
        }
    return NULL;
}
//---------------------------------------------------------------------------------------
list<LPSTR>::iterator vsConfig::FindParamI (list<LPSTR>::iterator i, LPCSTR pParam)
{
    if (!pParam || i == m_pStrings->end ()) return NULL;
    char param[64];
    strcpy (param, pParam);
    strlwr (param);
    ++i;
    for (list<LPSTR>::iterator a = i; a != m_pStrings->end (); ++a)
        {
        LPSTR str = *a;
        if (*str == '[') return m_pStrings->end ();
        char buf[64], *eol = strchr (str, 0x20);
        if (!eol) continue;
        int ilen = (int)(eol - str);
        memcpy (buf, str, ilen);
        buf[ilen] = 0;
        if (!stricmp (pParam, buf)) return a;
        }
    return m_pStrings->end ();

}
//---------------------------------------------------------------------------------------
char* vsConfig::GetParamComment (LPSTR lpString)
{
    if (!lpString) return NULL;
    return strchr (lpString, ';');
}
//---------------------------------------------------------------------------------------
int __isspace (int c)
{
    if ((c >= 0x09 && c <= 0x0d) || c == 0x20) return 1;
    return 0;
}
//---------------------------------------------------------------------------------------
void vsConfig::SplitParamValue (LPSTR pString, LPSTR pParam, LPSTR pValue)
{
    int  i, j;
    // skip forward spaces
    for (i = 0; pString[i] && __isspace (pString[i]); i++);
    // copy param until '=' or space
    j = 0;
    for (; pString[i] && pString[i]!= '=' && !__isspace (pString[i]); i++)
        pParam[j++] = pString[i];
    pParam[j--] = '\0';
    // remove end spaces and '='
    for (; j >= 0 && (__isspace (pParam[j]) || pParam[j]== '='); j--) pParam[j]  = '\0';
    // skip forward spaces
    i++;
    for (; pString[i] && (__isspace (pString[i]) || pString[i]== '='); i++);
    // skip forward brakes
    while (pString[i] && pString[i] == '"')  i++;
    // copy value
    j = 0;
    for (; pString[i] && pString[i] !=';'; i++) pValue[j++]  = pString[i];
    pValue[j--] = '\0';
    // remove end spaces
    for (; j >= 0 && __isspace (pValue[j]); j--) pValue[j]  = '\0';
    // remove brackets
    for (; j>=0 && pValue[j]=='"'; j-- ) pValue[j]  = '\0';
}
//---------------------------------------------------------------------------------------
bool                vsConfig::IsSectionExist      (LPCSTR lpSection)
{
    list<LPSTR>::iterator i = FindSection (lpSection);
    return i != m_pStrings->end ();
}
//---------------------------------------------------------------------------------------
