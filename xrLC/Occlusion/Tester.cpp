// Tester.cpp: implementation of the CTester class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "malloc.h"

#include "Tester.h"
#include "fbasicvisual.h"
#include "fhierrarhyvisual.h"
#include "ftimer.h"
#include "ffileops.h"
#include "visibilitytester.h"

#define LOG(a)
#define Layer2H(dwLayer) \
	((float(dwLayer)-float(MODE.dwVisLevels)*(1.f/3.f) + .5f)*vHeader.fSize)


CTester*	pTester = NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

float CTester::GetProgress()
{
	DWORD _total	= vHeader.wSizeZ*vHeader.wSizeX;
	DWORD _cur		= vZ*vHeader.wSizeX+vX;
	return float(_cur)/float(_total);
}


#define WRITE(a,b,c) fwrite(b,c,1,a)
CTester::CTester()
{
	LoadVisuals			();
	LoadVisibility		();
	AnalyzeVisibility	();
	ExpandVisuals		();
	StartClipper		();
	BuildCTable			();
	
	ZeroMemory(visible, sizeof(visible));
	vX	= vZ	= 0;	cam = fv_num = 0;
	dwLayer		= 0;

	NewSlot		();
	bDone		= false;

	//	Open vis list
	char name[_MAX_PATH];
	strcpy(name,MODE.folder);
	strcat(name,"map.");
	hMap = fopen(name,"wb");

	// ..and map
	strcpy(name,MODE.folder);
	strcat(name,"visibility.");
	hVis = fopen(name,"wb");

	_VIS_HEADER2 V = vHeader;
	V.wLayers = MODE.dwVisLevels;
	WRITE(hMap, &V, sizeof(vHeader));

	for (int i=0; i<MODE.dwVisLevels; i++) {
		float p1 = (i==0)?-100:Layer2H(i)-vHeader.fSize/2;
		float p2 = (i==(MODE.dwVisLevels-1))?100:Layer2H(i)+vHeader.fSize/2;
		Msg("Layer %d at point [%3.3f,%3.3f]",i,p1,p2);
	}
}

CTester::~CTester()
{
	_FREE(pVis);
	_FREE(pMap);
}

void CTester::LoadVisuals()
{
	VC.Load();
	VC.PerformOptimize();
}

void CTester::LoadVisibility()
{
	char name[128];
	char newname[_MAX_PATH];
	CTimer t;
	
	Log("Loading visibility list...");
	strcpy(name,MODE.folder);
	strcat(name,"visibility.");
	pVis=(WORD *)DownloadFile(name);
	strcpy(newname,MODE.folder);
	strcat(newname,"visibility.bak");
	rename(name,newname);

	Log("Loading visibility map...");
	t.Start();
	strcpy(name,MODE.folder);
	strcat(name,"map.");
	pMap=(_VIS_SLOT2 *)DownloadFile(name);
	CopyMemory(&vHeader,pMap,sizeof(_VIS_HEADER2));
	VERIFY(vHeader.wFormatVersion==sizeof(vHeader));
	for (DWORD i=0; i<vHeader.wSizeZ; i++) {
		for (DWORD j=0; j<vHeader.wSizeX; j++) {
			for (DWORD k=0; k<vHeader.wLayers; k++) {
				_VIS_SLOT2 *slot = 
					(_VIS_SLOT2 *)( DWORD(pMap) + DWORD(sizeof(vHeader)) + 
						DWORD(	i*vHeader.wSizeX*vHeader.wLayers+
								j*vHeader.wLayers + 
								k)*sizeof(_VIS_SLOT2));
				slot->dwPtr += DWORD(pVis);
			}
		}
	}
	t.Log();

	strcpy(newname,MODE.folder);
	strcat(newname,"map.bak");
	rename(name,newname);
}

void CTester::AddVTest(FBasicVisual* pV)
{
	if (pV) {
		if (pV->Type==MT_HIERRARHY) {
			Msg("  * Visual %s unrolled...",pV->fName);
			VInvalidated.push_back(pV);
			FHierrarhyVisual *pH = (FHierrarhyVisual *)pV;
			for (int i=0; i<pH->chields.size(); i++)
				AddVTest(pH->chields[i]);
		} else {
			VTest.push_back(pV);
		}
	}
}
void CTester::AnalyzeVisibility()
{
	Log("Analyzing base visibility info...");
	Log("* Warning: assuming vis-info totally incomplete!");
	ZeroMemory(visible, sizeof(visible));
	Fvector p; 
	WORD*	v;
	p.set(0,0,0);
	int C = GetSlot(p,&v);
	for (int i=0; i<C; i++)
		visible[v[i]]=true;

	// Build 'TEST' list of visuals
	for (i=0; i<C*2; i++)
	{
		if (visible[i]) AddVTest(VC.Get(i));
	}
	ZeroMemory(visible, sizeof(visible));
	Msg("* %d visuals after hierrarhy unrolling (was %d)",VTest.size(),C);
}
void CTester::ExpandVisuals()
{
	Log("Splitting visuals to improve consistency...");
	Log("* ERROR: not implemented yet.");
}
void CTester::StartClipper()
{
	Log("Starting FrustumClipper...");
	Clipper.Init();
	for (int i=0; i<VTest.size(); i++) {
		FBasicVisual* pV = VTest[i];
		Clipper.Add(pV, pV->vPosition, pV->fRadius);
	}
}

int CTester::GetSlot(Fvector &pos, WORD ** ptr) {
	// 2D Selection
	int px=int(pos.x/vHeader.fSize);
	int pz=int(pos.z/vHeader.fSize);
	if (px<0) px=0;
	if (pz<0) pz=0;
	if (WORD(px)>=vHeader.wSizeX) px=vHeader.wSizeX-1;
	if (WORD(pz)>=vHeader.wSizeZ) pz=vHeader.wSizeZ-1;

	// Height
	float range = vHeader.wLayers;
	float lower = range  * (1.f/3.f);
	float upper = range  * (2.f/3.f);
	float _py   = lower+pos.y/vHeader.fSize;
	int		py	= int(_py);
	if	(py<0)					py = 0;
	if	(py>=vHeader.wLayers)	py=vHeader.wLayers-1;

	// Slot
	_VIS_SLOT2 *slot = (_VIS_SLOT2 *)( 
		DWORD(pMap) + DWORD(sizeof(vHeader)) + DWORD(
			pz*vHeader.wSizeX*vHeader.wLayers+
			px*vHeader.wLayers+
			py
		)*sizeof(_VIS_SLOT2));
	*ptr = (WORD *)slot->dwPtr;
	return int(slot->dwCnt);
}

#define CAM_MAX			6
struct	_camdef {
	Fvector vAt;
	Fvector vUp;
} cam_def[CAM_MAX] = {
	{ {0,0,1}, {0,1,0} },	// forward
	{ {0,0,-1},{0,1,0} },	// backward
	{ {-1,0,0},{0,1,0} },	// left
	{ {1,0,0}, {0,1,0} },	// right
	{ {0,1,0}, {0,0,1} },	// up
	{ {0,-1,0},{0,0,1} }	// down
};
#define FV_MAX	6
Fvector	fv_def[FV_MAX] = {
	{-1,  0, -1},
	{-1,  0,  1},
	{ 1,  0, -1},
	{ 1,  0,  1},
	{ 0,  1,  0},
	{ 0, -1,  0}
};

void CTester::OnRender()
{
	LOG("Matrices");
	// build matrices
	Fmatrix mProject,mView;
	Fvector from,vAt;

	from.set(vFrom);
	if (MODE.bFullVolume) {
		Fvector v;
		v.set(fv_def[fv_num]);
		v.mul(.25f);
		from.add(v);
	}
	vAt.add(from, cam_def[cam].vAt);
	mView.build_camera(from, vAt, cam_def[cam].vUp );

	// render all
	Clipper.Calculate();
	for (int i=0; i<Clipper.Count; i++) {
		if (Clipper.Test(i)) {
			Device()->SetRenderState(D3DRENDERSTATE_AMBIENT,	CTable[i]);
			FBasicVisual* pV = (FBasicVisual*)Clipper.Object[i];
			pV->Render(1.f);
		}
	}
}

extern LPDIRECTDRAWSURFACE7 BackBuffer();
void CTester::OnSurfaceReady()
{
	LOG("BBuf");
	// check back buffer
	LPDIRECTDRAWSURFACE7	pSurface = BackBuffer();
	DDSURFACEDESC2			picDesc;
	
    picDesc.dwSize = sizeof(picDesc);
	CHK_DX(pSurface->Lock(0,&picDesc,DDLOCK_WAIT|DDLOCK_READONLY|DDLOCK_NOSYSLOCK,0));
	
	VERIFY(picDesc.dwWidth	==MODE.dwVisSize);
	VERIFY(picDesc.dwHeight ==MODE.dwVisSize);
	
	VERIFY(picDesc.ddpfPixelFormat.dwRGBBitCount == 16);
	
	VERIFY(picDesc.lpSurface );
	
	DWORD*	ptr;
	int		idx;
	DWORD	pixel;
	for (int y=0; y<MODE.dwVisSize; y++) {
		ptr = LPDWORD(LPWORD(picDesc.lpSurface)+y*MODE.dwVisSize);
		for (int x=0; x<MODE.dwVisSize/8; x++)
		{
			pixel	= *ptr++;
			idx		= (pixel&0xffff)/INC - 1;
			if (idx>=0 && idx<Clipper.Count) visible[idx]= true;
			idx		= (pixel>>16)/INC - 1;
			if (idx>=0 && idx<Clipper.Count) visible[idx]= true;

			pixel	= *ptr++;
			idx		= (pixel&0xffff)/INC - 1;
			if (idx>=0 && idx<Clipper.Count) visible[idx]= true;
			idx		= (pixel>>16)/INC - 1;
			if (idx>=0 && idx<Clipper.Count) visible[idx]= true;

			pixel	= *ptr++;
			idx		= (pixel&0xffff)/INC - 1;
			if (idx>=0 && idx<Clipper.Count) visible[idx]= true;
			idx		= (pixel>>16)/INC - 1;
			if (idx>=0 && idx<Clipper.Count) visible[idx]= true;

			pixel	= *ptr++;
			idx		= (pixel&0xffff)/INC - 1;
			if (idx>=0 && idx<Clipper.Count) visible[idx]= true;
			idx		= (pixel>>16)/INC - 1;
			if (idx>=0 && idx<Clipper.Count) visible[idx]= true;
		}
	}
	
	pSurface->Unlock(0);
	
	LOG("Ok");
	// switch camera
	NextCamera();
}

void CTester::NextCamera()
{
	LOG("NextCamera");
	// change camera
	cam++;
	if (cam>=CAM_MAX) {
		cam = 0;
		if (MODE.bFullVolume) {
			fv_num++;
			if (fv_num>=FV_MAX) {
				fv_num=0;
				ChangeSlot();
			}
		} else {
			ChangeSlot();
		}
	}
	LOG("Ok");
}

Fvector cameraPos;
bool __fastcall v_sort(WORD a, WORD b)
{
	FBasicVisual*	v1 = (FBasicVisual*)Clipper.Object[a];
	FBasicVisual*	v2 = (FBasicVisual*)Clipper.Object[b];
	return	cameraPos.distance_to_sqr(v1->vPosition)<
			cameraPos.distance_to_sqr(v2->vPosition);
}

void CTester::ChangeSlot()
{
	// save slot
	vis.clear();
	for (int k=0; k<VTest.size(); k++) 
		if (visible[k]) vis.push_back(k);

	STAT.dwClipped+=VTest.size()-vis.size();
	ZeroMemory(visible, sizeof(visible));
	// sort visuals
	// 'cause transparency is detected at render time - 
	//		don't mention it
	cameraPos.set(vFrom);
	std::sort(vis.begin(),vis.end(),v_sort);
	
	// map
	DWORD pos = ftell(hVis);
	WRITE(hMap,&pos,4);
	DWORD cnt = vis.size();
	WRITE(hMap,&cnt,4);
	
	// the 'vis' itself
	WORD* p = vis.begin();
	if (cnt && p) {
		WRITE(hVis, p, cnt*2);
	}
	
	// change slot
	dwLayer++;
	if (dwLayer>=MODE.dwVisLevels) {
		vX++; dwLayer=0;
		if (vX>=vHeader.wSizeX) {
			vZ++; vX=0;
			if (vZ>=vHeader.wSizeZ) {
				SaveVisibility();
				bDone = true;
			}
		}
	}
	
	// new slot
	NewSlot();
}
void CTester::NewSlot()
{
	vFrom.set(
		(float(vX)+.5f)*vHeader.fSize,
		Layer2H(dwLayer),
		(float(vZ)+.5f)*vHeader.fSize
		);
	slot_count = GetSlot(vFrom, &slot_indices);
	STAT.dwSlots++;
}

void CTester::SaveVisibility()
{ 
	Log("Shutting down all subsystems and saving 'meshlist'...");

	fclose(hVis);
	fclose(hMap);

	// Saving mesh list
	char Mname[_MAX_PATH];
	strcpy(Mname,MODE.folder);
	strcat(Mname,"meshlist.");
	int hM = _open(Mname,O_WRONLY|O_CREAT|O_BINARY,S_IREAD|S_IWRITE);

	vector<FBasicVisual*>	meshlist;
	meshlist.insert(meshlist.begin(),VTest.begin(),VTest.end());

	int k;
	for (int i=0; i<VC.Items.size(); i++) 
	{
		FBasicVisual*	pV = VC.Items[i];
		if (pV==0) goto main_continue;

		// check that it isn't in INVALIDATED list
		for (k=0; k<VInvalidated.size(); k++)
			if (VInvalidated[k]==pV) goto main_continue;

		// check that it isn't in READY list
		for (k=0; k<meshlist.size(); k++)
			if (meshlist[k]==pV) goto main_continue;

		// all tests passed - add it to mesh list
		Log("* Non static visual added: ",pV->fName);
		meshlist.push_back(pV);
main_continue: 
		;
	}

	// Real writting
	DWORD Count = meshlist.size();
	_write(hM,&Count,4);
	for (i=0; i<Count; i++)
	{
		char *name = meshlist[i]->fName;
		_write(hM,name,strlen(name)+1);
	}
	_close(hM);
}
