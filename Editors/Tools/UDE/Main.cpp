//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "config.h"
#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "RXCtrls"
#pragma resource "*.dfm"
TMainForm *MainForm;


//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
    vsConfig m_pConfig;

    if (FAILED (m_pConfig.LoadFromFile ("x:\\gamedata\\config\\misc\\dialog_manager.ltx")))
       throw Exception ("Error load file dialog_manager.ltx");
    TiXmlDocument m_File ("X:\\gamedata\\config\\text\\rus\\stable_dialog_manager.xml");
    if (m_File.LoadFile () == false)
       throw Exception ("Error load file stable_dialog_manager.xml");
    TiXmlElement *m_Root = m_File.FirstChildElement ("string_table");
    if (!m_Root)
       throw Exception ("Error getting node <string_table> from file stable_dialog_manager.xml");

    int count = m_pConfig.GetUnitsCount ("list");
    for (int a = 0; a < count; a++)
        {
        char param[256], value[256];
        m_pConfig.EnumerateItems ("list", a, param, value);
        //load the parameters
        DM_UNIT u;
        m_pConfig.GetString (param, "category", value, "intro");
        u.m_Category = value;
        m_pConfig.GetString (param, "community", value, "");
        u.m_Community = value;
        m_pConfig.GetString (param, "npc_сommunity", value, "");
        u.m_NPCCommunity = value;
        m_pConfig.GetString (param, "relation", value, "");
        u.m_Relation = value;
        u.m_NPCRank = m_pConfig.GetInt (param, "npc_rank", 0);
        m_pConfig.GetString (param, "level", value, "");
        u.m_Level = value;
        m_pConfig.GetString (param, "condlist", value, "");
        u.m_Condlist = value;
        m_pConfig.GetString (param, "smartterrain", value, "");
        u.m_SmartTerrain = value;
        TiXmlNode *child;
        for (child = m_Root->FirstChild (); child; child = child->NextSibling ())
            {
            if (child->Type () != TiXmlNode::ELEMENT) continue; 
            if (!strcmp (child->ToElement ()->Attribute ("id"), param))
               {
               u.m_Text = child->FirstChild ("text")->FirstChild ()->Value ();
               StringList->Items->Add (param);
               break;
               }
            }
        u.m_Used = true;
        m_List[param] = u;
        }
    if (StringList->Items->Count)
       {
       StringList->ItemIndex = 0;
       StringList->Selected[0] = true;
       StringListClick (StringList);
       }
    m_Original = &m_List;
    m_bInternal = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::StringListClick(TObject *Sender)
{
    if (!StringList->Items->Count) return;
    DM_UNIT u = m_List[StringList->Items->Strings[StringList->ItemIndex].c_str ()];
    if (u.m_Used == false) return;

    m_bInternal = true;
    Text->Clear ();
    Text->Text = u.m_Text.c_str ();
    Category->ItemIndex = Category->Items->IndexOf (u.m_Category.c_str ());
    if (Category->ItemIndex == -1) Category->ItemIndex = 0;
    Community->ItemIndex = Community->Items->IndexOf (u.m_Community.c_str ());
    if (Community->ItemIndex == -1) Community->ItemIndex = 0;
    NPCCommunity->ItemIndex = NPCCommunity->Items->IndexOf (u.m_NPCCommunity.c_str ());
    if (NPCCommunity->ItemIndex == -1) NPCCommunity->ItemIndex = 0;
    Relation->ItemIndex = Relation->Items->IndexOf (u.m_Relation.c_str ());
    if (Relation->ItemIndex == -1) Relation->ItemIndex = 0;
    NPCRank->Value = u.m_NPCRank;
    Level->ItemIndex = Level->Items->IndexOf (u.m_Level.c_str ());
    if (Level->ItemIndex == -1) Level->ItemIndex = 0;
    Condlist->Text = u.m_Condlist.c_str ();
    SmartTerrain->Text = u.m_SmartTerrain.c_str ();
    m_bInternal = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CategoryChange(TObject *Sender)
{
    if (!StringList->Items->Count || m_bInternal == true) return;
    DM_UNIT u = m_List[StringList->Items->Strings[StringList->ItemIndex].c_str ()];
    if (u.m_Used == false) return;
    u.m_Category = Category->Items->Strings[Category->ItemIndex].c_str ();
    u.m_Community = Community->Items->Strings[Community->ItemIndex].c_str ();
    u.m_NPCCommunity = NPCCommunity->Items->Strings[NPCCommunity->ItemIndex].c_str ();
    u.m_Relation = Relation->Items->Strings[Relation->ItemIndex].c_str ();
    u.m_NPCRank = NPCRank->Value;
    u.m_Level = Level->Items->Strings[Level->ItemIndex].c_str ();
    u.m_Condlist = Condlist->Text.c_str ();
    u.m_SmartTerrain = SmartTerrain->Text.c_str ();
    u.m_Text = Text->Text.c_str ();
    m_List[StringList->Items->Strings[StringList->ItemIndex].c_str ()] = u;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AddStringButtonClick(TObject *Sender)
{
    if (AddString->Text.IsEmpty () == true) return;
    if (StringList->Items->IndexOf (AddString->Text) != -1)
       {
       Application->MessageBoxA ("String with this ID is already present", "Error", MB_OK | MB_ICONERROR);
       return;
       }
    DM_UNIT u;
    u.m_Used = true;
    u.m_Category = "intro";
    u.m_Text = ((AnsiString)"Default text " + AddString->Text).c_str ();
    m_List[AddString->Text.c_str ()] = u;
    StringList->Items->Add (AddString->Text);
    StringList->ItemIndex = StringList->Items->Count - 1;
    StringListClick (StringList);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DeleteStringButtonClick(TObject *Sender)
{
    if (!StringList->Items->Count || StringList->ItemIndex == -1) return;
    int i = StringList->ItemIndex;
    DM_UNIT u = m_List[StringList->Items->Strings[i].c_str ()];
    if (u.m_Used == false) return;
    u.m_Used = false;
    m_List[StringList->Items->Strings[i].c_str ()] = u;
    StringList->Items->Delete (i);
    if (i > 0) --i;
    StringList->ItemIndex = i;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Shift.Contains (ssCtrl) == false) return;

    if (Key == 'F')
       {
       Find ();
       return;
       }
    if (Key != 'S') return;

    vsConfig config;

    TiXmlDocument m_File;
    TiXmlDeclaration decl ("1.0", "windows-1251", "yes");
    m_File.InsertEndChild (decl);
    TiXmlElement root ("string_table");

    dm_list_i s = m_List.begin (), e = m_List.end ();
    for (; s != e; ++s)
        {
        DM_UNIT u = (*s).second;
        string name = (*s).first;
        config.SetParam ("list", name.c_str ());
        if (u.m_Category != "")
           config.SetString (name.c_str (), "category", u.m_Category.c_str ());
        if (u.m_Community != "")
           config.SetString (name.c_str (), "community", u.m_Community.c_str ());
        if (u.m_NPCCommunity != "")
           config.SetString (name.c_str (), "npc_community", u.m_NPCCommunity.c_str ());
        if (u.m_Relation != "")
           config.SetString (name.c_str (), "relation", u.m_Relation.c_str ());
        if (u.m_NPCRank != 0)
           config.SetInt (name.c_str (), "npc_rank", u.m_NPCRank);
        if (u.m_Level != "")
           config.SetString (name.c_str (), "level", u.m_Level.c_str ());
        if (u.m_Condlist != "")
           config.SetString (name.c_str (), "condlist", u.m_Condlist.c_str ());
        if (u.m_SmartTerrain != "")
           config.SetString (name.c_str (), "smartterrain", u.m_SmartTerrain.c_str ());

        TiXmlElement id ("string");
        id.SetAttribute ("id", name.c_str ());    //set element id
        TiXmlElement text ("text");     //set text element
        TiXmlText textdata (u.m_Text.c_str ());
        text.InsertEndChild (textdata);
        id.InsertEndChild (text);
        root.InsertEndChild (id);

        }

    m_File.InsertEndChild (root);

    if (FAILED (config.Save ("x:\\gamedata\\config\\misc\\dialog_manager.ltx")) ||
        m_File.SaveFile ("X:\\gamedata\\config\\text\\rus\\stable_dialog_manager.xml") == false)
       Application->MessageBox ("Error save files. Possible set READ ONLY attribute", "Error", MB_OK);
    else
       Application->MessageBox ("Store files success", "Save", MB_OK);
}
//---------------------------------------------------------------------------
void            TMainForm::Find        ()
{
//    if (FindStringDialog->ShowModal (&m_List) == mrCancel) return;
//    if (FindStringDialog->m_Selected == "") return;
//    int index = StringList->Items->IndexOf (FindStringDialog->m_Selected.c_str ());
//    if (index == -1)
//       Application->MessageBox ("Странно, но строка не обнаружена в списке :()", "Таки ошибочка вышла", MB_OK | MB_ICONERROR);
//    else
//       {
//       StringList->ItemIndex = index;
//       StringListClick (StringList);
//       }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PageChange(TObject *Sender, int NewTab, bool &AllowChange)
{
    if (NewTab == 1)
       {
       m_Original = &m_List;
       FillList ();
       }
    else
       {
       AnsiString sel;
       if (StringList->ItemIndex != -1)
          sel = StringList->Items->Strings[StringList->ItemIndex];
       else
          sel = "";
       StringList->Clear ();
       dm_list_i s = m_List.begin (), e = m_List.end ();
       for (; s != e; ++s)
           {
           if ((*s).second.m_Used == false) continue;
           StringList->Items->Add ((*s).first.c_str ());
           }
       if (sel != "")
          {
          StringList->ItemIndex = StringList->Items->IndexOf (sel);
          StringListClick (StringList);
          }
       }
}
//---------------------------------------------------------------------------
void        TMainForm::FillList        ()
{
    m_FList.clear ();
    StringList->Clear ();

    dm_list_i s = m_Original->begin (), e = m_Original->end ();
    for (; s != e; ++s)
        {
        if ((*s).second.m_Used == false) continue;
        StringList->Items->Add ((*s).first.c_str ());
        m_FList[(*s).first] = (*s).second;
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CategoryFChange(TObject *Sender)
{
    FillList ();

    string category = CategoryF->Items->Strings[CategoryF->ItemIndex].c_str (),
           community = CommunityF->Items->Strings[CommunityF->ItemIndex].c_str (),
           npccommunity = NPCCommunityF->Items->Strings[NPCCommunityF->ItemIndex].c_str (),
           relation = RelationF->Items->Strings[RelationF->ItemIndex].c_str (),
           level = LevelF->Items->Strings[LevelF->ItemIndex].c_str (),
           condlist = CondlistF->Text.c_str (),
           smart = SmartTerrainF->Text.c_str ();
    int rank = NPCRankF->Value;

    dm_list_i s = m_FList.begin (), e = m_FList.end ();
    for (; s != e; ++s)
        {
        //check for category
        if (category != "" && (*s).second.m_Category != category)
           {
           (*s).second.m_Used = false;
           StringList->Items->Delete (StringList->Items->IndexOf ((*s).first.c_str ()));
           }
        //check for community
        if (community != "" && (*s).second.m_Community != community)
           {
           (*s).second.m_Used = false;
           StringList->Items->Delete (StringList->Items->IndexOf ((*s).first.c_str ()));
           }
        //check for npc community
        if (npccommunity != "" && (*s).second.m_NPCCommunity != npccommunity)
           {
           (*s).second.m_Used = false;
           StringList->Items->Delete (StringList->Items->IndexOf ((*s).first.c_str ()));
           }
        //check for relation
        if (relation != "" && (*s).second.m_Relation != relation)
           {
           (*s).second.m_Used = false;
           StringList->Items->Delete (StringList->Items->IndexOf ((*s).first.c_str ()));
           }
        //check for level
        if (level != "" && (*s).second.m_Level != level)
           {
           (*s).second.m_Used = false;
           StringList->Items->Delete (StringList->Items->IndexOf ((*s).first.c_str ()));
           }
        //check for condlist
        if (condlist != "" && (*s).second.m_Condlist != condlist)
           {
           (*s).second.m_Used = false;
           StringList->Items->Delete (StringList->Items->IndexOf ((*s).first.c_str ()));
           }
        //check for smartterrain
        if (smart != "" && (*s).second.m_SmartTerrain != smart)
           {
           (*s).second.m_Used = false;
           StringList->Items->Delete (StringList->Items->IndexOf ((*s).first.c_str ()));
           }
        if (rank != 0 && (*s).second.m_NPCRank != rank)
           {
           (*s).second.m_Used = false;
           StringList->Items->Delete (StringList->Items->IndexOf ((*s).first.c_str ()));
           }
        }
}
//---------------------------------------------------------------------------

