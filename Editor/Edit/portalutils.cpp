//----------------------------------------------------
// file: PortalUtils.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "portalutils.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Portal.h"
#include "Sector.h"
#include "EditMesh.h"
#include "EditObject.h"
#include "UI_Main.h"

CPortalUtils PortalUtils;
#define EPS_P 0.001f

void CPortalUtils::RemoveSectorPortal(CSector* S){
	UI->BeginEState(esSceneLocked);

    // remove existence sector portal
    ObjectList& lst = Scene->ListObj(OBJCLASS_PORTAL);
    ObjectIt _F = lst.begin();
    while(_F!=lst.end()){
    	CPortal* P=(CPortal*)(*_F);
        if((P->m_SectorFront==S)||(P->m_SectorBack==S)){
            _DELETE((*_F));
            ObjectIt _D = _F; _F++;
            lst.remove((*_D));
        }else{
            _F++;
        }
    }

	UI->EndEState();
}

void CPortalUtils::FindSVertexLinks(int sid, CSector* S, SVertexVec& V){
    Fmatrix parent;
    int idx[3];
    Fvector p[3];
	for (SItemIt s_it=S->sector_items.begin();s_it!=S->sector_items.end();s_it++){
		s_it->GetTransform(parent);
		FvectorVec& m_vert=s_it->mesh->m_Points;
        for (DWORDIt f_it=s_it->Face_IDs.begin(); f_it!=s_it->Face_IDs.end(); f_it++){
        	st_Face& P=s_it->mesh->m_Faces[*f_it];
           	for (int k=0; k<3; k++) parent.transform_tiny(p[k],m_vert[P.pv[k].pindex]);
            idx[0]=idx[1]=idx[2]=-1;
            for (SVertexIt pv_it=V.begin(); pv_it!=V.end(); pv_it++){
            	int id=pv_it-V.begin();
            	if (p[0].similar(*pv_it,EPS_P)) idx[0]=id;
                else if (p[1].similar(*pv_it,EPS_P)) idx[1]=id;
                else if (p[2].similar(*pv_it,EPS_P)) idx[2]=id;
            }
            if (idx[0]>=0) V[idx[0]].SetLink(sid, idx[1],idx[2]);
            if (idx[1]>=0) V[idx[1]].SetLink(sid, idx[0],idx[2]);
            if (idx[2]>=0) V[idx[2]].SetLink(sid, idx[0],idx[1]);
        }
    }
}
/*
int CPortalUtils::CalculatePortals(CSector* SF, CSector* SB){
    VERIFY(SF!=SB);
    int bResult=0;
    if (!SF->SpherePick(SB->m_SectorCenter,SB->m_SectorRadius,precalc_identity)) return bResult;
	UI->BeginEState(esSceneLocked);
	if (SF->m_bNeedUpdateCHull) SF->MakeCHull();
	if (SB->m_bNeedUpdateCHull) SB->MakeCHull();
//////////////////////////////////////////////////
    // remove existence sector portal
    ObjectList& lst = Scene->ListObj(OBJCLASS_PORTAL);
    ObjectIt _F = lst.begin();
    while(_F!=lst.end()){
    	CPortal* P=(CPortal*)(*_F);
        if(((P->m_SectorFront==SF)&&(P->m_SectorBack==SB))||
           ((P->m_SectorFront==SB)&&(P->m_SectorBack==SF))){
            _DELETE((*_F));
            ObjectIt _D = _F; _F++;
            lst.remove((*_D));
        }else{
            _F++;
        }
    }

	// merge sectors geometry
	SVertexList V;

    // find similar vertices
    for (FvectorIt sf_it=SF->m_SectorVertices.begin(); sf_it!=SF->m_SectorVertices.end(); sf_it++)
	    for (FvectorIt sb_it=SB->m_SectorVertices.begin(); sb_it!=SB->m_SectorVertices.end(); sb_it++)
        	if (sf_it->similar(*sb_it,EPS_P)){
            	bool bPresent=false;
			    for (SVertexIt pv_it=V.begin(); pv_it!=V.end(); pv_it++)
		        	if (pv_it->similar(*sf_it,EPS_P)){ bPresent=true; break; }
                if (!bPresent) V.push_back(SVertex(*sf_it));
                break;
            }

	// find used edges
	FindSVertexLinks(0, SF,V);
	FindSVertexLinks(1, SB,V);

    // find consolidate link
	for (SVertexIt v=V.begin(); v<V.end(); v++){
     	v->ConsolidateLink();
    }

    // create portals
    WORDList wl;
    int portal_num=0;
    bool bRight=false;
	for (DWORD i=0; i<V.size(); i++){
    	if (V[i].portal==-1){
            V[i].portal=portal_num;
            if (bRight) wl.insert(wl.begin(),i);
            else		wl.push_back(i);
            int cur_i=V[i].ulink[0]; // левая ветка
            int prev_i=i;
            while(true){
            	try{
					V[cur_i].portal=portal_num;
                }catch(...){
				    Log->DlgMsg(mtError,"Portal can't create, because has some errors.\nCheck geometry.\n'%s'<->'%s'",SF->GetName(),SB->GetName());
					UI->EndEState();
                	return 0;
                }
	            if (bRight) wl.insert(wl.begin(),cur_i);
    	        else		wl.push_back(cur_i);
            	if (V[cur_i].ulink.size()==2){
                    int next_i=(prev_i!=V[cur_i].ulink[1])?V[cur_i].ulink[1]:V[cur_i].ulink[0];
                    prev_i=cur_i;
                    cur_i=next_i;
                    if (V[cur_i].portal>=0) break; // закончился цикл выходим
                }else{
	                if ((V[i].ulink.size()==2)&&!bRight){
						cur_i=V[i].ulink[1]; // правая ветка
	                    prev_i=i;
                        bRight=true;
                    }else break;
                }
            }
		    if (wl.size()>2){
                // append portal
                char namebuffer[MAX_OBJ_NAME];
                Scene->GenObjectName( OBJCLASS_PORTAL, namebuffer );
                CPortal* _O = new CPortal(namebuffer);
                for (DWORD i=0; i<wl.size(); i++) _O->m_Vertices.push_back(V[wl[i]]);
                _O->m_SectorFront=SF;
                _O->m_SectorBack=SB;
                _O->Update();
                if (_O->m_Valid){
	                bResult++;
    	            Scene->AddObject(_O);
                }else{
                	delete _O;
				    Log->DlgMsg(mtError,"Portal can't create, because has some errors.\nCheck geometry.\n'%s'<->'%s'",SF->GetName(),SB->GetName());
                }
            }else
			    if (wl.size()<3) Log->DlgMsg(mtError,"Portal can't create, because has error: '< 3 edges'.\nCheck geometry.\n'%s'<->'%s'",SF->GetName(),SB->GetName());

            wl.clear();
            portal_num++;
        }
    }

	UI->EndEState();
    Scene->UndoSave();
    return bResult;
}
//---------------------------------------------------------------------------
*/
int CPortalUtils::CalculatePortals(char* s_front, char* s_back){
	CSector* SF;
	CSector* SB;
	SF=(CSector*)Scene->FindObjectByName(s_front,OBJCLASS_SECTOR);
	SB=(CSector*)Scene->FindObjectByName(s_back,OBJCLASS_SECTOR);
    return CalculatePortals(SF,SB);
}
//---------------------------------------------------------------------------

void CPortalUtils::RemoveAllPortals(){
	UI->BeginEState(esSceneLocked);

    // remove all existence portal
	ObjectList& p_lst = Scene->ListObj(OBJCLASS_PORTAL);
    for (ObjectIt _F=p_lst.begin(); _F!=p_lst.end(); _F++) delete (*_F);
	p_lst.erase(p_lst.begin(),p_lst.end());

	UI->EndEState();
}
//---------------------------------------------------------------------------

bool CPortalUtils::CreateDefaultSector(){
	UI->BeginEState(esSceneLocked);

    Fbox box;
	if (Scene->GetBox(box,OBJCLASS_EDITOBJECT)){
		CSector* sector_def=new CSector(DEFAULT_SECTOR_NAME);
        sector_def->sector_color.set(1,0,0,0);
        sector_def->m_bDefault=true;
        sector_def->CaptureAllUnusedFaces();
		if (sector_def->GetSectorFacesCount()>0){
         	Scene->AddObject(sector_def,false);
            Scene->UndoSave();
	        UI->UpdateScene();
			UI->EndEState();
            return true;
        } else delete sector_def;
    }
	UI->EndEState();
    return false;
}
//---------------------------------------------------------------------------

bool CPortalUtils::RemoveDefaultSector(){
	UI->BeginEState(esSceneLocked);
    SceneObject* O=Scene->FindObjectByName(DEFAULT_SECTOR_NAME,OBJCLASS_SECTOR);
    if (O){
    	Scene->RemoveObject(O,false);
        _DELETE(O);
		Scene->UndoSave();
		UI->EndEState();
        UI->UpdateScene();
        return true;
    }
	UI->EndEState();
    return false;
}
//---------------------------------------------------------------------------

int CPortalUtils::CalculateAllPortals2(){
	UI->BeginEState(esSceneLocked);

    int bResult=0;
    if (Validate(false)){
        RemoveAllPortals();
        // calculate 0-sector
//        RemoveDefaultSector();
//        CreateDefaultSector();

        // transfer from list to vector
        ObjectList& s_lst=Scene->ListObj(OBJCLASS_SECTOR);
        vector<CSector*> sectors;
        for(ObjectIt _F=s_lst.begin(); _F!=s_lst.end(); _F++) sectors.push_back((CSector*)(*_F));

        // calculate all portals
        for (DWORD f=0; f<sectors.size()-1; f++)
            for (DWORD b=f+1; b<sectors.size(); b++){
                bResult+=CalculatePortals(sectors[f],sectors[b]);
                AnsiString t;
                t.sprintf("Calculate %d of %d",f,sectors.size());
                UI->SetStatus(t.c_str());
            }

        Scene->UndoSave();
    }else{
		Log->DlgMsg(mtError,"*ERROR: Scene has non associated face!");
    }

	UI->SetStatus("...");
	UI->EndEState();
    return bResult;
}
//---------------------------------------------------------------------------

bool CPortalUtils::IsFaceUsed(CEditObject* o, CEditMesh* m, DWORD f_id){
	ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
	ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++) if (((CSector*)(*_F))->IsFaceUsed(o,m,f_id)) return true;
    return false;
}

// remove used from list
void CPortalUtils::TestUsedFaces(CEditObject* o, CEditMesh* m, DWORDVec& fl){
	ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
	ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++)
    	((CSector*)(*_F))->TestUsedFaces(o,m,fl);
}


bool CPortalUtils::Validate(bool bMsg){
	UI->BeginEState(esSceneLocked);

    Fbox box;
    bool bResult = false;
	if (Scene->GetBox(box,OBJCLASS_EDITOBJECT)){
		CSector* sector_def=new CSector(DEFAULT_SECTOR_NAME);
        sector_def->CaptureAllUnusedFaces();
        int f_cnt=sector_def->GetSectorFacesCount();
		if (f_cnt!=0){	if (bMsg) Log->DlgMsg(mtError,"*ERROR: Scene has '%d' non associated face!",f_cnt);
        }else{
			if (bMsg) Log->DlgMsg(mtInformation,"Validation OK!");
            bResult = true;
        }
        if (f_cnt&&Log->DlgMsg(mtConfirmation,"ERROR: Scene has '%d' non associated face.\nPrint errors?",f_cnt)==mrYes){
        	const Fvector* PT[3];
	        for (SItemIt it=sector_def->sector_items.begin(); it!=sector_def->sector_items.end(); it++){
            	Log->Msg(mtError,"Object: %s", it->object->GetName());
                for (DWORDIt dw_it=it->Face_IDs.begin(); dw_it!=it->Face_IDs.end(); dw_it++){
					it->mesh->GetFacePT(*dw_it, PT);
	            	Log->Msg(mtError," PT: [%3.2f, %3.2f, %3.2f]", PT[0]->x, PT[0]->y, PT[0]->z);
                }
            }
        }
        _DELETE(sector_def);

        // verify sectors
        ObjectList& s_lst=Scene->ListObj(OBJCLASS_SECTOR);
        for(ObjectIt _F=s_lst.begin(); _F!=s_lst.end(); _F++){
        	if (((CSector*)(*_F))->GetSectorFacesCount()<=4){
            	bResult=false;
                break;
            }
        }
    }else{
		if (bMsg) Log->DlgMsg(mtInformation,"Validation failed! Can't compute bbox.");
    }
	UI->EndEState();
    return bResult;
}
    
void CPortalUtils::CreateDebugCollection(){
	VERIFY((Scene->ObjCount(OBJCLASS_SECTOR)==0)&&(Scene->ObjCount(OBJCLASS_PORTAL)==0));

    UI->ProgressStart(6,"Create debug sectors and portal...");
	UI->ProgressInc();
   
    // create default sector
    CreateDefaultSector();
    CSector* DEF=(CSector*)Scene->FindObjectByName(DEFAULT_SECTOR_NAME,OBJCLASS_SECTOR);
    if (!DEF) return;
	UI->ProgressInc();

	// create debug object                       
    CEditObject* O = new CEditObject("$debug_object_0x247d05e9");
    AnsiString fn;
    fn = "$debug_sector.object";
    FS.m_Objects.Update(fn);
    if (!O->Load(fn.c_str())){
    	Log->DlgMsg(mtError, "Can't find object '%s'",fn.c_str());
    	_DELETE(O);
	    UI->ProgressEnd();
        return;
    }
	UI->ProgressInc();
    
    Fbox lev_box, obj_box;
    Fvector offs; offs.set(0,0,0);
	if (Scene->GetBox(lev_box,OBJCLASS_EDITOBJECT)){
    	offs.set(lev_box.min);
        O->GetBox(obj_box);
        offs.sub(obj_box.getradius());
    }
    O->Move(offs);
    Scene->AddObject(O,false);
	UI->ProgressInc();
    // create debug sector
    CSector* S = new CSector("$debug_sector_0x247d05e9");
    S->CaptureAllUnusedFaces();
    Scene->AddObject(S,false);
	UI->ProgressInc();
    // create debug portal
    CPortal* P = new CPortal("$debug_portal_0x247d05e9");
    P->m_SectorFront = DEF;
    P->m_SectorBack  = S;
    P->m_Center.set(0,0,0);
    P->m_SimplifyVertices.resize(3);
    P->m_SimplifyVertices[0].set(0,0,0);
    P->m_SimplifyVertices[1].set(1,0,0);
    P->m_SimplifyVertices[2].set(0,1,0);
    P->Move(offs);
    Scene->AddObject(P,false);
	UI->ProgressInc();

	UI->ProgressEnd();
}

//--------------------------------------------------------------------------------------------------
// calculate portals
//--------------------------------------------------------------------------------------------------

const int clpMX = 64, clpMY=24, clpMZ=64;

class sCollector
{
    struct sFace
    {
        DWORD v[3];
        DWORD sector;

        bool	hasVertex(DWORD vert)
        {
            return (v[0]==vert)||(v[1]==vert)||(v[2]==vert);
        }
    };
    struct sVert : public Fvector
    {
        DWORDVec adj;
    };
    struct sEdge
    {
        DWORD s[2];
        DWORD v[2];
        DWORD used;
        DWORD dummy;
    
        sEdge() { used=false; }
	    static bool c_less(const sEdge& E1, const sEdge& E2)
        {
            if (E1.s[0]<E2.s[0]) return true;
            if (E1.s[1]<E2.s[1]) return true;
            if (E1.v[0]<E2.v[0]) return true;
            if (E1.v[1]<E2.v[1]) return true;
            return false;
        }
	    static bool c_equal(const sEdge& E1, const sEdge& E2)
        {
	        return (E1.s[0]==E2.s[0])&&(E1.s[1]==E2.s[1])&&(E1.v[0]==E2.v[0])&&(E1.v[1]==E2.v[1]);
        }
        static int compare( const void *arg1, const void *arg2 )
		{
        	return memcmp(arg1,arg2,2*2*4);
            /*
	        sEdge& E1=*((sEdge*)arg1);
	        sEdge& E2=*((sEdge*)arg2);
            if (c_less(E1,E2)) 	return -1;
            if (c_equal(E1,E2)) return 0;
            return 1;
            */
		}
    };
   	struct sPortal
    {
        deque<int> e;
        DWORD s[2];
    };

    DEFINE_VECTOR(sVert, sVertVec, sVertIt);
    DEFINE_VECTOR(sFace, sFaceVec, sFaceIt);
    DEFINE_VECTOR(sEdge, sEdgeVec, sEdgeIt);
    DEFINE_VECTOR(sPortal, sPortalVec, sPortalIt);
public:
    sVertVec		verts;
    sFaceVec		faces;
    sEdgeVec		edges;
   	sPortalVec	 	portals;

    Fvector			VMmin, VMscale;
    DWORDVec		VM[clpMX+1][clpMY+1][clpMZ+1];
    Fvector			VMeps;

    DWORD			VPack(Fvector& V){
        DWORD P = 0xffffffff;
		
        DWORD ix,iy,iz;
        ix = floorf(float(V.x-VMmin.x)/VMscale.x*clpMX);
        iy = floorf(float(V.y-VMmin.y)/VMscale.y*clpMY);
        iz = floorf(float(V.z-VMmin.z)/VMscale.z*clpMZ);
        R_ASSERT(ix<=clpMX && iy<=clpMY && iz<=clpMZ);
		
        {
            DWORDVec* vl; 
            vl = &(VM[ix][iy][iz]);
            for(DWORDIt it=vl->begin();it!=vl->end(); it++)
                if( verts[*it].similar(V) )	{
                    P = *it;
                    break;
                }
        }
        if (0xffffffff==P){
            P = verts.size();
            sVert sV; sV.set(V);
            verts.push_back	(sV);
			
            VM[ix][iy][iz].push_back(P);
			
            DWORD ixE,iyE,izE;
            ixE = floorf(float(V.x+VMeps.x-VMmin.x)/VMscale.x*clpMX);
            iyE = floorf(float(V.y+VMeps.y-VMmin.y)/VMscale.y*clpMY);
            izE = floorf(float(V.z+VMeps.z-VMmin.z)/VMscale.z*clpMZ);
			
            R_ASSERT(ixE<=clpMX && iyE<=clpMY && izE<=clpMZ);
			
            if (ixE!=ix)							VM[ixE][iy][iz].push_back	(P);
            if (iyE!=iy)							VM[ix][iyE][iz].push_back	(P);
            if (izE!=iz)							VM[ix][iy][izE].push_back	(P);
            if ((ixE!=ix)&&(iyE!=iy))				VM[ixE][iyE][iz].push_back	(P);
            if ((ixE!=ix)&&(izE!=iz))				VM[ixE][iy][izE].push_back	(P);
            if ((iyE!=iy)&&(izE!=iz))				VM[ix][iyE][izE].push_back	(P);
            if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))	VM[ixE][iyE][izE].push_back	(P);
        }
        return P;
    }
        
    sCollector(const Fbox &bb){
        VMscale.set	(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
        VMmin.set	(bb.min);
        VMeps.set	(VMscale.x/clpMX/2,VMscale.y/clpMY/2,VMscale.z/clpMZ/2);
        VMeps.x		= (VMeps.x<EPS_L)?VMeps.x:EPS_L;
        VMeps.y		= (VMeps.y<EPS_L)?VMeps.y:EPS_L;
        VMeps.z		= (VMeps.z<EPS_L)?VMeps.z:EPS_L;
    }
		
    void			add_face(
        Fvector& v0,    // vertices
        Fvector& v1, 
        Fvector& v2,	
        DWORD sector
        )
    {
        sFace T;
        T.v[0] 	= VPack(v0);
        T.v[1] 	= VPack(v1);
        T.v[2] 	= VPack(v2);
        T.sector= sector;
        faces.push_back(T);
    }
    void update_adjacency(){
    	for (DWORD i=0; i<faces.size(); i++){
        	sFace& F=faces[i];
			verts[F.v[0]].adj.push_back(i);
			verts[F.v[1]].adj.push_back(i);
			verts[F.v[2]].adj.push_back(i);
        }
    }
    void find_edges(){
    	for (DWORD i=0; i<faces.size(); i++){
        	sFace& F=faces[i];
            DWORDIt a_it;
            sVert& v0=verts[F.v[0]];
            sVert& v1=verts[F.v[1]];
            sVert& v2=verts[F.v[2]];
            
            // 1 pair (0-1)
            for (a_it=v0.adj.begin(); a_it!=v0.adj.end(); a_it++) {
            	if (*a_it==i) continue;

                sFace& T = faces[*a_it];
                if (T.sector==F.sector) continue;

                if (!T.hasVertex(F.v[1])) continue;

                sEdge E;
                E.v[0]=F.v[0];
                E.v[1]=F.v[1];
                E.s[0]=F.sector;
                E.s[1]=T.sector;
                edges.push_back(E);
            }
            // 2 pair (1-2)
            for (a_it=v1.adj.begin(); a_it!=v1.adj.end(); a_it++) {
            	if (*a_it==i) continue;

                sFace& T = faces[*a_it];
                if (T.sector==F.sector) continue;

                if (!T.hasVertex(F.v[2])) continue;

                sEdge E;
                E.v[0]=F.v[1];
                E.v[1]=F.v[2];
                E.s[0]=F.sector;
                E.s[1]=T.sector;
                edges.push_back(E);
            }
            // 3 pair (2-0)
            for (a_it=v2.adj.begin(); a_it!=v2.adj.end(); a_it++) {
            	if (*a_it==i) continue;

                sFace& T = faces[*a_it];
                if (T.sector==F.sector) continue;

                if (!T.hasVertex(F.v[0])) continue;

                sEdge E;
                E.v[0]=F.v[2];
                E.v[1]=F.v[0];
                E.s[0]=F.sector;
                E.s[1]=T.sector;
                edges.push_back(E);
            }
        }
    }
    void dump_edges() {
       	Log->Msg(mtInformation,"********* dump");
    	for (DWORD i=0; i<edges.size(); i++){
        	sEdge& E = edges[i];
        	Log->Msg(mtInformation,"%d: %d,%d",i,E.v[0],E.v[1]);
        }
    }
    void sort_edges(){
    	// sort inside edges
    	for (DWORD i=0; i<edges.size(); i++){
        	sEdge& E = edges[i];
            if (E.v[0]>E.v[1]) swap(E.v[0],E.v[1]);
            if (E.s[0]>E.s[1]) swap(E.s[0],E.s[1]);
        }
        
        // remove equal
        qsort(edges.begin(),edges.size(),sizeof(sEdge),sEdge::compare);
        sEdgeIt NewEnd = std::unique(edges.begin(),edges.end(),sEdge::c_equal);
        edges.erase(NewEnd,edges.end());
		dump_edges();
    }
    void make_portals() {
        for(DWORD e_it=0; e_it<edges.size(); e_it++) 
        {
        	Log->Msg(mtInformation,"%d: %d,%d",e_it,edges[e_it].v[0],edges[e_it].v[1]);
        	if (edges[e_it].used) continue;
            
            sPortal current;
            current.e.push_back (e_it);
            current.s[0] = edges[e_it].s[0];
            current.s[1] = edges[e_it].s[1];
            edges[e_it].used= true;

            for (;;) {
                sEdge& 	eFirst 	= edges[current.e[0]];
                sEdge& 	eLast  	= edges[current.e.back()];
                DWORD	vFirst	= eFirst.v[0];
                DWORD	vLast	= eLast.v[1];
                bool 	bFound 	= false;
                for (DWORD i=0; i<edges.size(); i++)
                {
                    sEdge& E = edges[i];
                    if (E.used)					continue;
                    if (E.s[0]!=current.s[0]) 	continue;
                    if (E.s[1]!=current.s[1]) 	continue;

                    if (vLast ==E.v[0]) { E.used=true; current.e.push_back(i); bFound=true; break; }
                    if (vLast ==E.v[1]) { E.used=true; swap(E.v[0],E.v[1]); current.e.push_back (i); bFound=true; break; }
                    if (vFirst==E.v[0]) { E.used=true; swap(E.v[0],E.v[1]); current.e.push_front(i); bFound=true; break; }
                    if (vFirst==E.v[1]) { E.used=true; current.e.push_front(i); bFound=true; break; }
                }
                if (!bFound) break;
            }
            portals.push_back(current);
        }
    }
    void export_portals(vector<CSector*>& Sectors)
    {
    	for (sPortalIt p_it=portals.begin(); p_it!=portals.end(); p_it++){
		    if (p_it->e.size()>1)
            {
            	// build vert-list
                vector<int> 	vlist;
                deque<int>&		elist=p_it->e;
                vlist.reserve	(elist.size()*2);
                for (deque<int>::iterator e=elist.begin(); e!=elist.end(); e++)
                {
                	vlist.push_back(edges[*e].v[0]);
                	vlist.push_back(edges[*e].v[1]);
                }
                INTIt end = std::unique(vlist.begin(), vlist.end());
                vlist.erase(end,vlist.end());
            
                // append portal
                char namebuffer[MAX_OBJ_NAME];
                Scene->GenObjectName( OBJCLASS_PORTAL, namebuffer );
                CPortal* _O = new CPortal(namebuffer);
                for (DWORD i=0; i<vlist.size(); i++) {
	                _O->Vertices().push_back(verts[vlist[i]]);
                }
                _O->SetSectors(Sectors[p_it->s[0]],Sectors[p_it->s[1]]);
                _O->Update();
                if (_O->Valid()){
	 	            Scene->AddObject(_O);
                }else{
                	delete _O;
				    Log->DlgMsg(mtError,"Can't simplify Portal :(\nPlease check geometry once more.\n'%s'<->'%s'",Sectors[p_it->s[0]]->GetName(),Sectors[p_it->s[1]]->GetName());
                }
            }else
			    Log->DlgMsg(mtError,"Can't create Portal from one edge :(\nPlease check geometry once more.\n'%s'<->'%s'",Sectors[p_it->s[0]]->GetName(),Sectors[p_it->s[1]]->GetName());
        }
    }
};

int CPortalUtils::CalculateSelectedPortals(vector<CSector*>& sectors){
    // calculate portals
    Fbox bb;
    Scene->GetBox(bb,OBJCLASS_EDITOBJECT);
    sCollector* CL = new sCollector(bb);
    Fmatrix T;
        
    //1. xform + weld
    UI->SetStatus("xform + weld...");
    for (DWORD i=0; i<sectors.size(); i++){
        CSector* S=sectors[i];
        for (SItemIt s_it=S->sector_items.begin();s_it!=S->sector_items.end();s_it++){
            s_it->GetTransform(T);
            FvectorVec& m_vert=s_it->mesh->m_Points;
            for (DWORDIt f_it=s_it->Face_IDs.begin(); f_it!=s_it->Face_IDs.end(); f_it++){
                Fvector v0, v1, v2;
                st_Face& P=s_it->mesh->m_Faces[*f_it];
                T.transform_tiny(v0,m_vert[P.pv[0].pindex]);
                T.transform_tiny(v1,m_vert[P.pv[1].pindex]);
                T.transform_tiny(v2,m_vert[P.pv[2].pindex]);
                CL->add_face(v0,v1,v2,i);
            }
        }
    }
    //2. update pervertex adjacency
    UI->SetStatus("updating per-vertex adjacency...");
    CL->update_adjacency();
    //3. find edges
    UI->SetStatus("searching edges...");
    CL->find_edges();
    //4. sort edges
    UI->SetStatus("sorting edges...");
    CL->sort_edges();
    //5. make portals
    UI->SetStatus("calculating portals...");
    CL->make_portals();
    //6. export portals
    UI->SetStatus("building portals...");
    CL->export_portals(sectors);

    Scene->UndoSave();

    int iRes = CL->portals.size();

    _DELETE(CL);

    return iRes;
}

int CPortalUtils::CalculateAllPortals(){
	UI->BeginEState(esSceneLocked);

    int iPCount=0;
    if (Validate(false)){
		UI->SetStatus("Prepare...");
        RemoveAllPortals();
        // transfer from list to vector
        vector<CSector*> sectors;
        ObjectList& s_lst=Scene->ListObj(OBJCLASS_SECTOR);
        for(ObjectIt _F=s_lst.begin(); _F!=s_lst.end(); _F++) sectors.push_back((CSector*)(*_F));

        iPCount = CalculateSelectedPortals(sectors);
    }else{
		Log->DlgMsg(mtError,"*ERROR: Scene has non associated face!");
    }

	UI->SetStatus("...");
	UI->EndEState();
    return iPCount;
}

int CPortalUtils::CalculatePortals(CSector* SF, CSector* SB){
	UI->BeginEState(esSceneLocked);

    int iPCount=0;
    if (Validate(false)){
		UI->SetStatus("Prepare...");
        RemoveAllPortals();
        // transfer from list to vector
        vector<CSector*> sectors;
        sectors.push_back(SF);
        sectors.push_back(SB);

        iPCount = CalculateSelectedPortals(sectors);
    }else{
		Log->DlgMsg(mtError,"*ERROR: Scene has non associated face!");
    }

	UI->SetStatus("...");
	UI->EndEState();
    return iPCount;
}
