#include "stdafx.h"
#pragma hdrstop

#include "ESceneAIMapTools.h"

#include "scene.h"
#include "ui_main.h"
#include "ui_tools.h"
#include "cl_intersect.h"
#include "motion_simulator.h"
#include "MgcAppr3DPlaneFit.h"

//. box????
static SPickQuery	PQ;

IC void SnapXZ	(Fvector&	V, float ps)
{
	V.x = snapto(V.x,ps);
	V.z = snapto(V.z,ps);
}
struct tri	{
	Fvector v[3];
	Fvector	N;
};

const int	RCAST_MaxTris	= (2*1024);
const int	RCAST_Count		= 6;
const int	RCAST_Total		= (2*RCAST_Count+1)*(2*RCAST_Count+1);
const float	RCAST_Depth		= 1.f;
const float RCAST_VALID 	= 0.55f;
BOOL ESceneAIMapTools::CreateNode(Fvector& vAt, SAINode& N, bool bIC)
{
	// *** Query and cache polygons for ray-casting
	Fvector	PointUp;		PointUp.set(vAt);	PointUp.y	+= RCAST_Depth;		SnapXZ	(PointUp,m_Params.fPatchSize);
	Fvector	PointDown;		PointDown.set(vAt);	PointDown.y	-= RCAST_Depth;		SnapXZ	(PointDown,m_Params.fPatchSize);

	Fbox	BB;				BB.set	(PointUp,PointUp);		BB.grow(m_Params.fPatchSize/2);	// box 1
	Fbox	B2;				B2.set	(PointDown,PointDown);	B2.grow(m_Params.fPatchSize/2);	// box 2
	BB.merge				(B2);
	Scene.BoxQuery			(PQ,BB,CDB::OPT_FULL_TEST,&m_SnapObjects);
	DWORD	dwCount 		= PQ.r_count();
	if (dwCount==0){
//		Log("chasm1");
		return FALSE;			// chasm?
	}

	// *** Transfer triangles and compute sector
	R_ASSERT(dwCount<RCAST_MaxTris);
	static svector<tri,RCAST_MaxTris> tris;		tris.clear();
	for (DWORD i=0; i<dwCount; i++)
	{
		tri&		D = tris.last();
		CDB::TRI&	T = (PQ.r_begin()+i)->T;

		D.v[0].set	(*T.verts[0]);
		D.v[1].set	(*T.verts[1]);
		D.v[2].set	(*T.verts[2]);
		D.N.mknormal(D.v[0],D.v[1],D.v[2]);
		if (D.N.y<=0)	continue;

		tris.inc	();
	}
	if (tris.size()==0){
//		Log("chasm2");
		return FALSE;			// chasm?
	}

	static svector<Fvector,RCAST_Total>	points;		points.clear();
	static svector<Fvector,RCAST_Total>	normals;	normals.clear();
	Fvector P,D; D.set(0,-1,0);

	float coeff = 0.5f*m_Params.fPatchSize/float(RCAST_Count);

	for (int x=-RCAST_Count; x<=RCAST_Count; x++) 
	{
		P.x = vAt.x + coeff*float(x);
		for (int z=-RCAST_Count; z<=RCAST_Count; z++) {
			P.z = vAt.z + coeff*float(z);
			P.y = vAt.y + 10.f;

			float	tri_min_range	= flt_max;
			int		tri_selected	= -1;
			float	range,u,v;
			for (i=0; i<DWORD(tris.size()); i++) 
			{
				if (CDB::TestRayTri(P,D,tris[i].v,u,v,range,false)) 
				{
					if (range<tri_min_range) {
						tri_min_range	= range;
						tri_selected	= i;
					}
				}
			}
			if (tri_selected>=0) {
				P.y -= tri_min_range;
				points.push_back(P);
				normals.push_back(tris[tri_selected].N);
			}
		}
	}
	if (points.size()<3) {
//		Msg		("Failed to create node at [%f,%f,%f].",vAt.x,vAt.y,vAt.z);
		return	FALSE;
	}
//.
	float rc_lim = bIC?0.5f:0.7f;
	if (float(points.size())/float(RCAST_Total) < rc_lim) {
//		Msg		("Partial chasm at [%f,%f,%f].",vAt.x,vAt.y,vAt.z);
		return	FALSE;
	}

	// *** Calc normal
	Fvector vNorm;
	vNorm.set(0,0,0);
	for (DWORD n=0; n<normals.size(); n++)
		vNorm.add(normals[n]);
	vNorm.div(float(normals.size()));
	vNorm.normalize();
	/*
	{
		// second algorithm (Magic)
		Fvector N,O;
		N.set(vNorm);
		O.set(points[0]);
		Mgc::OrthogonalPlaneFit(
			points.size(),(Mgc::Vector3*)points.begin(),
			*((Mgc::Vector3*)&O),
			*((Mgc::Vector3*)&N)
		);
		if (N.y<0) N.invert();
		N.normalize();
		vNorm.lerp(vNorm,N,.3f);
		vNorm.normalize();
	}
	*/

 
	// *** Align plane
	Fvector vOffs;
	vOffs.set(0,-1000,0);
	Fplane PL; 	PL.build(vOffs,vNorm);
	for (DWORD p=0; p<points.size(); p++)
	{
		float dist = PL.classify(points[p]);
		if (dist>0) {
			vOffs = points[p];
			PL.build(vOffs,vNorm);
		}
	}

	// *** Create node and register it
	N.Plane.build	(vOffs,vNorm);					// build plane
	D.set			(0,1,0);
	N.Plane.intersectRayPoint(PointDown,D,N.Pos);	// "project" position

	// *** Validate results
	vNorm.set(0,1,0);
	if (vNorm.dotproduct(N.Plane.n)<_cos(deg2rad(60.f)))  return FALSE;

	float y_old = vAt.y;
	float y_new = N.Pos.y;
	if (y_old>y_new) {
		// down
		if (y_old-y_new > m_Params.fCanDOWN ) return FALSE;
	} else {
		// up
		if (y_new-y_old > m_Params.fCanUP	) return FALSE;
	}
 
	// *** Validate plane
	{
		Fvector PLP; D.set(0,-1,0);
		int num_successed_rays = 0;
		for (int x=-RCAST_Count; x<=RCAST_Count; x++) 
		{
			P.x = N.Pos.x + coeff*float(x);
			for (int z=-RCAST_Count; z<=RCAST_Count; z++) {
				P.z = N.Pos.z + coeff*float(z);
				P.y = N.Pos.y;
				N.Plane.intersectRayPoint(P,D,PLP);	// "project" position
				P.y = PLP.y+RCAST_VALID*0.01f;
				
				float	tri_min_range	= flt_max;
				int		tri_selected	= -1;
				float	range,u,v;
				for (i=0; i<tris.size(); i++){
					if (CDB::TestRayTri(P,D,tris[i].v,u,v,range,false)){
						if (range<tri_min_range){
							tri_min_range	= range;
							tri_selected	= i;
						}
					}
				}
				if (tri_selected>=0){
					if (tri_min_range<RCAST_VALID) num_successed_rays++;
				}
			}
		}
		float perc = float(num_successed_rays)/float(RCAST_Total);
//.		if (!bIC&&(perc < 0.5f)){
		if (perc < 0.5f){
			//			Msg		("Floating node.");
			return	FALSE;
		}
	}

	// *** Mask check
	// ???

	return TRUE;
}

void ESceneAIMapTools::hash_Initialize()
{
	for (int i=0; i<=HDIM_X; i++)
		for (int j=0; j<HDIM_Z; j++){
//			m_HASH[i][j]			= xr_new<AINodeVec>();
			m_HASH[i][j].clear		();
			m_HASH[i][j].reserve	(64);
		}
}

void ESceneAIMapTools::hash_FillFromNodes()
{
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
		HashMap((*it)->Pos).push_back	(*it);
}

void ESceneAIMapTools::hash_Clear()
{
	for (int i=0; i<=HDIM_X; i++)
		for (int j=0; j<HDIM_Z; j++)
//			xr_delete	(m_HASH[i][j]);
			m_HASH[i][j].clear		();
}

void ESceneAIMapTools::HashRect(const Fvector& v, float radius, Irect& result)
{
	Fvector				VMmin,	VMscale, VMeps, scale;

	Fbox&				bb = m_BBox;
	VMscale.set			(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
	VMmin.set			(bb.min);
	VMeps.set			(float(VMscale.x/HDIM_X/2.f),float(0),float(VMscale.z/HDIM_Z/2.f));
	VMeps.x				= (VMeps.x<EPS_L)?VMeps.x:EPS_L;
	VMeps.y				= (VMeps.y<EPS_L)?VMeps.y:EPS_L;
	VMeps.z				= (VMeps.z<EPS_L)?VMeps.z:EPS_L;
	scale.set			(float(HDIM_X),float(0),float(HDIM_Z));
	scale.div			(VMscale);

	// Hash
	result.x1 			= iFloor((v.x-VMmin.x-radius)*scale.x); clamp(result.x1,0,HDIM_X);
	result.y1 			= iFloor((v.z-VMmin.z-radius)*scale.z);	clamp(result.y1,0,HDIM_Z);
	result.x2 			= iFloor((v.x-VMmin.x+radius)*scale.x);	clamp(result.x2,0,HDIM_X);
	result.y2 			= iFloor((v.z-VMmin.z+radius)*scale.z);	clamp(result.y2,0,HDIM_Z);
}

AINodeVec& ESceneAIMapTools::HashMap(int ix, int iz)
{
	R_ASSERT(ix<=HDIM_X && iz<=HDIM_Z);
	return m_HASH[ix][iz];
}

AINodeVec& ESceneAIMapTools::HashMap(Fvector& V)
{
	// Calculate offset,scale,epsilon
	Fvector				VMmin,	VMscale, VMeps, scale;

	Fbox&				bb = m_BBox;
	VMscale.set			(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
	VMmin.set			(bb.min);
	VMeps.set			(float(VMscale.x/HDIM_X/2.f),float(0),float(VMscale.z/HDIM_Z/2.f));
	VMeps.x				= (VMeps.x<EPS_L)?VMeps.x:EPS_L;
	VMeps.y				= (VMeps.y<EPS_L)?VMeps.y:EPS_L;
	VMeps.z				= (VMeps.z<EPS_L)?VMeps.z:EPS_L;
	scale.set			(float(HDIM_X),float(0),float(HDIM_Z));
	scale.div			(VMscale);

	// Hash
	DWORD ix,iz;
	ix = iFloor((V.x-VMmin.x)*scale.x);
	iz = iFloor((V.z-VMmin.z)*scale.z);
	R_ASSERT(ix<=HDIM_X && iz<=HDIM_Z);
	return m_HASH[ix][iz];
}

void ESceneAIMapTools::UnregisterNode(SAINode* node)
{
	VERIFY(node);
	AINodeVec& V= HashMap(node->Pos);
	for (AINodeIt I=V.begin(); I!=V.end(); I++)
    	if (node==*I){V.erase(I); return;}
}

void ESceneAIMapTools::RegisterNode(SAINode* N)
{
	m_Nodes.push_back			(N);
	HashMap(N->Pos).push_back	(N);
}

SAINode* ESceneAIMapTools::FindNode(Fvector& vAt, float eps)
{
	AINodeVec& V= HashMap(vAt);

	for (AINodeIt I=V.begin(); I!=V.end(); I++)
	{
		SAINode* N = *I;
		if (vAt.similar(N->Pos,eps)) return N;
	}
	return 0;
}
 
BOOL ESceneAIMapTools::CanTravel(Fvector _from, Fvector _at)
{
	_at.y		= _from.y;
    
	float eps	= 0.1f;
	float eps_y = m_Params.fPatchSize*1.5f; // * tan(56) = 1.5
	Fvector Result; float radius = m_Params.fPatchSize/_sqrt(2.f);

	// 1
	msimulator_Simulate(Result,_from,_at,radius,0.7f,&m_SnapObjects);
	BOOL b1 = fsimilar(Result.x,_at.x,eps)&&fsimilar(Result.z,_at.z,eps)&&fsimilar(Result.y,_at.y,eps_y);
	if (b1) return TRUE;

	// 2
	msimulator_Simulate(Result,_from,_at,radius,2.f,&m_SnapObjects);
	BOOL b2 = fsimilar(Result.x,_at.x,eps)&&fsimilar(Result.z,_at.z,eps)&&fsimilar(Result.y,_at.y,eps_y);
	if (b2) return TRUE;

	return FALSE;
}

SAINode* ESceneAIMapTools::BuildNode(Fvector& vFrom, Fvector& vAt, bool bIC)	// return node's index
{
	// *** Test if we can travel this path
	SnapXZ			(vAt,m_Params.fPatchSize);

	if (!(bIC||CanTravel(vFrom, vAt)))	return 0;

	// *** set up xr_new<node
	SAINode* N 		= xr_new<SAINode>();
	if (CreateNode(vAt,*N,bIC)) {
		//*** check if similar node exists
		SAINode* old = FindNode(N->Pos);
		if (!old){
			// register xr_new<node
			RegisterNode(N);
			return N;
		}else{
			// where already was node - return it
	    	xr_delete(N);
			return old;
		}
	}else{ 
    	xr_delete(N);
    	return 0;
    }
}

void ESceneAIMapTools::BuildNodes()
{
	// begin
	m_Nodes.reserve	(1024*1024);

	// Initialize hash
	hash_Initialize ();

	for (DWORD em=0; em<m_Emitters.size(); em++) 
	{
		// Align emitter
		Fvector			Pos = m_Emitters[em].pos;
		SnapXZ			(Pos,m_Params.fPatchSize);
		Pos.y			+=1;
		Fvector			Dir; Dir.set(0,-1,0);
		
		if (Scene.RayQuery(PQ,Pos,Dir,3,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,&m_SnapObjects)==0) {
			Msg		("Can't align emitter");
			abort	();
		} else {
			Pos.y = Pos.y - PQ.r_begin()->range;
		}
		
		// Build first node
		int oldcount 	= m_Nodes.size();
		SAINode* start 	= BuildNode(Pos,Pos,false);
		if (!start)							continue;
		if (int(m_Nodes.size())<=oldcount)	continue;

		// Estimate nodes
		Fvector	LevelSize;
		m_BBox.getsize	(LevelSize);
		float estimated_nodes	= (LevelSize.x/m_Params.fPatchSize)*(LevelSize.z/m_Params.fPatchSize);

		UI.ProgressStart		(1, "Building nodes...");
		// General cycle
		for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++){
        	SAINode* N = *it;
			// left 
			if (0==N->n1){
				Pos.set			(N->Pos);
				Pos.x			-=	m_Params.fPatchSize;
				N->n1			=	BuildNode(N->Pos,Pos,false);
			}
			// fwd
			if (0==N->n2){
				Pos.set			(N->Pos);
				Pos.z			+=	m_Params.fPatchSize;
				N->n2			=	BuildNode(N->Pos,Pos,false);
			}
			// right
			if (0==N->n3){
				Pos.set			(N->Pos);
				Pos.x			+=	m_Params.fPatchSize;
				N->n3			=	BuildNode(N->Pos,Pos,false);
			}
			// back
			if (0==N->n4){
				Pos.set			(N->Pos);
				Pos.z			-=	m_Params.fPatchSize;
				N->n4			=	BuildNode(N->Pos,Pos,false);
			}
            DWORD i = it-m_Nodes.begin();
			if (i%512==0) {
				float	p1	= float(i)/float(m_Nodes.size());
				float	p2	= float(m_Nodes.size())/estimated_nodes;
				float	p	= 0.1f*p1+0.9f*p2;

				clamp	(p,0.f,1.f);
				UI.ProgressUpdate(p);
                // check need abort && redraw
//.				if (i%4096) UI.RedrawScene(true);
                if (UI.NeedAbort()) break;
			}
		}
		UI.ProgressEnd();
	}
}

SAINode* ESceneAIMapTools::GetNode(Fvector vAt, bool bIC)	// return node's index
{
	// *** Test if we can travel this path
	SnapXZ			(vAt,m_Params.fPatchSize);

	// *** set up xr_new<node
	SAINode* N 		= xr_new<SAINode>();
    SAINode* R		= 0;
	if (CreateNode(vAt,*N,bIC)){
		R 			= FindNode(N->Pos);
    	xr_delete	(N);
    }
	xr_delete(N);
    return R;
}

void ESceneAIMapTools::UpdateLinks(SAINode* N, bool bIC)
{
	Fvector 	Pos;
    SAINode* 	D;
    // left 
    {
        Pos.set			(N->Pos);
        Pos.x			-=	m_Params.fPatchSize;
        D				= GetNode(Pos,bIC);
        if (bIC||CanTravel(N->Pos, Pos)) 			N->n1 = D;
        if (D&&(bIC||CanTravel(D->Pos, N->Pos))) 	D->n3 = N;
    }
    // fwd
    {
        Pos.set			(N->Pos);
        Pos.z			+=	m_Params.fPatchSize;
        D				= GetNode(Pos,bIC);
        if (bIC||CanTravel(N->Pos, Pos)) 			N->n2 = D;
        if (D&&(bIC||CanTravel(D->Pos, N->Pos))) 	D->n4 = N;
    }
    // right
    {
        Pos.set			(N->Pos);
        Pos.x			+=	m_Params.fPatchSize;
        D				= GetNode(Pos,bIC);
        if (bIC||CanTravel(N->Pos, Pos)) 			N->n3 = D;
        if (D&&(bIC||CanTravel(D->Pos, N->Pos))) 	D->n1 = N;
    }
    // back
    {
        Pos.set			(N->Pos);
        Pos.z			-=	m_Params.fPatchSize;
        D				= GetNode(Pos,bIC);
        if (bIC||CanTravel(N->Pos, Pos)) 			N->n4 = D;
        if (D&&(bIC||CanTravel(D->Pos, N->Pos))) 	D->n2 = N;
    }
}

bool ESceneAIMapTools::GenerateMap()
{
	bool bRes = false;
	if (!m_SnapObjects.empty()){
        if (!m_Emitters.empty()){
            // clear
            Clear				(true);
                
            // building
            Scene.lock			();
            BuildNodes			();
            Scene.unlock		();
                
            Scene.UndoSave		();
            bRes = true;
        }else{
            ELog.DlgMsg(mtError,"Append at least one emitter and try again!");
        }
    }else{ 
    	ELog.DlgMsg(mtError,"Fill snap list and activate before generating slots!");
    }
    return bRes;
}

bool ESceneAIMapTools::FillSnapList(ObjectList* lst)
{
    hash_Clear					();
	if (lst){ 	
    	m_SnapObjects = *lst;
        Fbox bb;
        Scene.GetBox(bb,m_SnapObjects);
        if (!m_BBox.similar(bb)){
            ELog.Msg(mtError,"AIMap: Bounding volume changed. Please verify map.");
            m_BBox.set(bb);
        }
	    hash_FillFromNodes		();
    }else{
    	m_BBox.invalidate		();
    	m_SnapObjects.clear		();
    }
    return true;
}

void ESceneAIMapTools::RemoveLinks()
{
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
    	if ((*it)->flags.is(SAINode::flSelected)){
            for (int k=0; k<4; k++) 
		    	if ((*it)->n[k]&&(*it)->n[k]->flags.is(SAINode::flSelected))
        	    	(*it)->n[k] = 0;
        }
    UpdateHLSelected	();
}

static const int opposite[4]={2,3,0,1};
static const int fl[4]		={SAINode::flN1,SAINode::flN2,SAINode::flN3,SAINode::flN4};
void ESceneAIMapTools::InvertLinks()
{
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
    	if ((*it)->flags.is(SAINode::flSelected))
        	for (int k=0; k<4; k++)
                if ((*it)->n[k]&&(*it)->n[k]->flags.is(SAINode::flSelected)&&!(*it)->flags.is(fl[k])){
                    if (0==(*it)->n[k]->n[opposite[k]]){ 
                        (*it)->n[k]->n[opposite[k]] 		= (*it);
                        (*it)->n[k]->flags.set(fl[opposite[k]],TRUE);
                        (*it)->n[k]	= 0;
                    }
                }
    // reset processing flag
	for (AINodeIt a_it=m_Nodes.begin(); a_it!=m_Nodes.end(); a_it++)
		(*a_it)->flags.set(SAINode::flN1|SAINode::flN2|SAINode::flN3|SAINode::flN4,FALSE);
    UpdateHLSelected	();
}

SAINode* ESceneAIMapTools::FindNeighbor(SAINode* N, int side)
{
	Fvector Pos;
	Pos.set			(N->Pos);
    SnapXZ			(Pos,m_Params.fPatchSize);
    switch (side){
	case 0: Pos.x -= m_Params.fPatchSize; break;
	case 1: Pos.z += m_Params.fPatchSize; break;
	case 2: Pos.x += m_Params.fPatchSize; break;
	case 3: Pos.z -= m_Params.fPatchSize; break;
    }
    AINodeVec& nodes	= HashMap(Pos);
	for (AINodeIt I=nodes.begin(); I!=nodes.end(); I++)
        if (fsimilar((*I)->Pos.x,Pos.x)&&fsimilar((*I)->Pos.z,Pos.z)) return *I;
    return 0;
}

static AINodeVec g_nodes;
void ESceneAIMapTools::MakeLinks(u8 side_flag, EMode mode)
{
	if (!side_flag) return;
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++){
    	SAINode* T 						= *it;
    	if ((*it)->flags.is(SAINode::flSelected)){
        	for (int k=0; k<4; k++){
            	if (!(side_flag&fl[k])) continue;
            	switch (mode){
                case mdAppend:{ 
                    SAINode* S 				= FindNeighbor(T,k);
                    if (S) T->n[k] = S;
                }break;
                case mdRemove:{ 
                    SAINode* S 				= FindNeighbor(T,k);
                    if (S) T->n[k] = 0;
                }break;
                case mdInvert:{ 
                    SAINode* S 				= FindNeighbor(T,k);
                    if (S){
                    	if (!T->flags.is(fl[k])){ 
                            if (T->n[k]&&S->n[opposite[k]]) continue;
                            SAINode* a			= T->n[k];
                            T->n[k] 			= S->n[opposite[k]];
                            S->n[opposite[k]]	= a;
                        }
	                    S->flags.set(fl[opposite[k]],TRUE);
                    }
                }break;
                }
            }
        }
    }
    // reset processing flag
	for (AINodeIt a_it=m_Nodes.begin(); a_it!=m_Nodes.end(); a_it++)
		(*a_it)->flags.set(SAINode::flN1|SAINode::flN2|SAINode::flN3|SAINode::flN4,FALSE);
    UpdateHLSelected	();
}
/*
void ESceneAIMapTools::MakeLinks(bool bUp, bool bDown)
{
    // copy selected nodes
    g_nodes.clear();
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
    	if ((*it)->flags.is(SAINode::flSelected)) g_nodes.push_back(*it);
    // make links
	for (it=g_nodes.begin(); it!=g_nodes.end(); it++){
    	SAINode* T 						= *it;
    	for (int k=0; k<4; k++){
        	if (!T->flags.is(fl[k])){
                SAINode* N 					= FindNeighbor(T,g_nodes,k);
                if (N){
                    if (bUp&&bDown){
                        T->n[k] 			= N; T->flags.set(fl[k],TRUE);
                        N->n[opposite[k]] 	= T; N->flags.set(fl[opposite[k]],TRUE);
                    }else{
                        if (bUp){
                        	if (T->Pos.y<N->Pos.y){T->n[k] = N;/ N->n[opposite[k]]=0;/}
                        }
                        if (bDown)
                        	if (T->Pos.y>N->Pos.y){T->n[k] = N;/ N->n[opposite[k]]=0;/}
                    }
                }
                T->flags.set(fl[k],TRUE);
            }
        }
    }
    // reset processing flag
	for (it=g_nodes.begin(); it!=g_nodes.end(); it++)
		(*it)->flags.set(SAINode::flN1|SAINode::flN2|SAINode::flN3|SAINode::flN4,FALSE);

    UpdateHLSelected	();
}
*/

