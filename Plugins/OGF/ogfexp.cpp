//************************************************************************** 
//* OGFexp.cpp	- OGF File Exporter
//* 
//* By Christer Janson
//* Kinetix Development
//*
//* January 20, 1997 CCJ Initial coding
//*
//* This module contains the DLL startup functions
//*
//* Copyright (c) 1997, All Rights Reserved. 
//***************************************************************************

#include "stdafx.h"
#include "OGFexp.h"
#include "mesh.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

static BOOL showPrompts;
static BOOL exportSelected;

// Class ID. These must be unique and randomly generated!!
// If you use this as a sample project, this is the first thing
// you should change!
#define	OGFEXP_CLASS_ID	Class_ID(0x758132ec, 0x6eba3e56)

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
{
	hInstance = hinstDLL;

	// Initialize the custom controls. This should be done only once.
	if (!controlsInit) {
		controlsInit = TRUE;
		InitCustomControls(hInstance);
		InitCommonControls();
	}
	
	return (TRUE);
}


__declspec( dllexport ) const TCHAR* LibDescription() 
{
	return GetString(IDS_LIBDESCRIPTION);
}

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS 
__declspec( dllexport ) int LibNumberClasses() 
{
	return 1;
}


__declspec( dllexport ) ClassDesc* LibClassDesc(int i) 
{
	switch(i) {
	case 0: return GetOGFExpDesc();
	default: return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion() 
{
	return VERSION_3DSMAX;
}

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

//------------------------------------------------------------------------------
class OGFExpClassDesc:public ClassDesc {
	OGFExp*			exp;
public:
	int				IsPublic() { return 1; }
	void*			Create(BOOL loading = FALSE) { return exp = new OGFExp; } 
	const TCHAR*	ClassName() { return GetString(IDS_OGFEXP); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; } 
	Class_ID		ClassID() { return OGFEXP_CLASS_ID; }
	const TCHAR*	Category() { return GetString(IDS_CATEGORY); }

	OGFExp*			GetExporter() {return exp;}
};

static OGFExpClassDesc OGFExpDesc;

ClassDesc* GetOGFExpDesc()
{
	return &OGFExpDesc;
}

OGFExp*	GetExporter(){
	return OGFExpDesc.GetExporter();
};

HWND GetDlgWnd()
{
	return GetExporter()->hDialog;
}

//------------------------------------------------------------------------------
// messages
//------------------------------------------------------------------------------

void DMsg( char *format, ... )
{
	va_list marker; char buf[1024];
	va_start( marker, format );
	vsprintf( buf, format, marker );

	int idx = SendDlgItemMessage( GetDlgWnd(), IDC_PROCESS, LB_ADDSTRING, 0, (LPARAM)buf );
	SendDlgItemMessage( GetDlgWnd(), IDC_PROCESS, LB_SETCARETINDEX, idx+1, MAKELPARAM(TRUE, 0));
	UpdateWindow(GetDlgWnd());

	strcat( buf, "\n" );
	DebugPrint( buf );
}
//------------------------------------------------------------------------------

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;

	return NULL;
}

OGFExp::OGFExp()
{
}

OGFExp::~OGFExp()
{
}

int OGFExp::ExtCount()
{
	return 1;
}

const TCHAR * OGFExp::Ext(int n)
{
	switch(n) {
	case 0:
		return _T("ogf");
	}
	return _T("");
}

const TCHAR * OGFExp::LongDesc()
{
	return GetString(IDS_LONGDESC);
}

const TCHAR * OGFExp::ShortDesc()
{
	return GetString(IDS_SHORTDESC);
}

const TCHAR * OGFExp::AuthorName() 
{
	return _T("Christer Janson");
}

const TCHAR * OGFExp::CopyrightMessage() 
{
	return GetString(IDS_COPYRIGHT);
}

const TCHAR * OGFExp::OtherMessage1() 
{
	return _T("");
}

const TCHAR * OGFExp::OtherMessage2() 
{
	return _T("");
}

unsigned int OGFExp::Version()
{
	return 100;
}

//------------------------------------------------------------------------------
static BOOL CALLBACK SimplEnumChld( HWND hwnd, LPARAM lParam ){
	HWND* phw = (HWND*)lParam;
	(*phw) = hwnd;
	return FALSE;
}
static HWND L1Child( HWND control ){
	if( 0== control )
		return 0;
	HWND hwc = 0;
	EnumChildWindows( control, SimplEnumChld, (LPARAM)&hwc );
	return hwc;
}
static HWND L2Child( HWND control ) {
	return L1Child( L1Child( control ) );
}
void SetCheck(HWND h, DWORD c, bool b)
{
	SendMessage( GetDlgItem(h, c), BM_SETCHECK, b?BST_CHECKED:BST_UNCHECKED, 0 );
}
BOOL GetCheck(HWND h, DWORD c)
{
	return BST_CHECKED==SendMessage( GetDlgItem(h, c), BM_GETCHECK, 0, 0 );
}
void SetEditText(HWND h, DWORD c, char *text)
{
	SetWindowText(L2Child(GetDlgItem(h, c)), text);
}
void GetEditText(HWND h, DWORD c, char *text, int max)
{
	GetWindowText(L2Child(GetDlgItem(h, c)), text, max);
}
void SetTBPara	(HWND h, DWORD c, DWORD min, DWORD max, DWORD cur) 
{
	HWND htb = GetDlgItem(h, c);

	SendMessage(htb, TBM_SETRANGE	, FALSE,		 MAKELONG(min, max) );
	SendMessage(htb, TBM_SETPOS		, TRUE,			 cur				);
	SendMessage(htb, TBM_SETTICFREQ	, (max-min)/20,	 0					);
}
DWORD GetTB		(HWND h, DWORD c)
{
	HWND htb = GetDlgItem(h, c);
	return SendMessage(htb, TBM_GETPOS, 0, 0);
}
void SelectAllItems	(HWND h)
{
	int cnt		= SendDlgItemMessage( h, IDC_SELECTIONLIST, LB_GETCOUNT, 0, 0 ); 
	if (cnt>0)	SendDlgItemMessage( h, IDC_SELECTIONLIST, LB_SELITEMRANGE, 1, (cnt-1)<<16 );
}
void OGFExp::ClearLists()
{
	SendDlgItemMessage( hDialog, IDC_SELECTIONLIST, LB_RESETCONTENT, 0, 0 );
	exportlist.clear();
}
void OGFExp::AppendItem	(INode *pNode)
{
	ExpItem item;
	item.pNode = pNode;
	exportlist.push_back( item );
	SendDlgItemMessage( hDialog, IDC_SELECTIONLIST, LB_ADDSTRING, 0, (LPARAM) pNode->GetName() );
}
//------------------------------------------------------------------------------
/*
void OGFExp::UpdateSelectionListBox()
{
	SendDlgItemMessage( hDialog, IDC_SELECTIONLIST, LB_RESETCONTENT, 0, 0 );
	std::list<ExpItem>::iterator i = exportlist.begin();
	for(;i!=exportlist.end();i++)
		SendDlgItemMessage( hDialog, IDC_SELECTIONLIST, LB_ADDSTRING, 0, (LPARAM) i->pNode->GetName() );
}

void OGFExp::RefreshExportList()
{
	exportlist.clear();
	ExpItem item;
	int i = ip->GetSelNodeCount();
	while( i-- ){
		item.pNode = ip->GetSelNode(i);
		exportlist.push_back( item );
	}
	if (exportlist.size()<=0){
		DMsg( "No objects found in scene !" );
#pragma message ("! SEND CORRECT MESSAGE TO DISABLE BUTTON")
		SendDlgItemMessage( hDialog, IDC_RUN, STN_DISABLE, 0, 0 );
		UpdateWindow(hDialog);
	}
}
*/
bool OGFExp::GetSelectItem()
{
	int cnt = SendDlgItemMessage( hDialog, IDC_SELECTIONLIST, LB_GETSELCOUNT, 0, 0 ); 
	if (cnt>0){
		copylist.clear();
		copylist.resize(cnt);
		SendDlgItemMessage( hDialog, IDC_SELECTIONLIST, LB_GETSELITEMS, cnt, (LPARAM)(copylist.begin()) );

		return true;
	}else return false;
}

bool OGFExp::IsItemSelected	(LPSTR name) 
{
	char text[256];
	for (int i=0; i<copylist.size(); i++) {
		SendDlgItemMessage( hDialog, IDC_SELECTIONLIST, LB_GETTEXT, copylist[i], (LPARAM)(text) );
		if (stricmp(name,text)==0) return true;
	} 
	return false;
}

void OGFExp::Init(HWND hWnd)
{
	hDialog		= hWnd;
	SetCheck	(hWnd,IDC_MP,		false);
	SetCheck	(hWnd,IDC_MATERIAL,	false);
	SetCheck	(hWnd,IDC_SPECULAR,	true );
	SetCheck	(hWnd,IDC_ALPHA,	false);
	SetCheck	(hWnd,IDC_LIGHT,	true );
	SetCheck	(hWnd,IDC_VC,		false);
	SetCheck	(hWnd,IDC_CFORM,	true);
	SetEditText	(hWnd,IDC_SHADER,	"default");
	SetEditText	(hWnd,IDC_SHADERMT1,"mt1");
	SetEditText	(hWnd,IDC_SHADERMT2,"mt2");
	SetTBPara	(hWnd,IDC_BRIGHTNESS,	1,100,40);		// 80 = 1,1,1
	SetTBPara	(hWnd,IDC_POWER,		0,100,10);		// real power
	CheckRadioButton(hWnd,IDC_R_Standart,IDC_R_Skeleton,IDC_R_Standart);
//	RefreshExportList();
//	UpdateSelectionListBox( );

// fill list box
	ClearLists();
	int numChildren = ip->GetRootNode()->NumberOfChildren();
	
	// Call our node enumerator.
	// The nodeEnum function will recurse into itself and 
	// export each object found in the scene.
	
	for (int idx=0; idx<numChildren; idx++) {
		if (ip->GetCancel())
			break;
		nodeEnum(ip->GetRootNode()->GetChildNode(idx), 0);
	}
//----------------------------------------------
	if (exportlist.size()<=0){
		DMsg( "No objects found in scene !" );
		HWND itm = GetDlgItem( hDialog, IDC_RUN );
		EnableWindow( itm, false);
		SendDlgItemMessage( hDialog, IDC_RUN, STN_DISABLE, 0, 0 );
		UpdateWindow(hDialog);
	}
	SelectAllItems(hDialog);
#pragma message ("! правильно сохранить список объектов (пока объекты перезаписываются)")
}

void OGFExp::Destroy()
{
}

static BOOL CALLBACK AboutBoxDlgProc(HWND hWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		CenterWindow(hWnd, GetParent(hWnd)); 
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hWnd, 1);
			break;
		}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}       

void OGFExp::ShowAbout(HWND hWnd)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutBoxDlgProc, 0);
}


// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}


// Dialog proc
static BOOL CALLBACK ExportDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp)
{
	OGFExp *exp = (OGFExp*)GetWindowLong(hw,GWL_USERDATA); 

	switch (msg) {
		case WM_INITDIALOG:
  			exp = (OGFExp*)lp;
			exp->Init(hw);
			break;

		case WM_CLOSE:
			EndDialog(hw, 0);
			break;

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDC_RUN:
					if( GetExporter()->PrepareExport() )
						if( GetExporter()->RunExport( ) ){
							DMsg( "Export successfull." );
						}else{
							DMsg( "Export failed." );
						}
					break;
				case IDC_CLOSE:
					EndDialog(hw, 0);
					break;
				case IDC_LOGO:
					GetExporter()->ShowAbout(GetDlgWnd());
					break;
			}
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

// Start the exporter!
// This is the real entrypoint to the exporter. After the user has selected
// the filename (and he's prompted for overwrite etc.) this method is called.
int OGFExp::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options) 
{
	// Set a global prompt display switch
	showPrompts = suppressPrompts ? FALSE : TRUE;
	exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	// Grab the interface pointer.
	ip = i;

	if(showPrompts) {
		strcpy(filename, name);
		// Prompt the user with our dialogbox, and get all the options.
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_OGFEXPORT_DLG),
			ip->GetMAXHWnd(), ExportDlgProc, (LPARAM)this)) {
			DMsg( "Export canceled");
			return 1;
			}
		}
	else {	// Set default parameters here
		}

	return 1;
}

BOOL OGFExp::SupportsOptions(int ext, DWORD options) {
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

bool OGFExp::PrepareExport()
{
	SendDlgItemMessage( hDialog, IDC_PROCESS, LB_RESETCONTENT, 0, 0 );
	if (!GetSelectItem()){
		DMsg( "No item selected!", filename);
		return false;
	}

	return true;
}

bool OGFExp::RunExport()
{
	DMsg( "Starting..." );
	DMsg( "Exported to '%s'", filename);
	bool bResult = true;
	bool bRet = true;
	
	std::list<ExpItem>::iterator i = exportlist.begin();
	for(;i!=exportlist.end();i++) {
		if ( IsItemSelected(i->pNode->GetName()) ){
			BOOL bNeedDel = FALSE;
			TriObject* tri = GetTriObjectFromNode(i->pNode, bNeedDel);
			if (tri){
				DMsg( "Object '%s' start export .........................", i->pNode->GetName());
				char fns[MAX_PATH];
				if (GetCheck(hDialog,IDC_LINK_SF)) {
					// build_shadow name
					strcpy(fns,filename);
					strcpy(strchr(fns,'.'),"_sf.ogf");
				} else {
					fns[0]=0;
				}

				Mesh &m = tri->GetMesh();
				Mtl *mat = i->pNode->GetMtl();

				// Write main
				CMesh *ogf = new CMesh(hDialog, false);
				int hfVcn = _open( filename,
					_O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC,
					_S_IREAD |_S_IWRITE );
				if( hfVcn<=0 ) {
					DMsg("Can't open file");
					return false;
				}
				bResult&=ogf->GetMaterials(mat);
				bResult&=ogf->GetVertices(m);
				bResult&=ogf->GetVColors(m);
				bResult&=ogf->GetFaces(m);
				bResult&=ogf->GetMapping(m);
				bResult&=ogf->GetNormals();
				ogf->Information();
				bResult&=ogf->ConvertMapping();
				bResult&=ogf->Optimize();
				bResult&=ogf->GetBBox();
				bResult&=ogf->SaveOne(hfVcn, fns );
				_close( hfVcn );
				delete ogf;

				// Write shadow if need
				if (GetCheck(hDialog,IDC_LINK_SF)) {
					DMsg(" ");
					DMsg(" *** Building shadow form '%s'...", fns);
					DMsg(" ");
					CMesh *ogf = new CMesh(hDialog, true);
					hfVcn = _open( fns,
						_O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC,
						_S_IREAD |_S_IWRITE );
					if( hfVcn<=0 ) {
						DMsg("Can't open file");
						return false;
					}
					bResult&=ogf->GetMaterials(mat);
					bResult&=ogf->GetVertices(m);
					bResult&=ogf->GetVColors(m);
					bResult&=ogf->GetFaces(m);
					bResult&=ogf->GetMapping(m);
					bResult&=ogf->GetNormals();
					ogf->Information();
					bResult&=ogf->ConvertMapping();
					bResult&=ogf->Optimize();
					bResult&=ogf->GetBBox();
					bResult&=ogf->SaveOne(hfVcn, "");
					_close( hfVcn );
					delete ogf;
				}
				// Write CF if need
				if (GetCheck(hDialog,IDC_CFORM)) {
					// build CF name
					strcpy(fns,filename);
					strcpy(strchr(fns,'.'),".vcf");

					DMsg(" ");
					DMsg(" *** Building collision form '%s'...",fns);
					DMsg(" ");
					CMesh *ogf = new CMesh(hDialog, true);
					hfVcn = _open( fns,
						_O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC,
						_S_IREAD |_S_IWRITE );
					if( hfVcn<=0 ) {
						DMsg("Can't open file");
						return false;
					}
					bResult&=ogf->GetMaterials(mat);
					bResult&=ogf->GetVertices(m);
					bResult&=ogf->GetVColors(m);
					bResult&=ogf->GetFaces(m);
					bResult&=ogf->GetMapping(m);
					bResult&=ogf->GetNormals();
					ogf->Information();
					bResult&=ogf->ConvertMapping();
					bResult&=ogf->Optimize();
					bResult&=ogf->GetBBox();
					bResult&=ogf->SaveCF(hfVcn);
					_close( hfVcn );
					delete ogf;
				}
				DMsg("File: '%s' exported successfull", i->pNode->GetName());
			}else{
				DMsg("! Unsuported mesh format (non editable mesh): '%s'", i->pNode->GetName());
				bRet = false;
			}
			if (bNeedDel) delete tri;
		}
	}

	return bRet;
}

//------------ editable mesh
TriObject *OGFExp::ExtractTriObject( INode *node )
{
	ObjectState state = node->EvalWorldState(0,FALSE);
	if( state.obj->ClassID() == Class_ID(EDITTRIOBJ_CLASS_ID,0) )
		return (TriObject *)state.obj;
	return 0;
}

TriObject* OGFExp::GetTriObjectFromNode(INode *node, int &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(0).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) { 
		TriObject *tri = (TriObject *) obj->ConvertToType(0, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}
	else {
		return NULL;
	}
}

TSTR OGFExp::GetIndent(int indentLevel)
{
	TSTR indentString = "";
	for (int i=0; i<indentLevel; i++) {
		indentString += "\t";
	}
	
	return indentString;
}

BOOL OGFExp::nodeEnum(INode* node, int indentLevel) 
{
	if(exportSelected && node->Selected() == FALSE)
		return TREE_CONTINUE;

	// Stop recursing if the user pressed Cancel 
	if (ip->GetCancel())
		return FALSE;
	
	TSTR indent = GetIndent(indentLevel);
	
	// If this node is a group head, all children are 
	// members of this group. The node will be a dummy node and the node name
	// is the actualy group name.

//!!!!!!!!!!!!!
//	if (node->IsGroupHead()) {
//		fprintf(pStream,"%s%s \"%s\" {\n", indent.data(), ID_GROUP, FixupName(node->GetName())); 
//		indentLevel++;
//	}
	
	// Only export if exporting everything or it's selected
	if(!exportSelected || node->Selected()) {

		// The ObjectState is a 'thing' that flows down the pipeline containing
		// all information about the object. By calling EvalWorldState() we tell
		// max to eveluate the object at end of the pipeline.
		ObjectState os = node->EvalWorldState(0); 

		// The obj member of ObjectState is the actual object we will export.
		if (os.obj) {

			// We look at the super class ID to determine the type of the object.
			switch(os.obj->SuperClassID()) {
			case GEOMOBJECT_CLASS_ID: 
				AppendItem(node);
				break;
/*			case GEOMOBJECT_CLASS_ID: 
				if (GetIncludeObjGeom()) ExportGeomObject(node, indentLevel); 
				break;
			case CAMERA_CLASS_ID:
				if (GetIncludeObjCamera()) ExportCameraObject(node, indentLevel); 
				break;
			case LIGHT_CLASS_ID:
				if (GetIncludeObjLight()) ExportLightObject(node, indentLevel); 
				break;
			case SHAPE_CLASS_ID:
				if (GetIncludeObjShape()) ExportShapeObject(node, indentLevel); 
				break;
			case HELPER_CLASS_ID:
				if (GetIncludeObjHelper()) ExportHelperObject(node, indentLevel); 
				break;
*/			}

		}
	}	
	
	// For each child of this node, we recurse into ourselves 
	// until no more children are found.
	for (int c = 0; c < node->NumberOfChildren(); c++) {
		if (!nodeEnum(node->GetChildNode(c), indentLevel))
			return FALSE;
	}
	
	// If thie is true here, it is the end of the group we started above.
	if (node->IsGroupHead()) {
//		fprintf(pStream,"%s}\n", indent.data());
		indentLevel--;
	}

	return TRUE;
}
