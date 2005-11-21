//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "xmlstring.h"

using namespace std;

std::string _replace (std::string strin, std::string strfind, std::string strrep)
{
    char buf[16384], *out = buf;
    const char *src = strin.c_str (), *find = strfind.c_str (), *rep = strrep.c_str ();
    while (src)
          {
          char *founded = strstr ((char*)src, (char*)find);
          if (!founded)
             {
             strcpy (out, src);
             break;
             }
          int ilen = (int)(founded - src);
          CopyMemory (out, src, ilen);
          out += ilen;
          *out = 0;
          strcat (out, rep);
          out += strlen (rep);
          src = founded + strlen (find);
          }
    return std::string (buf);
}

//---------------------------------------------------------------------------

#pragma package(smart_init)


//---------------------------------------------------------------------------
                    TXMLString::TXMLString      (string id)
{
    m_Id = id;
}
//---------------------------------------------------------------------------
void                TXMLString::set_string      (string str)
{
    m_String = str;
    GetLocalTime (&m_LastChanged);
    m_bChanged = true;
}
//---------------------------------------------------------------------------
bool                TXMLString::save            (FILE *file)
{
    fwrite (&m_LastChanged, sizeof (SYSTEMTIME), 1, file);
    size_t len = m_Id.size ();
    fwrite (&len, sizeof (size_t), 1, file);
    fwrite (m_Id.c_str (), len, 1, file);
    len = m_String.size ();
    fwrite (&len, sizeof (size_t), 1, file);
    fwrite (m_String.c_str (), len, 1, file);
    return true;
}
//---------------------------------------------------------------------------
bool                TXMLString::load            (FILE *file)
{
    fread (&m_LastChanged, sizeof (SYSTEMTIME), 1, file);
    size_t len = 0;
    fread (&len, sizeof (size_t), 1, file);
    char buf[16384];
    fread (buf, len, 1, file);
    buf[len] = 0;
    m_Id = buf;
    fread (&len, sizeof (size_t), 1, file);
    fread (buf, len, 1, file);
    if (len > 16384)
       len = len; 
    buf[len] = 0;
    m_String = buf;
    return true;
}
//---------------------------------------------------------------------------
TXMLString*         TXMLString::get_copy        ()
{
    TXMLString* str = new TXMLString ();
    str->m_String = m_String;
    str->m_Id = m_Id;
    str->m_bChanged = m_bChanged;
    str->m_LastChanged = m_LastChanged;
    return str;
}
//---------------------------------------------------------------------------
TXMLString*         TXMLString::create_new      ()
{
    TXMLString *str = new TXMLString ();
    str->m_String = "This string was added automatically";
    str->m_Id = m_Id;
    str->m_bChanged = m_bChanged;
    str->m_LastChanged = m_LastChanged;
    return str;
}
//---------------------------------------------------------------------------
int                 TXMLString::get_words       ()
{
    if (m_String.size() == 0) return 0;
    string str = _replace (m_String, "\n", " ");
    str = _replace (m_String, "\t", " ");

    char *ptr = (char*) str.c_str ();

    int count = 0;
    while (true)
          {
          char *found = strstr (ptr, " ");
          if (!found)
             {
             count++;
             break;
             }
          if ((int)(found - ptr) >= 2)
             count++;
          ptr = trim_space (found);
          if (!ptr) break;
          }

    return count;
}
//---------------------------------------------------------------------------
char*               TXMLString::trim_space  (char *ptr)
{
    while (true)
          {
          if (!*ptr) return NULL;
          if (*ptr != 0x20 && *ptr != 0x9 && *ptr != '\n')
             return ptr;
          ptr++;
          }
    return ptr;          
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

