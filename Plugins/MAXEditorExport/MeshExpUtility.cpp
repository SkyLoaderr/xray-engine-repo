// file: MeshExpUtility.cpp

#include "stdafx.h"
#pragma hdrstop

#include "MeshExpUtility.h"
#include "FileSystem.h"
#include "MeshExpUtility.rh"
#include "ExportObject\EditObject.h"
#include "ExportObject\EditMesh.h"

//-------------------------------------------------------------------
//  to access edit controls

static BOOL CALLBACK SimplEnumChld( HWND hwnd, LPARAM lParam ){
	HWND* phw = (HWND*)lParam;
	(*phw) = hwnd;
	return FALSE;
}

static HWND L1Child(  HWND control ){
	if( 0== control )
		return 0;
	HWND hwc = 0;
	EnumChildWindows( control, SimplEnumChld, (LPARAM)&hwc );
	return hwc;
}

static HWND L2Child( HWND control ) {
	return L1Child( L1Child( control ) );
}


//-------------------------------------------------------------------
//  Dialog Handler for Utility


static BOOL CALLBACK DefaultDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {

		case WM_INITDIALOG:
			U.Init(hWnd);
			break;

		case WM_DESTROY:
			U.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_CLOSE:
					U.iu->CloseUtility();
					break;
				case IDC_OBJECT_EXPORT:
					U.ExportAsObject();
					break;
				case IDC_SKIN_EXPORT:
					U.ExportAsSkin();
					break;
				case IDC_SKIN_KEYS_EXPORT:
					U.ExportSkinKeys();
					break;
			}
			break;


		default:
			return FALSE;
	}
	return TRUE;
}

//-------------------------------------------------------------------
//     Utility implimentations

MeshExpUtility::MeshExpUtility()
{
	iu = 0;
	ip = 0;	
	hPanel = 0;

	m_ObjectFlipFaces			= false;
	m_ObjectNoOptimize			= false;
	m_ObjectSuppressSmoothGroup = false;
	m_SkinSuppressSmoothGroup	= true;
	m_SkinProgressive			= true;

	m_ExportName[0] = 0;
}

MeshExpUtility::~MeshExpUtility()
{
}


void MeshExpUtility::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	NConsole.Init( hInstance, 0 );

	RegRead();
	FS.Init();

	hPanel = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_MWND), DefaultDlgProc, "XRay Export", 0);
}
	
void MeshExpUtility::EndEditParams(Interface *ip,IUtil *iu) 
{
	m_ObjectFlipFaces			= IsDlgButtonChecked( hPanel, IDC_OBJECT_FLIPFACES );
	m_ObjectNoOptimize			= IsDlgButtonChecked( hPanel, IDC_OBJECT_NO_OPTIMIZE );
	m_ObjectSuppressSmoothGroup	= IsDlgButtonChecked( hPanel, IDC_OBJECT_SUPPRESS_SMOOTH_GROUPS );
	m_SkinSuppressSmoothGroup	= IsDlgButtonChecked( hPanel, IDC_SKIN_SUPPRESS_SMOOTH_GROUPS );
	m_SkinProgressive			= IsDlgButtonChecked( hPanel, IDC_SKIN_PROGRESSIVE );

	RegSave();

	NConsole.Clear();
	
	this->iu = 0;
	this->ip = 0;
	ip->DeleteRollupPage(hPanel);
	hPanel = 0;
}

void MeshExpUtility::SelectionSetChanged(Interface *ip,IUtil *iu)
{
	RefreshExportList();
	UpdateSelectionListBox();
}

void MeshExpUtility::Init(HWND hWnd)
{
	hPanel = hWnd;

	hCtlList = GetDlgItem(hWnd, IDC_OBJLIST);

	CheckDlgButton( hPanel, IDC_OBJECT_FLIPFACES,				m_ObjectFlipFaces );
	CheckDlgButton( hPanel, IDC_OBJECT_NO_OPTIMIZE,				m_ObjectNoOptimize );
	CheckDlgButton( hPanel, IDC_OBJECT_SUPPRESS_SMOOTH_GROUPS,	m_ObjectSuppressSmoothGroup );
	CheckDlgButton( hPanel, IDC_SKIN_SUPPRESS_SMOOTH_GROUPS,	m_SkinSuppressSmoothGroup );
	CheckDlgButton( hPanel, IDC_SKIN_PROGRESSIVE,				m_SkinProgressive );

	RefreshExportList();
	UpdateSelectionListBox();
}

void MeshExpUtility::Destroy(HWND hWnd)
{
}

void MeshExpUtility::RefreshExportList(){
	m_Items.clear();
	ExportItem item;
	int i = ip->GetSelNodeCount();
	while( i-- ){
		item.pNode = ip->GetSelNode(i);
		m_Items.push_back( item );
	}
}

void MeshExpUtility::UpdateSelectionListBox(){
	SendMessage( hCtlList, LB_RESETCONTENT, 0, 0 );
	std::vector<ExportItem>::iterator i = m_Items.begin();
	for(;i!=m_Items.end();i++)
		SendMessage( hCtlList, LB_ADDSTRING,
			0,(LPARAM) i->pNode->GetName() );
}

/*
VOID matrix3_to_nsap ( Matrix3& M, FLOAT n[3], FLOAT s[3], FLOAT a[3], FLOAT p[3] ){
	Point3	r1				= M.GetRow(0);
	Point3	r2				= M.GetRow(1);
	Point3	r3				= M.GetRow(2);
	Point3	r4				= M.GetRow(3);
	n[0]=r1.x;	n[1]=r1.z;	n[2]=r1.y;
	s[0]=r3.x;	s[1]=r3.z;	s[2]=r3.y;
	a[0]=r2.x;	a[1]=r2.z;	a[2]=r2.y;
	p[0]=r4.x;	p[1]=r4.z;	p[2]=r4.y;
}
VOID matrix_from_nsap ( matrix& m, vector& n, vector& s, vector& a, vector& p ){
	m.m00 = n.x;		m.m01 = s.x;		m.m02 = a.x;		m.m03 = 0.0f;
	m.m10 = n.y;		m.m11 = s.y;		m.m12 = a.y;		m.m13 = 0.0f;
	m.m20 = n.z;		m.m21 = s.z;		m.m22 = a.z;		m.m23 = 0.0f;
	m.m30 = -dot(p,n);	m.m31 = -dot(p,s);	m.m32 = -dot(p,a);	m.m33 = 1.0f;	
}


VOID matrix3_to_nsap ( Matrix3& M, Fvector& n, Fvector& s, Fvector& a, Fvector& p ){
	Point3	r1				= M.GetRow(0);
	Point3	r2				= M.GetRow(1);
	Point3	r3				= M.GetRow(2);
	Point3	r4				= M.GetRow(3);
	n.set(r1.x, r1.z, r1.y);
	s.set(r3.x, r3.z, r3.y);
	a.set(r2.x, r2.z, r2.y);
	p.set(r4.x, r4.z, r4.y);
}
/*VOID matrix_from_nsap ( Fmatrix& m, Fvector& n, Fvector& s, Fvector& a, Fvector& p ){
	m.m[0][0] = n.x;				m.m[0][1] = s.x;				m.m[0][2] = a.x;				m.m[0][3] = 0.0f;
	m.m[1][0] = n.y;				m.m[1][1] = s.y;				m.m[1][2] = a.y;				m.m[1][3] = 0.0f;
	m.m[2][0] = n.z;				m.m[2][1] = s.z;				m.m[2][2] = a.z;				m.m[2][3] = 0.0f;
	m.m[3][0] = -p.dotproduct(n);	m.m[3][1] = -p.dotproduct(s);	m.m[3][2] = -p.dotproduct(a);	m.m[3][3] = 1.0f;	
}
static VOID		matr_from_nsap	( Fmatrix& m, Fvector& _n, Fvector& _s, Fvector& _a, Fvector& p ){
	FLOAT N		= _n.magnitude();//Magnitude	( _n );
	FLOAT S		= _s.magnitude();//Magnitude	( _s );
	FLOAT A		= _a.magnitude();//Magnitude	( _a );
	Fvector n,s,a;
	n.div(_n,(N*N));
	s.div(_s,(S*S));
	a.div(_a,(A*A));
	m.m[0][0] = n.x;	m.m[1][0] = n.y;	m.m[2][0] = n.z;	m.m[3][0] = -p.dotproduct(n);
	m.m[0][1] = s.x;	m.m[1][1] = s.y;	m.m[2][1] = s.z;	m.m[3][1] = -p.dotproduct(s);
	m.m[0][2] = a.x;	m.m[1][2] = a.y;	m.m[2][2] = a.z;	m.m[3][2] = -p.dotproduct(a);
	m.m[0][3] = 0.0f;	m.m[1][3] = 0.0f;	m.m[2][3] = 0.0f;	m.m[3][3] = 1.0f;	
}
*/
// ================================================== dummyFn()
// used by 3DS progress bar
static DWORD WINAPI dummyFn(LPVOID arg)
{
    return(0);
}
//-------------------------------------------------------------------

bool MeshExpUtility::SaveAsObject(const char* m_ExportName){
	bool bResult = true;

	if (m_ExportName[0]==0) return false;

	NConsole.print( "Exporting..." );
	NConsole.print( "-------------------------------------------------------" );

	char fname[256]; _splitpath( m_ExportName, 0, 0, fname, 0 );
	CEditObject* exp_obj = new CEditObject(fname);	

	std::vector<ExportItem>::iterator it = m_Items.begin();
	for(;it!=m_Items.end();it++){
		CEditMesh *submesh = new CEditMesh(exp_obj);
		if( submesh->Convert(it->pNode) ){
			// transform
			Matrix3 mMatrix;
			mMatrix = it->pNode->GetNodeTM(0)*Inverse(it->pNode->GetParentNode()->GetNodeTM(0));
			
			Fvector n,s,a,p;
			Fmatrix m;	m.identity();

			Point3	r1	= mMatrix.GetRow(0);
			Point3	r2	= mMatrix.GetRow(1);
			Point3	r3	= mMatrix.GetRow(2);
			Point3	r4	= mMatrix.GetRow(3);
			n.set(r1.x, r1.z, r1.y);
			s.set(r2.x, r2.z, r2.y);
			a.set(r3.x, r3.z, r3.y);
			p.set(r4.x, r4.z, r4.y);

			m.i.set(n);
			m.j.set(s);
			m.k.set(a);
			m.c.set(p);

			submesh->Transform( m );
			// flip faces
			Fvector v; v.crossproduct(s, a);
			if (v.dotproduct(n)<0.f) submesh->FlipFaces();
			if( m_ObjectFlipFaces )	submesh->FlipFaces();
			submesh->RecomputeBBox();
			exp_obj->AddMesh(submesh, it->pNode->GetName());
		}else{
			NConsole.print( "'%s' object can't convert", it->pNode->GetName());
			delete submesh;
			bResult = false;
			break;
		}
		NConsole.print( "-------------------------------------------------------" );
	}

	if (bResult){
		exp_obj->UpdateBox		();
		exp_obj->VerifyMeshNames();
		NConsole.print			("Object saving...");
		exp_obj->SaveObject		(m_ExportName);
		NConsole.print			("Object '%s' contains: %d points, %d faces",
								exp_obj->GetName(), exp_obj->GetVertexCount(), exp_obj->GetFaceCount());
	}

	delete exp_obj;

	return bResult;
}
//-------------------------------------------------------------------


//-------------------------------------------------------------------
//     registry access
const char *g_rOptionsKey		= "SOFTWARE\\GSC\\XRay Map Editor\\2.0\\Options";
const char *g_rPathValue		= "MainPath";
const char *g_rObjFlipFacesVal	= "Object Flip Faces";
const char *g_rObjSuppressSMVal	= "Object Suppress SM";
const char *g_rNoOptimizeVal	= "Object No Optimize";
const char *g_rSkinSuppressSMVal= "Skin Suppress SM";
const char *g_rSkinProgressive	= "Skin Progressive";

void MeshExpUtility::RegRead(){

	HKEY hk;
	DWORD keytype = REG_SZ;
	DWORD keysize = MAX_PATH;


	if( ERROR_SUCCESS==RegOpenKey( HKEY_LOCAL_MACHINE,g_rOptionsKey,&hk ) ){
		char keyvalue[32]="";
		
		keytype = REG_SZ;
		keysize = 32;
		RegQueryValueEx(hk,g_rObjFlipFacesVal,0,&keytype,(LPBYTE)keyvalue, &keysize );
		m_ObjectFlipFaces = !!atoi(keyvalue);
		RegQueryValueEx(hk,g_rObjSuppressSMVal,0,&keytype,(LPBYTE)keyvalue, &keysize );
		m_ObjectSuppressSmoothGroup = !!atoi(keyvalue);
		RegQueryValueEx(hk,g_rNoOptimizeVal,0,&keytype,(LPBYTE)keyvalue, &keysize );
		m_ObjectNoOptimize = !!atoi(keyvalue);
		RegQueryValueEx(hk,g_rSkinSuppressSMVal,0,&keytype,(LPBYTE)keyvalue, &keysize );
		m_SkinSuppressSmoothGroup = !!atoi(keyvalue);
		RegQueryValueEx(hk,g_rSkinProgressive,0,&keytype,(LPBYTE)keyvalue, &keysize );
		m_SkinProgressive = !!atoi(keyvalue);

		RegCloseKey( hk );
	}
}

void MeshExpUtility::RegSave(){

	HKEY hk;

	if( ERROR_SUCCESS==RegCreateKey( HKEY_LOCAL_MACHINE,g_rOptionsKey,&hk ) ){
		char keyvalue[32]="";

		sprintf(keyvalue,"%d",m_ObjectFlipFaces);
		RegSetValueEx(hk,g_rObjFlipFacesVal,0,REG_SZ,(LPBYTE)keyvalue, strlen(keyvalue)+1 );
		sprintf(keyvalue,"%d",m_ObjectSuppressSmoothGroup);
		RegSetValueEx(hk,g_rObjSuppressSMVal,0,REG_SZ,(LPBYTE)keyvalue, strlen(keyvalue)+1 );
		sprintf(keyvalue,"%d",m_ObjectNoOptimize);
		RegSetValueEx(hk,g_rNoOptimizeVal,0,REG_SZ,(LPBYTE)keyvalue, strlen(keyvalue)+1 );
		sprintf(keyvalue,"%d",m_SkinSuppressSmoothGroup);
		RegSetValueEx(hk,g_rSkinSuppressSMVal,0,REG_SZ,(LPBYTE)keyvalue, strlen(keyvalue)+1 );
		sprintf(keyvalue,"%d",m_SkinProgressive);
		RegSetValueEx(hk,g_rSkinProgressive,0,REG_SZ,(LPBYTE)keyvalue, strlen(keyvalue)+1 );

		RegCloseKey( hk );
	}
}

//-------------------------------------------------------------------------------------------------
void MeshExpUtility::ExportAsObject(){
	bool bResult = true;

	if( m_Items.empty() ){
		NConsole.print( "Nothing selected" );
		NConsole.print( "-------------------------------------------------------" );
		return; 
	}

	m_ObjectFlipFaces			= IsDlgButtonChecked( hPanel, IDC_OBJECT_FLIPFACES );
	m_ObjectNoOptimize			= IsDlgButtonChecked( hPanel, IDC_OBJECT_NO_OPTIMIZE );
	m_ObjectSuppressSmoothGroup	= IsDlgButtonChecked( hPanel, IDC_OBJECT_SUPPRESS_SMOOTH_GROUPS );

	char m_ExportName[MAX_PATH];
	m_ExportName[0]=0;
	if( !FS.GetSaveName(&FS.m_Import,m_ExportName) ){
		NConsole.print( "Export cancelled" );
		return;
	}

	bResult						= SaveAsObject(m_ExportName);

	NConsole.print( "-------------------------------------------------------" );
	if (bResult) NConsole.print( "Export completed" );
	else		 NConsole.print( "Export failed***********************" );
	NConsole.print( "-------------------------------------------------------" );
}
//-------------------------------------------------------------------------------------------------


void MeshExpUtility::ExportAsSkin(){
	bool bResult = true;

	if( m_Items.empty() ){
		NConsole.print( "Nothing selected." );
		NConsole.print( "-------------------------------------------------------" );
		return; 
	}
	if( m_Items.size()>1 ){
		NConsole.print( "More than one object selected." );
		NConsole.print( "-------------------------------------------------------" );
		return; 
	}

	m_SkinSuppressSmoothGroup	= IsDlgButtonChecked( hPanel, IDC_SKIN_SUPPRESS_SMOOTH_GROUPS );
	m_SkinProgressive			= IsDlgButtonChecked( hPanel, IDC_SKIN_PROGRESSIVE	 );

	char m_ExportName[MAX_PATH];
	m_ExportName[0]=0;
	if( !FS.GetSaveName(&FS.m_GameMeshes,m_ExportName) ){
		NConsole.print( "Export cancelled" );
		NConsole.print( "-------------------------------------------------------" );
		return;
	}

	bResult						= SaveAsSkin(m_ExportName);

	NConsole.print( "-------------------------------------------------------" );
	if (bResult) NConsole.print( "Export completed" );
	else		 NConsole.print( "Export failed***********************" );
	NConsole.print( "-------------------------------------------------------" );
}
//-------------------------------------------------------------------------------------------------

