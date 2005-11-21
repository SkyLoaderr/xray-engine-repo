//---------------------------------------------------------------------------

#ifndef databaseH
#define databaseH
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "xmlstring.h"
#include "xmlfile.h"

using namespace std;

//---------------------------------------------------------------------------

typedef struct _DB_STAT
{
    int         m_iFiles;
    int         m_iStrings;
    int         m_iWords;
} DB_STAT;

typedef map<string, CXMLFile*>                  xml_file_base;
typedef map<string, CXMLFile*>::iterator        xml_file_base_i;

typedef map<string, bool>                       file_lock;
typedef map<string, bool>::iterator             file_lock_i;


class CDatabase
{
public:
    std::string         m_Name;
    std::string         m_Path;
    xml_file_base       m_Base;
    file_lock           m_LockedFiles;
                        CDatabase           (string path, string name);
                       ~CDatabase           ();
    void                Create              ();
    void                Save                ();
    bool                Load                ();
    void                FillListBox         (TListBox *box);
    void                AddDifferentFiles   (CDatabase *another, vector<string> *list);
    void                AddDifferentStrings (CDatabase *another, xml_diff_strings *list);
    void                FillStrings         (string file, TListBox *box);
    string              GetString           (string file, string strid);
    void                UpdateString        (string file, string strid, string text);
    void                GetDifferentFiles   (CDatabase *another, TListBox *box);
    void                GetDifferentStrings (CDatabase *another, string file, vector<string> *list);
    void                Export              ();
    void                GetStatistic        (DB_STAT *stats);
    bool                IsStringExist       (string file, string strid);
    void                RemoveString        (string file, string strid);
    void                GetFilesList        (vector<string> *list);
    void                GetStringList       (string file, vector<string> *list);
    void                GetNoTranslatedFiles(TListBox *box);
    void                GetNoTranslatedStrings (string file, vector<string> *list);
    void                Dump                ();
    bool                LockFile            (string file);
    void                UnlockFile          (string file);
    void                UnlockFileWSave     (string file);
    bool                IsFileLocked        (string file);
    string              GetFileOwner        (string file);
    void                ExportFile          (string file);
    bool                IsOurOwnerFile      (string file);
    bool                IsFileExist         (string file);
    bool                IsLockStatusChanged ();
    bool                IsLockedOurFilesPres();
    void                UnlockAllFiles      ();
};


CDatabase*    Database_Load       (string srcpath, string shortcut);
CDatabase*    Database_Import     (string srcpath, string shortcut);
#endif
