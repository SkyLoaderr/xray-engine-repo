//---------------------------------------------------------------------------

#ifndef xmlstringH
#define xmlstringH
//---------------------------------------------------------------------------
#include <string>
#include <stdio.h>

using namespace std;

class TXMLString
{
protected:
    string              m_String;
    string              m_Id;
    char*               trim_space  (char *ptr);
public:
    bool                m_bChanged;
    SYSTEMTIME          m_LastChanged;
                        TXMLString      () {};
                        TXMLString      (string id);
    string              get_string      () { return m_String; }
    string              get_id          () { return m_Id; }
    void                set_string      (string str);
    bool                save            (FILE *file);
    bool                load            (FILE *file);
    TXMLString*         get_copy        ();
    int                 get_words       ();
    TXMLString*         create_new      ();
};


#endif
