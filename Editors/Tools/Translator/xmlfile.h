#ifndef _XMLFILE_INCLUDED_
#define _XMLFILE_INCLUDED_

#include "tunyxml\tinyxml.h"
#include <map>
#include <vector>
#include <string>
#include "xmlstring.h"

using namespace std;

//typedef map<string, string>             XMLString;
//typedef map<string, string>::iterator   XMLStringI;

typedef vector<string>                              include_list;
typedef vector<string>::iterator                    include_list_i;

typedef map<std::string, TXMLString*>               xml_string;
typedef map<std::string, TXMLString*>::iterator     xml_string_i;

typedef vector<string>                              xml_string_id;
typedef vector<string>::iterator                    xml_string_id_i;

typedef map<string, xml_string_id>                  xml_diff_strings;
typedef map<string, xml_string_id>::iterator        xml_diff_strings_i;


class CXMLFile
{
private:
protected:
    string                          m_Name;
    string                          m_LockName;
    string                          m_ShortName;
    include_list                    m_Includes;
    xml_string                      m_Strings;
    bool                            m_bWithoutHeader;
    void                            LoadFile    ();
    bool                            remove_header ();
    HANDLE                          m_Lock;
public:
    bool                            m_bChanged;

                CXMLFile            () { m_Lock = INVALID_HANDLE_VALUE; }
                CXMLFile            (string name);
               ~CXMLFile            ();
    string      GetName             () { return m_ShortName; }
    string      GetFullName         () { return m_Name; }
    TXMLString* get_string          (string id);
    void        save                (FILE* hFile);
    void        load                (FILE* hFile);
    CXMLFile*   get_copy            ();
    void        add_different_str   (CXMLFile *import, xml_diff_strings *list);
    void        fill_list           (TListBox *box);
    void        add_string          (string strid, string text);
    void        get_deff_str_list   (CXMLFile *in, vector<string> *list);
    void        export_data         ();
    void        set_name            (string name);
    void        get_statistic       (int *strings, int *words);
    void        get_string_list     (vector<string> *list);
    bool        is_string_present   (string strid);
    void        remove_string       (string strid);
    void        get_notrans_strings (vector<string> *list);
    void        dump                (FILE *file);
    bool        locked              ();
    bool        lock                ();
    void        unlock              ();
    void        unlock_w_save       ();
    string      get_owner           ();
};

#endif /*_XMLFILE_INCLUDED_*/
