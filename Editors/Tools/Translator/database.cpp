//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdlib.h>
#pragma hdrstop
#include "xmlfile.h"
#include "database.h"
//---------------------------------------------------------------------------



#pragma package(smart_init)
using namespace std;


//---------------------------------------------------------------------------
                    CDatabase::CDatabase           (string path, string name)
{
    m_Path = path;
    m_Name = name;
}
//---------------------------------------------------------------------------
                    CDatabase::~CDatabase          ()
{
    xml_file_base_i s = m_Base.begin (), e = m_Base.end ();
    for (; s != e; ++s)
        {
        CXMLFile *file = (*s).second;
        if (file) delete file;
        }
}
//---------------------------------------------------------------------------
void                CDatabase::Create              ()
{
    string fullpath = m_Path + m_Name, srcpath = m_Path;
    fullpath += "\\";
    srcpath += m_Name;
    srcpath += "\\*.xml";
    WIN32_FIND_DATA data;
    HANDLE hFind = FindFirstFile (srcpath.c_str(), &data);
    BOOL bEnd = hFind != INVALID_HANDLE_VALUE;
    while (bEnd)
          {
          CXMLFile *file = new CXMLFile (fullpath + data.cFileName);
          if (!file)
             throw Exception ("Error allocate memory");
          char buf[512];
          strcpy (buf, data.cFileName);
          CharLowerBuff (buf, strlen (buf));
          char *ptr = strstr (buf, ".xml");
          if (ptr) *ptr = 0;
          m_Base[buf] = file;
          bEnd = FindNextFile (hFind, &data);
          }
    FindClose (hFind);
}
//---------------------------------------------------------------------------
void                CDatabase::Save                ()
{
    size_t files = m_Base.size();
    if (!files) return;

    char drv[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT], final[512];
    _splitpath (Application->ExeName.c_str (), drv, dir, fname, ext);
    sprintf (final, "%s%s%s.db", drv, dir, m_Name.c_str ());
    FILE *file = fopen (final, "wb");
    if (!file)
       throw Exception ("Error create database file " + (AnsiString)final);
    fwrite (&files, sizeof (size_t), 1, file);
    xml_file_base_i s = m_Base.begin (), e = m_Base.end ();
    for (; s != e; ++s)
        {
        CXMLFile *xmlfile = (*s).second;
        xmlfile->save (file);
        }
    fclose (file);
}
//---------------------------------------------------------------------------
bool                CDatabase::Load                ()
{
    if (m_Base.size())
       throw Exception ("Can't load in non empty database");

    char drv[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT], final[512];
    _splitpath (Application->ExeName.c_str (), drv, dir, fname, ext);
    sprintf (final, "%s%s%s.db", drv, dir, m_Name.c_str ());
    FILE *file = fopen (final, "rb");
    if (!file)
       {
       Create ();
       Save ();
       return true;
       }
    size_t count = 0;
    fread (&count, sizeof (size_t), 1, file);
    if (!count)
       {
       fclose (file);
       Create ();
       Save ();
       return true;
       }
    for (size_t a = 0; a < count; a++)
        {
        CXMLFile *xmlfile = new CXMLFile ();
        if (!xmlfile) throw Exception ("Error allocate memory");
        xmlfile->load (file);
        m_Base[xmlfile->GetName()] = xmlfile;
        m_LockedFiles[xmlfile->GetName()] = xmlfile->locked ();
        }
    fclose (file);
    return true;
}
//---------------------------------------------------------------------------
void                CDatabase::FillListBox         (TListBox *box)
{
    box->Clear ();
    xml_file_base_i s = m_Base.begin (), e = m_Base.end ();
    for (; s != e; ++s)
        {
        AnsiString str = (*s).first.c_str ();
        box->Items->Add (str);
        }
}
//---------------------------------------------------------------------------
void                CDatabase::AddDifferentFiles   (CDatabase *another, vector<string> *list)
{
    xml_file_base_i s = another->m_Base.begin(), e = another->m_Base.end ();
    for (; s != e; ++s)
        {
        if (!m_Base[(*s).first])
           {
           string str = (*s).first;
           m_Base[(*s).first] = (*s).second->get_copy();
           string newname = m_Path;
           newname += m_Name;
           newname += "\\";
           newname += m_Base[(*s).first]->GetName ();
           newname += ".xml";
           m_Base[(*s).first]->set_name (newname);
           m_Base[(*s).first]->export_data ();
           m_LockedFiles[(*s).first] = (*s).second->locked ();
           if (list) list->push_back ((*s).first);
           }
        }
}
//---------------------------------------------------------------------------
void                CDatabase::AddDifferentStrings (CDatabase *another, xml_diff_strings *list)
{
    xml_file_base_i s = another->m_Base.begin(), e = another->m_Base.end ();
    for (; s != e; ++s)
        {
        CXMLFile *dfile = m_Base[(*s).first];
        CXMLFile *ifile = another->m_Base[(*s).first];
        if (!dfile || !ifile)
           throw Exception ("Found different file " + (AnsiString)(*s).first.c_str ());
        dfile->add_different_str (ifile, list);   
        }
}
//---------------------------------------------------------------------------
void                CDatabase::FillStrings         (string file, TListBox *box)
{
    box->Clear ();
    CXMLFile *xmlfile = m_Base[file];
    if (!xmlfile) return;
    xmlfile->fill_list (box);
}
//---------------------------------------------------------------------------
string              CDatabase::GetString           (string file, string strid)
{
    CXMLFile *xmlfile = m_Base[file];
    if (!xmlfile)
       {
       return "";
       }
    TXMLString *str = xmlfile->get_string (strid);
    if (!str)
       {
       xmlfile->add_string (strid, "This string was added automatically");
       return xmlfile->get_string (strid)->get_string ();
       }
    return str->get_string();
}
//---------------------------------------------------------------------------
void                CDatabase::UpdateString        (string file, string strid, string text)
{
    CXMLFile *xmlfile = m_Base[file];
    if (!xmlfile) return;
    TXMLString *str = xmlfile->get_string (strid);
    if (!str)
       xmlfile->add_string (strid, text);
    else
       str->set_string (text);
}
//---------------------------------------------------------------------------
void                CDatabase::GetDifferentFiles   (CDatabase *another, TListBox *box)
{
    box->Clear ();
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        vector<string> diff_str;
        GetDifferentStrings (another, (*s).first, &diff_str);
        if (diff_str.size())
           box->Items->Add ((AnsiString)(*s).first.c_str ());
        }
}
//---------------------------------------------------------------------------
void                CDatabase::GetDifferentStrings (CDatabase *another, string file, vector<string> *list)
{
    CXMLFile *infile = m_Base[file], *anfile = another->m_Base[file];
    if (!infile || !anfile)
       throw Exception ("Invalid file interface");
     infile->get_deff_str_list (anfile, list);
}
//---------------------------------------------------------------------------
void                CDatabase::GetNoTranslatedFiles(TListBox *box)
{
    box->Clear ();
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        vector<string> diff_str;
        GetNoTranslatedStrings ((*s).first, &diff_str);
        if (diff_str.size())
           box->Items->Add ((AnsiString)(*s).first.c_str ());
        }
}
//---------------------------------------------------------------------------
void                CDatabase::GetNoTranslatedStrings (string file, vector<string> *list)
{
    m_Base[file]->get_notrans_strings (list);
}
//---------------------------------------------------------------------------
void                CDatabase::Export              ()
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        (*s).second->export_data();
}
//---------------------------------------------------------------------------
void                CDatabase::GetStatistic        (DB_STAT *stats)
{
    stats->m_iFiles = m_Base.size ();
    stats->m_iStrings = stats->m_iWords = 0;
    int strings, words;
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        (*s).second->get_statistic (&strings, &words);
        stats->m_iStrings += strings;
        stats->m_iWords += words;
        }
}
//---------------------------------------------------------------------------
bool                CDatabase::IsStringExist       (string file, string strid)
{
    if (IsFileExist (file) == false) return false;
    return m_Base[file]->is_string_present (strid);
}
//---------------------------------------------------------------------------
void                CDatabase::RemoveString        (string file, string strid)
{
    if (IsFileExist (file) == false) return;
    m_Base[file]->remove_string (strid);
}
//---------------------------------------------------------------------------
void                CDatabase::GetFilesList        (vector<string> *list)
{
    if (m_Base.size() == 0 || !list) return;
    list->clear ();
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        list->push_back ((*s).first);
}
//---------------------------------------------------------------------------
void                CDatabase::GetStringList       (string file, vector<string> *list)
{
    m_Base[file]->get_string_list (list);
}
//---------------------------------------------------------------------------
void                CDatabase::Dump                ()
{
    char buf[256];
    sprintf (buf, "%s_dump.txt", m_Name);
    FILE *file = fopen (buf, "wt");
    if (!file) return;
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        CXMLFile *f = (*s).second;
        fprintf (file, "File : %s\n", f->GetName());
        f->dump (file);
        }
    fclose (file);
}
//---------------------------------------------------------------------------
bool                CDatabase::LockFile            (string file)
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        if ((*s).first == file)
           {
           m_LockedFiles[(*s).first] = true;
           return (*s).second->lock ();
           }
        }
    return false;
}
//---------------------------------------------------------------------------
void                CDatabase::UnlockFile          (string file)
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        if ((*s).first == file)
           {
           m_LockedFiles[(*s).first] = false;
           (*s).second->unlock ();
           return;
           }
        }
}
//---------------------------------------------------------------------------
void                CDatabase::UnlockFileWSave     (string file)
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        if ((*s).first == file)
           {
           m_LockedFiles[(*s).first] = false;
           (*s).second->unlock ();
           return;
           }
        }
}
//---------------------------------------------------------------------------
bool                CDatabase::IsFileLocked        (string file)
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        if ((*s).first == file)
           return (*s).second->locked ();
        }
    return false;
}
//---------------------------------------------------------------------------
string              CDatabase::GetFileOwner        (string file)
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        if ((*s).first == file)
           return (*s).second->get_owner ();
        }
    return "Unknown";
}
//---------------------------------------------------------------------------
void                CDatabase::ExportFile          (string file)
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        if ((*s).first == file)
           return (*s).second->export_data ();
        }
}
//---------------------------------------------------------------------------
bool                CDatabase::IsOurOwnerFile      (string file)
{
    string s = GetFileOwner (file);
    char buf[256];
    DWORD size;
    GetUserName (buf, &size);
    buf[size] = 0;
    return s == string (buf);
}
//---------------------------------------------------------------------------
bool                CDatabase::IsFileExist         (string file)
{
    return m_Base.find (file) != m_Base.end ();
}
//---------------------------------------------------------------------------
bool                CDatabase::IsLockStatusChanged ()
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        if ((*s).second->locked () != m_LockedFiles[(*s).first])
           {
           m_LockedFiles[(*s).first] = (*s).second->locked ();
           return true;
           }
    return false;
}
//---------------------------------------------------------------------------
bool                CDatabase::IsLockedOurFilesPres()
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        if (IsFileLocked ((*s).first) == true && IsOurOwnerFile ((*s).first))
           return true;
        }
    return false;
}
//---------------------------------------------------------------------------
void                CDatabase::UnlockAllFiles      ()
{
    xml_file_base_i s = m_Base.begin(), e = m_Base.end ();
    for (; s != e; ++s)
        {
        string name = (*s).first;
        if (IsFileLocked ((*s).first) == true && IsOurOwnerFile ((*s).first))
           (*s).second->unlock ();
        }
}










//---------------------------------------------------------------------------
CDatabase*    Database_Load       (string srcpath, string shortcut)
{
    CDatabase *data = new CDatabase (srcpath, shortcut);
    if (!data) throw Exception ("Error allocate memory for database");
    if (data->Load () == false)
       {
       delete data;
       throw Exception ("Error load database");
       }
    return data;
}
//---------------------------------------------------------------------------
CDatabase*    Database_Import     (string srcpath, string shortcut)
{
    CDatabase *data = new CDatabase (srcpath, shortcut);
    if (!data) throw Exception ("Error allocate memory for database");
    data->Create ();
    return data;
}

