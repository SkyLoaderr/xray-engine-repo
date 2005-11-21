#include <vcl.h>
#pragma hdrstop
#include "xmlfile.h"

#define VERIFY(x) {}


//-------------------------------------------------------------------------------------
            CXMLFile::CXMLFile            (string name)
{
    m_Name = name;
    char drv[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT], final[512];
    _splitpath (m_Name.c_str (), drv, dir, fname, ext);
    m_ShortName = fname;
    m_LockName = string (drv) + string (dir) + string (fname) + string (".desc");

    m_bWithoutHeader = false;
    SetFileAttributes (name.c_str (), GetFileAttributes (name.c_str ()) & ~FILE_ATTRIBUTE_READONLY);
    //load files
    LoadFile ();

    if (m_Strings.size () == 0)
       {
       char buf[512];
       GetTempPath (512, buf);
       strcat (buf, "temp.xml");
       FILE *fileout = fopen (buf, "wt");
       VERIFY (fileout);
       FILE *filein = fopen (m_Name.c_str (), "rt");
       fputs ("<?xml version=\"1.0\" encoding=\"windows-1251\" ?>\n", fileout);
       fputs ("<string_table>\n", fileout);
       char tempstr[16384];
       while (fgets (tempstr, 16384, filein))
             fputs (tempstr, fileout);
       fputs ("</string_table>", fileout);
       fclose (filein);
       fclose (fileout);
       string tempname = m_Name;
       m_Name = buf;
       LoadFile ();
       m_Name = tempname;
       DeleteFile (buf);
       m_bWithoutHeader = true;
       } 
    
    FILE *file = fopen (m_Name.c_str (), "rt");
    VERIFY (file);
    char buf[512];
    ZeroMemory (buf, 512);
    while (fgets (buf, 512, file))
          if (*buf == '#')
             {
             char *ptr = strstr (buf, "\n");
             if (ptr) *ptr = 0;
             m_Includes.push_back (buf);
             ZeroMemory (buf, 512);
             }
    fclose (file);
    m_bChanged = false;
    m_Lock = INVALID_HANDLE_VALUE;
}
//-------------------------------------------------------------------------------------
void        CXMLFile::LoadFile              ()
{
    TiXmlDocument doc (m_Name.c_str ());
    doc.LoadFile ();
    TiXmlElement* root = doc.FirstChildElement ("string_table");
    if (!root) return;
    TiXmlElement *data = root->FirstChildElement ("string");
    while (data)
          {
          if (data->Type () == TiXmlNode::ELEMENT)
             {
             string idattrib = data->Attribute ("id");
             VERIFY (idattrib.empty () == false);
             TiXmlElement *text = (TiXmlElement*)data->FirstChild ("text");
             VERIFY (text);
             TiXmlText *ct = (TiXmlText*)text->FirstChild ();
             string textdata;
             if (ct)
                textdata = ct->Value ();
             else
                textdata = "empty";
             TXMLString* string = new TXMLString (idattrib);
             if (!string) throw Exception ("Error allocate memory");
             string->set_string (textdata);
             m_Strings[idattrib] = string;
             }
          data = (TiXmlElement*)data->NextSibling ();
          }
}
//-------------------------------------------------------------------------------------
            CXMLFile::~CXMLFile           ()
{
    unlock ();
    xml_string_i s = m_Strings.begin (), e = m_Strings.end ();
    for (; s != e; ++s)
        {
        TXMLString *str = (*s).second;
        if (str) delete str;
        }

    if (m_Name != "")
       SetFileAttributes (m_Name.c_str (), GetFileAttributes (m_Name.c_str ()) | FILE_ATTRIBUTE_READONLY);
}
//-------------------------------------------------------------------------------------
TXMLString* CXMLFile::get_string          (string id)
{
    TXMLString *str = m_Strings[id];
    return str;
}
//-------------------------------------------------------------------------------------
void        CXMLFile::save                (FILE* hFile)
{
    //store file name
    int pos;
    pos = ftell (hFile);                      //4
    size_t temp_size = m_Name.size ();
    fwrite (&temp_size, sizeof (size_t), 1, hFile);
    fwrite (m_Name.c_str (), temp_size, 1, hFile);

    //store header flag
    pos = ftell (hFile);                         //48
    fwrite (&m_bWithoutHeader, sizeof (bool), 1, hFile);

    //store includes list
    pos = ftell (hFile);                               //49
    temp_size = m_Includes.size ();
    fwrite (&temp_size, sizeof (size_t), 1, hFile);
    if (temp_size)
       {
       include_list_i is = m_Includes.begin (), ie = m_Includes.end ();
       for (; is != ie; ++is)
           {
           temp_size = (*is).size ();
           fwrite (&temp_size, sizeof (size_t), 1, hFile);
           fwrite ((*is).c_str (), temp_size, 1, hFile);
           }
       }
    //store strings list
    pos = ftell (hFile);                        //53
    temp_size = m_Strings.size ();
    fwrite (&temp_size, sizeof (size_t), 1, hFile);
    if (!temp_size) return;
    xml_string_i s = m_Strings.begin (), e = m_Strings.end ();
    for (; s != e; ++s)
        {
        TXMLString* str = (*s).second;
        str->save (hFile);
        }
}
//-------------------------------------------------------------------------------------
void        CXMLFile::load                (FILE* hFile)
{
    char buf[16384];
    size_t temp_size;
    //load file name
    int pos;
    pos = ftell (hFile);             //4
    fread (&temp_size, sizeof (size_t), 1, hFile);
    if (!temp_size) throw Exception ("Invalid database file");
    fread (buf, temp_size, 1, hFile);
    buf[temp_size] = 0;
    m_Name = buf;
    char drv[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT], final[512];
    _splitpath (m_Name.c_str (), drv, dir, fname, ext);
    m_ShortName = fname;
    m_LockName = string (drv) + string (dir) + string (fname) + string (".desc");
    //load header flag
    pos = ftell (hFile);            //48
    fread (&m_bWithoutHeader, sizeof (bool), 1, hFile);
    //load includes list
    pos = ftell (hFile);                   //49
    fread (&temp_size, sizeof (size_t), 1, hFile);
    if (temp_size)
       {
       for (size_t a = 0; a < temp_size; a++)
           {
           size_t strsize;
           fread (&strsize, sizeof (size_t), 1, hFile);
           fread (buf, strsize, 1, hFile);
           buf[strsize] = 0;
           m_Includes.push_back (buf);
           }
       }
    //read string list
    temp_size = 0;
    pos = ftell (hFile);                       //53
    fread (&temp_size, sizeof (size_t), 1, hFile);
    if (!temp_size) return;
    for (size_t a = 0; a < temp_size; a++)
        {
        TXMLString *str = new TXMLString ();
        if (!str) throw Exception ("Error allocate memory");
        str->load (hFile);
        m_Strings[str->get_id ()] = str;
        }
}
//-------------------------------------------------------------------------------------
CXMLFile*   CXMLFile::get_copy            ()
{
    CXMLFile *file = new CXMLFile ();
    file->m_Name = m_Name;
    file->m_ShortName = m_ShortName;
    file->m_Includes.assign (m_Includes.begin (), m_Includes.end());
    file->m_bWithoutHeader = m_bWithoutHeader;
    xml_string_i s = m_Strings.begin(), e = m_Strings.end ();
    for (; s != e; ++s)
        file->m_Strings[(*s).first] = (*s).second->create_new ();//get_copy();
    return file;
}
//-------------------------------------------------------------------------------------
void        CXMLFile::add_different_str   (CXMLFile *import, xml_diff_strings *list)
{
    xml_string_id strids;
    xml_string_i s = import->m_Strings.begin (), e = import->m_Strings.end ();
    for (; s != e; ++s)
        {
        if (!m_Strings[(*s).first])
           {
           strids.push_back ((*s).first);
           TXMLString *str = new TXMLString ((*s).first);
           if (!str) throw Exception ("Error allocate memory");
           str->set_string ("This string was added automatically");
           m_Strings[(*s).first] = str;
//           m_Strings[(*s).first] = (*s).second->get_copy();
           }
        }
    if (list && strids.size ())
       (*list)[m_ShortName] = strids;
}
//-------------------------------------------------------------------------------------
void        CXMLFile::fill_list           (TListBox *box)
{
    xml_string_i s = m_Strings.begin (), e = m_Strings.end ();
    for (; s != e; ++s)
        box->Items->Add ((AnsiString)(*s).second->get_id ().c_str ());
}
//-------------------------------------------------------------------------------------
void        CXMLFile::add_string          (string strid, string text)
{
    TXMLString *str = new TXMLString (strid);
    if (!str) throw Exception ("Error allocate memory");
    str->set_string(text);
    m_Strings[strid] = str;
}
//-------------------------------------------------------------------------------------
void        CXMLFile::get_deff_str_list   (CXMLFile *infile, vector<string> *list)
{
    xml_string_i s = m_Strings.begin (), e = m_Strings.end ();
    for (; s != e; ++s)
        {
        string str1 = (*s).first;
        TXMLString *in = m_Strings[(*s).first];
        TXMLString *out = infile->m_Strings[(*s).first];
        if (!out)
           {
           infile->m_Strings[(*s).first] = m_Strings[(*s).first]->get_copy();
           list->push_back ((*s).first);
           infile->m_Strings[(*s).first]->set_string ("This string was added automatically");
           }
        else
           if (in->get_string() != out->get_string() || out->get_string () == "This string was added automatically")
              list->push_back ((*s).first);
        }
}
//-------------------------------------------------------------------------------------
void        CXMLFile::export_data         ()
{
    TiXmlDocument *document = new TiXmlDocument (m_Name.c_str ());
    if (!document) throw Exception ("Error allocate memory");
    TiXmlDeclaration decl ("1.0", "windows-1251", "no");
    document->InsertEndChild (decl);
    TiXmlElement root ("string_table");

    xml_string_i s = m_Strings.begin(), e = m_Strings.end ();
    for (;  s != e; ++s)
        {
        TiXmlElement id ("string");
        id.SetAttribute ("id", (*s).first.c_str ());    //set element id
        TiXmlElement text ("text");     //set text element
        TiXmlText textdata ((*s).second->get_string().c_str ());
        text.InsertEndChild (textdata);
        id.InsertEndChild (text);
        root.InsertEndChild (id);
        }

    document->InsertEndChild (root);
    if (document->SaveFile () == false)
       throw Exception ("Error save file " + (AnsiString)m_Name.c_str ());
    delete document;

    if (m_bWithoutHeader == true)
       remove_header ();

    if (!m_Includes.size ()) return;

    char buf[512], str[256];
    GetTempPath (512, buf);
    strcat (buf, "temp.xml");
    FILE *filetemp = fopen (buf, "wt");
    VERIFY (filetemp);
    FILE *filesrc = fopen (m_Name.c_str (), "rt");
    VERIFY (filesrc);
    while (fgets (str, 256, filesrc))
          {
          if (strstr (str, "<string_table>"))
             {
             fputs (str, filetemp);
             include_list_i s = m_Includes.begin (), e = m_Includes.end ();
             for (; s != e; ++s)
                 fprintf (filetemp, "%s\n", (*s).c_str ());
             }
          else
             fputs (str, filetemp);
          }
    fclose (filetemp);
    CopyFile (buf, m_Name.c_str (), FALSE);
    DeleteFile (buf);

}
//-------------------------------------------------------------------------------------
bool        CXMLFile::remove_header ()
{
    char buf[512];
    GetTempPath (512, buf);
    strcat (buf, "temp.xml");
    FILE *filein = fopen (m_Name.c_str (), "rt");
    VERIFY (filein);
    FILE *fileout = fopen (buf, "wt");
    char tstr[16384];
    while (fgets (tstr, 512, filein))
          {
          if (!strstr (tstr, "xml version=") && !strstr (tstr, "<string_table>") && !strstr (tstr, "</string_table>"))
             fputs (tstr, fileout);
          }
    fclose (filein);
    fclose (fileout);
    CopyFile (buf, m_Name.c_str (), FALSE);
    DeleteFile (buf);
    return true;
}
//-------------------------------------------------------------------------------------
void        CXMLFile::set_name            (string name)
{
    m_Name = name;
    char drv[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT], final[512];
    _splitpath (m_Name.c_str (), drv, dir, fname, ext);
    m_ShortName = fname;
}
//-------------------------------------------------------------------------------------
void        CXMLFile::get_statistic       (int *strings, int *words)
{
    *strings = m_Strings.size ();
    xml_string_i s = m_Strings.begin(), e = m_Strings.end ();
    *words = 0;
    for (;  s != e; ++s)
        {
        if ((*s).second)
           *words += (*s).second->get_words();
        }

}
//-------------------------------------------------------------------------------------
void        CXMLFile::get_string_list     (vector<string> *list)
{
    if (!list) return;
    list->clear ();
    if (!m_Strings.size ()) return;
    list->clear ();
    xml_string_i s = m_Strings.begin(), e = m_Strings.end ();
    for (; s != e; ++s)
        list->push_back ((*s).first);
}
//-------------------------------------------------------------------------------------
bool        CXMLFile::is_string_present   (string strid)
{
    bool res = m_Strings[strid] != NULL;
    if (res == true) return res;
    xml_string_i i = m_Strings.find (strid);
    if (i == m_Strings.end ()) return false;
    m_Strings.erase (i);
    return false;
}
//-------------------------------------------------------------------------------------
void        CXMLFile::remove_string       (string strid)
{
    xml_string_i i = m_Strings.find (strid);
    if (i == m_Strings.end ()) return;
    TXMLString *str = (*i).second;
    delete str;
    m_Strings.erase (i);
}
//-------------------------------------------------------------------------------------
void        CXMLFile::get_notrans_strings (vector<string> *list)
{
    if (!list) return;
    list->clear ();
    xml_string_i s = m_Strings.begin(), e = m_Strings.end ();
    for (; s != e; ++s)
        if ((*s).second->get_string() == "This string was added automatically")
           list->push_back ((*s).first);
}
//-------------------------------------------------------------------------------------
void        CXMLFile::dump                (FILE *file)
{
    xml_string_i s = m_Strings.begin(), e = m_Strings.end ();
    for (; s != e; ++s)
        if ((*s).second == NULL)
           fprintf (file, "    ! %s\n", (*s).first);
        else
           fprintf (file, "    + %s\n", (*s).first);

}
//-------------------------------------------------------------------------------------
bool        CXMLFile::locked              ()
{
    if (m_Lock != INVALID_HANDLE_VALUE) return true;
    HANDLE h = CreateFile (m_Name.c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (h == INVALID_HANDLE_VALUE) return true;
    CloseHandle (h);
    return false;
}
//-------------------------------------------------------------------------------------
bool        CXMLFile::lock                ()
{
    if (locked () == true) return true;
    m_Lock = CreateFile (m_Name.c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_Lock == INVALID_HANDLE_VALUE)
       {
       Application->MessageBoxA (get_owner().c_str (), "File locked by another user", MB_OK + MB_ICONSTOP);
       return false;
       }
    FILE *file = fopen (m_LockName.c_str (), "wt");
    if (file)
       {
       char buf[256];
       DWORD size;
       GetUserName (buf, &size);
       buf[size] = 0;
       fprintf (file, "%s", buf);
       fclose (file);
       }
    return true;
}
//-------------------------------------------------------------------------------------
void        CXMLFile::unlock              ()
{
    if (locked () == false) return;
    CloseHandle (m_Lock);
    m_Lock = INVALID_HANDLE_VALUE;
    remove (m_LockName.c_str ());
}
//-------------------------------------------------------------------------------------
void        CXMLFile::unlock_w_save       ()
{
    if (locked () == false) return;
    CloseHandle (m_Lock);
    m_Lock = INVALID_HANDLE_VALUE;
    remove (m_LockName.c_str ());
}
//-------------------------------------------------------------------------------------
string      CXMLFile::get_owner           ()
{
    FILE *file = fopen (m_LockName.c_str (), "rt");
    if (!file) return "Unknown";
    char buf[256];
    fgets (buf, 256, file);
    fclose (file);
    return string (buf);
}

