/*
*  Progressive Mesh type Polygon Reduction Algorithm
*  by Stan Melax (c) 1998, Oles Shishkovtsov  (c) 2001
*  Permission to use any of this code wherever you want is granted..
*  Although, please do acknowledge authorship if appropriate.
*
*  See the header file progmesh.h for a description of this module
*/

#include "stdafx.h"
#include "progmesh.h"
#include "vector.h"

struct P_params {
	DWORD	dwUVMASK;
	DWORD	dwUVCount;
	DWORD	dwMinVerts;
	float	wUV,wPOS,wNORM;
	float	fBorderH;
	float	fBorderDistanceLimit;
	float	fQualityH;
} PARA;

List<P_Vertex *>	vertices;
List<P_Triangle *>	triangles;

void P_Triangle::_Construct(void)
{
	ComputeNormal();
	triangles.Add(this);
	for(int i=0;i<3;i++) {
		vertex[i]->face.Add(this);
		for(int j=0;j<3;j++) if(i!=j) {
			vertex[i]->neighbor.AddUnique(vertex[j]);
		}
	}
}

P_Triangle::P_Triangle(P_Vertex *v0,P_Vertex *v1,P_Vertex *v2){
	VERIFY(v0!=v1 && v1!=v2 && v2!=v0);
	vertex[0]=v0;
	vertex[1]=v1;
	vertex[2]=v2;
	_Construct();
}
P_Triangle::P_Triangle(int v0, int v1, int v2){
	VERIFY(v0!=v1 && v1!=v2 && v2!=v0);
	vertex[0]=vertices[v0];
	vertex[1]=vertices[v1];
	vertex[2]=vertices[v2];
	_Construct();
}
P_Triangle::~P_Triangle(){
	int i;
	triangles.Remove(this);
	for(i=0;i<3;i++) {
		if(vertex[i]) vertex[i]->face.Remove(this);
	}
	for(i=0;i<3;i++) {
		int i2 = (i+1)%3;
		if(!vertex[i] || !vertex[i2]) continue;
		vertex[i ]->RemoveIfNonNeighbor(vertex[i2]);
		vertex[i2]->RemoveIfNonNeighbor(vertex[i ]);
	}
}
int P_Triangle::HasVertex(P_Vertex *v) {
	return (v==vertex[0] ||v==vertex[1] || v==vertex[2]);
}
void P_Triangle::ComputeNormal(){
	Vector v0=vertex[0]->position;
	Vector v1=vertex[1]->position;
	Vector v2=vertex[2]->position;
	normal = (v1-v0)*(v2-v1);
	if(magnitude(normal)==0)return;
	normal = normalize(normal);
}
void P_Triangle::ReplaceVertex(P_Vertex *vold,P_Vertex *vnew) {
	VERIFY(vold && vnew);
	VERIFY(vold==vertex[0] || vold==vertex[1] || vold==vertex[2]);
	VERIFY(vnew!=vertex[0] && vnew!=vertex[1] && vnew!=vertex[2]);
	if(vold==vertex[0]){
		vertex[0]=vnew;
	}
	else if(vold==vertex[1]){
		vertex[1]=vnew;
	}
	else {
		VERIFY(vold==vertex[2]);
		vertex[2]=vnew;
	}
	int i;
	vold->face.Remove(this);
	VERIFY(!vnew->face.Contains(this));
	vnew->face.Add(this);
	for(i=0;i<3;i++) {
		vold->RemoveIfNonNeighbor(vertex[i]);
		vertex[i]->RemoveIfNonNeighbor(vold);
	}
	for(i=0;i<3;i++) {
		VERIFY(vertex[i]->face.Contains(this)==1);
		for(int j=0;j<3;j++) if(i!=j) {
			vertex[i]->neighbor.AddUnique(vertex[j]);
		}
	}
	ComputeNormal();
}

P_Vertex::P_Vertex(Vector v,int _id,P_UV *_uv) {
	bBorder			= false;
	bCriticalBorder = false;	
	position		= v;
	id				= _id;
	CopyMemory		(uv,_uv,PARA.dwUVCount*sizeof(P_UV));
	vertices.Add	(this);
}
P_Vertex::~P_Vertex(){
	VERIFY	(face.num()==0);
	while	(neighbor.num()) {
		neighbor[0]->neighbor.Remove(this);
		neighbor.Remove(neighbor[0]);
	}
	vertices.Remove(this);
}
void P_Vertex::RemoveIfNonNeighbor(P_Vertex *n) {
	// removes n from neighbor list if n isn't a neighbor.
	if(!neighbor.Contains(n)) return;
	for(int i=0;i<face.num();i++) {
		if(face[i]->HasVertex(n)) return;
	}
	neighbor.Remove(n);
}
//----------------------------------------------------------------------------
bool P_Vertex::LiesOnBorder() {
	int i,j;
	for(i=0;i<neighbor.num();i++) {
		int count=0;
		for(j=0;j<face.num();j++) {
			if(face[j]->HasVertex(neighbor[i])) {
				count++;
			}
		}
		VERIFY(count>0);
		if(count==1) return true;
	}
	return false;
} 
float DistanceToLineSegment(
							Vector& rkPoint, 
							Vector& segP1,
							Vector& segP2)
{
	Vector	segDir	= segP2-segP1;
    Vector	kDiff	= rkPoint - segP1;
    float	fT		= kDiff ^ segDir;
	
    if ( fT <= 0.0 )
    {
        fT = 0.0;
    }
    else
    {
        float fSqrLen= magnitude_sqr(segDir);
        if ( fT >= fSqrLen )
        {
            fT = 1.0;
            kDiff = kDiff - segDir;
        }
        else
        {
            fT /= fSqrLen;
            kDiff = kDiff - fT * segDir;
        }
    }
	
    return magnitude(kDiff);
}
void P_Vertex::OptimizeBorder()
{
	if (!bBorder) return;
	else bCriticalBorder = true;
	vector<int> nb; 
	
	for(int i=0;i<neighbor.num();i++) if (neighbor[i]->bBorder) nb.push_back(i);
	if (nb.size()!=2) return;
	
	Vector &P1	= neighbor[nb[0]]->position;
	Vector &P2	= neighbor[nb[1]]->position;
	
	Vector	E1		= position-P1;
	Vector	E2		= position-P2;
	float	angle	= normalize(E1) ^ normalize(E2);
	if (angle<PARA.fBorderH) {
		bCriticalBorder = false;
		
		float	A	= magnitude(E1);
		float	B	= magnitude(E2);
		float	C	= magnitude(P2-P1);
		float	X	= (A*A - B*B + C*C)/(2*C);
		float	h2	= A*A - X*X;
		if (h2<0)	return;
		float	h	= sqrtf(h2);
		if (h>PARA.fBorderDistanceLimit) bCriticalBorder = true;
	}
} 
//----------------------------------------------------------------------------
float ComputeEdgeCollapseCost(P_Vertex *u,P_Vertex *v) {
	static List<P_Triangle *> sides;
	
	// if we collapse edge uv by moving u to v then how 
	// much different will the model change, i.e. how much "error".
	// Texture, vertex normal, and border vertex code was removed
	// to keep this demo as simple as possible.
	// The method of determining cost was designed in order 
	// to exploit small and coplanar regions for
	// effective polygon reduction.
	// It is possible to add some checks here to see if "folds"
	// would be generated.  i.e. normal of a remaining face gets
	// flipped.  I never seemed to run into this problem and
	// therefore never added code to detect this case.
	
	int i;
	float edgelength	= magnitude(v->position - u->position);
	float curvature		= 0.0001f;
	
	// find the "sides" triangles that are on the edge uv
	for(i=0;i<u->face.num();i++) {
		if(u->face[i]->HasVertex(v)){
			sides.Add(u->face[i]);
		}
	}
	// use the triangle facing most away from the sides 
	// to determine our curvature term
	// the more coplanar the lower the curvature term
	for(i=0;i<u->face.num();i++) {
		float mincurv=1; // curve for face i and closer side to it
		for(int j=0;j<sides.num();j++) {
			// use dot product of face normals. '^' defined in vector
			float dotprod = u->face[i]->normal ^ sides[j]->normal;
			mincurv =  _MIN(mincurv,(1.0002f-dotprod)/2.0f);
		}
		curvature = _MAX(curvature,mincurv);
	}
	
	// check for border to interior collapses
	//	if(u->IsBorder() && sides.num>1) {
	//		curvature=1;
	//	} 
	
	// LockDown border edges (i.e. try not to touch them at all)
	// this is done by locking any border vertex.
	// i.e. even if uv isn't a border edge we dont want to callapse u to any vertex
	// if u is on a border
	if((u->bBorder && !v->bBorder) || u->bCriticalBorder) {
		curvature += 0.01f;		// make sure, that even if it is ZERO it will become NONZERO
		curvature *= 9999.9f;	// scale some more to collapse this vertex sometimes later
	}
	
	// Compute UV collapse cost
	float uv_collapse	= .0f;
	float uv_num		= .0f;
	for (DWORD T=0; T<PARA.dwUVCount; T++)
	{
		DWORD mask= 1<<T;
		if (PARA.dwUVMASK & mask) {
			uv_collapse += u->uv[T].dist(v->uv[T]);
			uv_num		+= 1.f;
		}
	}
	uv_collapse/=uv_num;
	
	sides.clear();
	return 
		powf(edgelength,PARA.wPOS) * 
		powf(curvature,PARA.wNORM) * 
		powf(uv_collapse,PARA.wUV);
}

void ComputeEdgeCostAtVertex(P_Vertex *v) {
	// compute the edge collapse cost for all edges that start
	// from vertex v.  Since we are only interested in reducing
	// the object by selecting the min cost edge at each step, we
	// only cache the cost of the least cost edge at this vertex
	// (in member variable collapse) as well as the value of the 
	// cost (in member variable objdist).
	if(v->neighbor.num==0) {
		// v doesn't have neighbors so it costs nothing to collapse
		v->collapse=NULL;
		v->objdist=-0.01f;
		return;
	}
	v->objdist = 1000000;
	v->collapse=NULL;
	// search all neighboring edges for "least cost" edge
	for(int i=0;i<v->neighbor.num();i++) {
		float dist;
		dist = ComputeEdgeCollapseCost(v,v->neighbor[i]);
		if(dist<v->objdist) {
			v->collapse=v->neighbor[i];  // candidate for edge collapse
			v->objdist=dist;             // cost of the collapse
		}
	}
}
void ComputeAllEdgeCollapseCosts() {
	// For all the edges, compute the difference it would make
	// to the model if it was collapsed.  The least of these
	// per vertex is cached in each vertex object.
	for(int i=0;i<vertices.num();i++) {
		ComputeEdgeCostAtVertex(vertices[i]);
	}
}
void Collapse(P_Vertex *u,P_Vertex *v){
	static List<P_Vertex *> tmp;
	
	// Collapse the edge uv by moving vertex u onto v
	// Actually remove tris on uv, then update tris that
	// have u to have v, and then remove u.
	if(!v) {
		// u is a vertex all by itself so just delete it
		delete u;
		return;
	}
	int i;
	// make tmp a list of all the neighbors of u
	for(i=0;i<u->neighbor.num();i++) {
		tmp.Add(u->neighbor[i]);
	}
	// delete triangles on edge uv:
	for(i=u->face.num()-1;i>=0;i--) {
		if(u->face[i]->HasVertex(v)) {
			delete(u->face[i]);
		}
	}
	// update remaining triangles to have v instead of u
	for(i=u->face.num()-1;i>=0;i--) {
		u->face[i]->ReplaceVertex(u,v);
	}
	delete u; 
	// recompute the edge collapse costs for neighboring vertices
	for(i=0;i<tmp.num();i++) {
		ComputeEdgeCostAtVertex(tmp[i]);
	}
	
	tmp.clear();
}

P_Vertex *MinimumCostEdge(){
	// Find the edge that when collapsed will affect model the least.
	// This funtion actually returns a P_Vertex, the second vertex
	// of the edge (collapse candidate) is stored in the vertex data.
	// Serious optimization opportunity here: this function currently
	// does a sequential search through an unsorted list :-(
	// Our algorithm could be O(n*lg(n)) instead of O(n*n)
	P_Vertex *mn=vertices[0];
	for(int i=0;i<vertices.num();i++) {
		if(vertices[i]->objdist < mn->objdist) {
			mn = vertices[i];
		}
	}
	return mn;
}

void ProgressiveMesh(List<int> &PM, List<int> &permutation)
{
	ComputeAllEdgeCollapseCosts();		// cache all edge collapse costs
	permutation.SetSize	(vertices.num()); // allocate space
	PM.SetSize			(vertices.num()); // allocate space
	
	// reduce the object down to nothing:
	while(vertices.num() > 0) {
		// get the next vertex to collapse
		P_Vertex *mn = MinimumCostEdge();
		// keep track of this vertex, i.e. the collapse ordering
		permutation[mn->id]=vertices.num()-1;
		// keep track of vertex to which we collapse to
		PM[vertices.num()-1] = (mn->collapse)?mn->collapse->id:-1;
		// Collapse this edge
		Collapse(mn,mn->collapse);
	}
	
	// reorder the map list based on the collapse ordering
	for(int i=0;i<PM.num();i++) {
		PM[i] = (PM[i]==-1)?0:permutation[PM[i]];
	}
	
	// The caller of this function should reorder their vertices
	// according to the returned "permutation".
}

PM_API void __cdecl PM_CreateVertex(float x, float y, float z,int _id, P_UV *uv)
{
	P_Vertex	*v	= new P_Vertex(Vector(x,y,z),_id,uv);
}


PM_API void __cdecl PM_Options(
							   DWORD	dwRelevantUV, DWORD dwRelevantUVMASK,
							   DWORD	dwMinVertCount, 
							   float	w_UV,
							   float	w_Pos,
							   float    w_Norm,
							   float	p_BorderHeuristic,
							   float	p_BorderHeuristicD,
							   float	p_QualityHeuristic
							   )
{
	PARA.dwUVCount	= dwRelevantUV;
	PARA.dwUVMASK	= dwRelevantUVMASK;
	PARA.dwMinVerts	= dwMinVertCount;
	PARA.wUV		= w_UV;
	PARA.wPOS		= w_Pos;
	PARA.wNORM		= w_Norm;
	PARA.fBorderH	= cosf(p_BorderHeuristic*(3.1415926535897932384626433832795f)/180.f);
	PARA.fBorderDistanceLimit = p_BorderHeuristicD;
	PARA.fQualityH	= p_QualityHeuristic;
}

void DumpIDX(WORD* P, DWORD C)
{
/*
R_ASSERT(C%3 == 0);
C/=3;
Msg("**** DUMP ****\n");
for (DWORD I=0; I<C; I++)
Msg("%3d %3d %3d",P[I*3+0],P[I*3+1],P[I*3+2]);
	*/
}

/*
__declspec(dllimport) void __cdecl Msg(const char* format,...);
*/

PM_API int		__cdecl PM_Convert(
								   WORD*			pIndices,	
								   DWORD			idxCount,
								   PM_Result*		RESULT
								   )
{
	static vector<WORD>		pmPERMUTE;
	static vector<Vsplit>	pmSPLIT;
	static vector<WORD>		pmFACE_FIX;
	
	pmPERMUTE.clear		();
	pmSPLIT.clear		();
	pmFACE_FIX.clear	();
	
	ZeroMemory			(RESULT,sizeof(*RESULT));
	if (idxCount<32*3)	return -1;

	// Build triangle list (and create list of adjacent faces at vertices)
	for (DWORD I=0; I<idxCount; I+=3)	
		P_Triangle *t= new P_Triangle(
		pIndices[I+0],
		pIndices[I+1],
		pIndices[I+2]);
	
	// 1. Detect borders
	// 2. Optimize them, allowing some of 'border' vertices to collapse
	for (I=0; I<DWORD(vertices.num()); I++) vertices[I]->DetectBorder	();
	for (I=0; I<DWORD(vertices.num()); I++) vertices[I]->OptimizeBorder	();
	
	// Calculate minimal vertices and FAIL if needed
	DWORD dwRealMin	= 0, dwBorderMin = 0, dwVertCount=vertices.num(); 
	for (I=0; I<dwVertCount; I++) {
		if (vertices[I]->bBorder)	{
			dwBorderMin++;
			dwRealMin++;
			vertices[I]->bCriticalBorder = TRUE;
		}
		//if (vertices[I]->bCriticalBorder)	dwRealMin++;
	}
	
	dwRealMin = _MAX(PARA.dwMinVerts,dwRealMin);
//	float fQuality = float(dwRealMin)/vertices.num();
	if ((vertices.num()-dwRealMin) < 8) 
	{
		vertices.clear	();
		triangles.clear	();
		return -1;
	}
	
	//****** Calculate mesh
	List<int> Vperm;
	List<int> Vcollapses;  // to which neighbor each vertex collapses
	ProgressiveMesh(Vcollapses,Vperm);
	
	//****** transfer permute
	pmPERMUTE.clear();
	for (I=0; I<DWORD(Vperm.num()); I++) pmPERMUTE.push_back(WORD(DWORD(Vperm[I])));
	
	//****** Update the changes in the entries in the triangle list
	for (I=0; I<idxCount; I++)
		pIndices[I] = Vperm[pIndices[I]];
	
	// Convert indices
	vector<WORD>	Indices;
	Indices.resize	(idxCount);
	CopyMemory(Indices.begin(),pIndices,idxCount*sizeof(WORD));
	
	// Perform all collapses and build face permute table
	vector<DWORD> deg;
	for (int V_Num = Vperm.num()-1; V_Num>=0; V_Num--)
	{
		// perform collapse
		WORD V_New			= Vcollapses[V_Num];
		
		// modify indices
		for (DWORD I_Num=0; I_Num<Indices.size(); I_Num++)
		{
			if (Indices[I_Num]==V_Num) 
				Indices[I_Num]=V_New;
		}
		
		// now let's see how many triangles becames degenerated
		for (DWORD T=0; T<Indices.size(); T += 3) {
			if (
				(Indices[T+0]==Indices[T+1]) ||
				(Indices[T+1]==Indices[T+2]) ||
				(Indices[T+0]==Indices[T+2]) )
			{
				// triangle degenerated
				DWORD T_ID = T/3;
				if (find(deg.begin(),deg.end(),T_ID)==deg.end())
				{
					// only now became degenerated so record it
					deg.push_back(T_ID);
					//					Msg("deg: %d",T_ID);
				}
			}
		}
	}
	
	// Permute faces
	R_ASSERT(deg.size()==(idxCount/3));
	CopyMemory(Indices.begin(),pIndices,idxCount*sizeof(WORD));
	reverse(deg.begin(),deg.end());
	{
		for (DWORD d=0; d<deg.size(); d++) {
			DWORD base0= d*3;
			DWORD base = deg[d]*3;
			for (I=0; I<3; I++) {
				pIndices[base0+I]=Indices[base+I];
			}
		}
	}
	
	DumpIDX(pIndices,idxCount);
	
	// Build tables
	pmSPLIT.clear	();
	pmFACE_FIX.clear();
	int I_Current	= idxCount;
	CopyMemory(Indices.begin(),pIndices,idxCount*2);
	for (V_Num = Vperm.num()-1; V_Num>=int(dwRealMin); V_Num--)
	{
		Vsplit				S;
		
		// perform collapse
		WORD V_New			= Vcollapses[V_Num];
		R_ASSERT			(V_New!=V_Num);
		S.vsplitVert		= V_New;
		S.numFixFaces		= 0;
		S.numNewTriangles	= 0;
		
		// modify indices
		//		Msg("Collapsing %d to %d, I_Current=%d",V_Num,V_New,I_Current);
		for (int F_Num=I_Current-1; F_Num>=0; F_Num--)
		{
			if (pIndices[F_Num] == V_Num)
			{
				pIndices[F_Num] = V_New;
				pmFACE_FIX.push_back(F_Num);
				S.numFixFaces++;
				if (S.numFixFaces>=255) {
					DumpIDX(pIndices,idxCount);
					R_ASSERT(S.numFixFaces<255);
				}
			}
		}
		
		// now let's see how many triangles becames degenerated
		int T_Num = (I_Current - 3);
		while (
			(pIndices[T_Num+0]==pIndices[T_Num+1]) ||
			(pIndices[T_Num+1]==pIndices[T_Num+2]) ||
			(pIndices[T_Num+0]==pIndices[T_Num+2]) )
		{
			S.numNewTriangles++;
			R_ASSERT(S.numNewTriangles<255);
			T_Num-=3;
			if (T_Num<0) break;
		}
		I_Current-=3*DWORD(S.numNewTriangles);
		pmSPLIT.push_back(S);
	}
	DumpIDX(pIndices,idxCount);
	
	// reverse vsplit order
	R_ASSERT(pmSPLIT.size()==(Vperm.num()-dwRealMin));
	reverse(pmSPLIT.begin(),pmSPLIT.end());
	
	// reverse affect list
	reverse(pmFACE_FIX.begin(), pmFACE_FIX.end());
	
	// Cleanup
	vertices.clear();
	triangles.clear();
	
	// Record results
	RESULT->permutePTR		= pmPERMUTE.begin	();
	RESULT->permuteSIZE		= pmPERMUTE.size	();
	
	RESULT->splitPTR		= pmSPLIT.begin		();
	RESULT->splitSIZE		= pmSPLIT.size		();
	
	RESULT->facefixPTR		= pmFACE_FIX.begin	();
	RESULT->facefixSIZE		= pmFACE_FIX.size	();
	
	RESULT->minVertices		= dwRealMin;
	return I_Current;
}
