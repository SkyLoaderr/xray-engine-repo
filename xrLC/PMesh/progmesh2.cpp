/*
 *  Progressive Mesh type Polygon Reduction Algorithm
 *  by Stan Melax (c) 1998
 *
 *  Version for 3DS MAX plugin.
 *  See the header file progmesh.h for a description of this module
 */

#include "stdafx.h"
#include "progmesh2.h"
#include "vector.h"
#include "..\xrUVpoint.h"


PMarg pmarg;

/*
 *  For the polygon reduction algorithm we use data structures
 *  that contain a little bit more information than the usual
 *  indexed face set type of data structure.
 *  From a vertex we wish to be able to quickly get the
 *  neighboring faces and vertices.
 */

class Triangle;
class Vertex;


class Triangle {
  public:
	Vertex *        vertex	[3];	// the 3 points that make this tri
	Vector          normal;			// unit vector othogonal to this face
	int				place;			// used to cut algorithm order

	Triangle		(Vertex *v0,Vertex *v1,Vertex *v2);
	~Triangle		();

	void            ComputeNormal();
	void            ReplaceVertex(Vertex *vold,Vertex *vnew);
	int             HasVertex	(Vertex *v);
	DWORD           texat		(Vertex *v);
	DWORD           vcat		(Vertex *v);
	void            settexat	(Vertex *v,DWORD t);
	void            setvcat		(Vertex *v,DWORD t);
};

class Vertex {
  public:
	Vector				position;	// location of point in euclidean space
	int					id;			// place of vertex in original list
	List<Vertex *>		neighbor;	// adjacent vertices
	List<Triangle *>	face;		// adjacent triangles
	float				objdist;	// cached cost of collapsing edge
	Vertex *			collapse;	// candidate vertex for collapse

	int					place;		// used to cut algorithm order
	int					heapspot;
	DWORD				selected;	// lock vertex selection

	Vertex				(Vector v,int _id,int _selected);
	~Vertex				();

	void				RemoveIfNonNeighbor(Vertex *n);
	int					IsBorder();
};


List<Vertex *>   vertices;
List<Vertex *>   heap;
List<Triangle *> triangles;

float HeapVal(int i) {
	if(i>=heap.num || heap[i]==NULL) return 9999999999999.9f;
	return heap[i]->objdist;
}
void SortUp(int k){
	int k2;
	while(HeapVal(k) < HeapVal((k2=(k-1)/2))) {
		Vertex *tmp = heap[k];
		heap[k] = heap[k2];
		heap[k]->heapspot=k;
		heap[k2]=tmp;
		heap[k2]->heapspot=k2;
		k=k2;
	}
}
void SortDown(int k){
	int k2;
	while(HeapVal(k) > HeapVal((k2=(k+1)*2)) || HeapVal(k) > HeapVal((k2-1))) {
		k2 = (HeapVal(k2)<HeapVal(k2-1))? k2:k2-1;
		Vertex *tmp = heap[k];
		heap[k] = heap[k2];
		heap[k]->heapspot=k;
		heap[k2]=tmp;
		if(tmp)heap[k2]->heapspot=k2;
		k=k2;
	}
}
void HeapAdd(Vertex *v) {
	int k=heap.num;
	heap.Add(v);
	v->heapspot=k;
	SortUp(k);
}
Vertex *HeapPop() {
	Vertex *rv = heap[0];
	VERIFY(rv);
	rv->heapspot=-1;
	heap[0]=NULL;
	SortDown(0);
	return rv;
}
Triangle::Triangle(Vertex *v0,Vertex *v1,Vertex *v2){
	VERIFY(v0!=v1 && v1!=v2 && v2!=v0);
	vertex[0]=v0;
	vertex[1]=v1;
	vertex[2]=v2;
	if(_tex) {
		tex[0] = _tex[0];
		tex[1] = _tex[1];
		tex[2] = _tex[2];
	}
	else {
		tex[0]=tex[1]=tex[2]=0;
	}
	if(_vc) {
		vc[0] = _vc[0];
		vc[1] = _vc[1];
		vc[2] = _vc[2];
	}
	else {
		vc[0]=vc[1]=vc[2]=0;
	}
	ComputeNormal();
	place = triangles.num;
	triangles.Add(this);
	for(int i=0;i<3;i++) {
		vertex[i]->face.Add(this);
		for(int j=0;j<3;j++) if(i!=j) {
			vertex[i]->neighbor.AddUnique(vertex[j]);
		}
	}
}
Triangle::~Triangle(){
	int i;
	//triangles.Remove(this);
	triangles[place]=triangles[triangles.num-1];
	triangles[place]->place=place;
	// the following 2 lines are useless:
	//triangles[triangles.num-1]=this;
	//place = triangles.num-1
	triangles.num--;
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
int Triangle::HasVertex(Vertex *v) {
	return (v==vertex[0] ||v==vertex[1] || v==vertex[2]);
}
void Triangle::ComputeNormal(){
	Vector v0=vertex[0]->position;
	Vector v1=vertex[1]->position;
	Vector v2=vertex[2]->position;
	normal = (v1-v0)*(v2-v1);
	if(magnitude(normal)==0)return;
	normal = normalize(normal);
}
DWORD Triangle::texat(Vertex *v){
	VERIFY(HasVertex(v));
	for(int i=0;i<3;i++) {
		if(vertex[i]==v) break;
	}
	VERIFY(i<3);
	return tex[i];
}
DWORD Triangle::vcat(Vertex *v){
	VERIFY(HasVertex(v));
	for(int i=0;i<3;i++) {
		if(vertex[i]==v) break;
	}
	VERIFY(i<3);
	return vc[i];
}
void Triangle::settexat(Vertex *v,DWORD t){
	VERIFY(HasVertex(v));
	for(int i=0;i<3;i++) {
		if(vertex[i]==v) break;
	}
	VERIFY(i<3);
	tex[i]=t;
}
void Triangle::setvcat(Vertex *v,DWORD t){
	VERIFY(HasVertex(v));
	for(int i=0;i<3;i++) {
		if(vertex[i]==v) break;
	}
	VERIFY(i<3);
	vc[i]=t;
}
void Triangle::ReplaceVertex(Vertex *vold,Vertex *vnew) {
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

Vertex::Vertex(Vector v,int _id,int _selected) {
	position =v;
	id=_id;
	selected=_selected;
	place=vertices.num;
	vertices.Add(this);
}

Vertex::~Vertex(){
	if(face.num!=0){
		VERIFY(face.num==0);
	}
	while(neighbor.num) {
		neighbor[0]->neighbor.Remove(this);
		neighbor.Remove(neighbor[0]);
	}
	//vertices.Remove(this);
	vertices[place]=vertices[vertices.num-1];
	vertices[place]->place=place;
	// the following 2 lines are useless:
	//vertices[vertices.num-1]=this;
	//place = vertices.num-1
	vertices.num--;
}
int Vertex::IsBorder() {
	int i,j;
	for(i=0;i<neighbor.num;i++) {
		int count=0;
		for(j=0;j<face.num;j++) {
			if(face[j]->HasVertex(neighbor[i])) {
				count ++;
			}
		}
		VERIFY(count>0);
		if(count==1) return 1;
	}
	return 0;
} 
void Vertex::RemoveIfNonNeighbor(Vertex *n) {
	// removes n from neighbor list if n isn't a neighbor.
	if(!neighbor.Contains(n)) return;
	for(int i=0;i<face.num;i++) {
		if(face[i]->HasVertex(n)) return;
	}
	neighbor.Remove(n);
}


float ComputeEdgeCollapseCost(Vertex *u,Vertex *v) {
	// if we collapse edge uv by moving u to v then how 
	// much different will the model change, i.e. how much "error".
	// Texture, vertex normal, and border vertex code was removed
	// to keep this demo as simple as possible.
	// The method of determining cost was designed in order 
	// to exploit small and coplanar regions for
	// effective polygon reduction.
	// Is is possible to add some checks here to see if "folds"
	// would be generated.  i.e. normal of a remaining face gets
	// flipped.  I never seemed to run into this problem and
	// therefore never added code to detect this case.
	int i;
	float edgelength=1.0f;
	if(pmarg.useedgelength){
		edgelength = magnitude(v->position - u->position);
	}
	float curvature=0.001f;

	// find the "sides" triangles that are on the edge uv
	static List<Triangle *> sides;
	sides.num=0;
	for(i=0;i<u->face.num;i++) {
		if(u->face[i]->HasVertex(v)){
			sides.Add(u->face[i]);
		}
	}
	if(pmarg.usecurvature) {
		// use the triangle facing most away from the sides 
		// to determine our curvature term
		for(i=0;i<u->face.num;i++) {
			float mincurv=1; // curve for face i and closer side to it
			for(int j=0;j<sides.num;j++) {
				// use dot product of face normals. '^' defined in vector
				float dotprod = u->face[i]->normal ^ sides[j]->normal;
				mincurv =  _MIN(mincurv,(1.002f-dotprod)/2.0f);
			}
			curvature = _MAX(curvature,mincurv);
		}
	}

	// check for border to interior collapses
	if(u->IsBorder() && sides.num>1) {
		curvature=1;
	} 

	// LockDown border edges (i.e. try not to touch them at all)
	// this is done by locking any border vertex.
	// i.e. even if uv isn't a border edge we dont want to callapse u to any vertex
	// if u is on a border
    if(pmarg.protecttexture){
		// check for texture seam ripping
		int nomatch=0;
		for(i=0;i<u->face.num;i++) {
			for(int j=0;j<sides.num;j++) {
				// perhaps we should actually compare the positions in uv space
				if(u->face[i]->texat(u) == sides[j]->texat(u)) break;
			}
			if(j==sides.num) 
			{
				// we didn't find a triangle with edge uv that shares texture coordinates
				// with face i at vertex u
				nomatch++;
			}
		}
		if(nomatch) {
			curvature=1;
	    }
	}
    if(pmarg.protectvc){
		// adding support for 2nd pass or vert color here:
		// check for vert color (or uvw2) seam ripping
		int nomatch=0;
		for(i=0;i<u->face.num;i++) {
			for(int j=0;j<sides.num;j++) {
				// perhaps we should actually compare the positions in uv space
				if(u->face[i]->vcat(u) == sides[j]->vcat(u)) break;
			}
			if(j==sides.num) 
			{
				// we didn't find a triangle with edge uv that shares texture coordinates
				// with face i at vertex u
				nomatch++;
			}
		}
		if(nomatch) {
			curvature=1;
	    }
	}
	if(pmarg.lockborder && u->IsBorder()) {
		curvature = 9999.9f;
	}
	if(pmarg.lockselected && u->selected) {
		curvature = 9999.9f;
	}


	return edgelength * curvature;
}

void ComputeEdgeCostAtVertex(Vertex *v) {
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
	for(int i=0;i<v->neighbor.num;i++) {
		float dist;
		dist = ComputeEdgeCollapseCost(v,v->neighbor[i]);
		if((!v->collapse) || dist<v->objdist) {
			v->collapse=v->neighbor[i];  // candidate for edge collapse
			v->objdist=dist;             // cost of the collapse
		}
	}
}
void ComputeAllEdgeCollapseCosts() {
	// For all the edges, compute the difference it would make
	// to the model if it was collapsed.  The least of these
	// per vertex is cached in each vertex object.
	for(int i=0;i<vertices.num;i++) {
		ComputeEdgeCostAtVertex(vertices[i]);
		HeapAdd(vertices[i]);
	}
}

void Collapse(Vertex *u,Vertex *v,int recompute=1){
	// Collapse the edge uv by moving vertex u onto v
	// Actually remove tris on uv, then update tris that
	// have u to have v, and then remove u.
	if(!v) {
		// u is a vertex all by itself so just delete it
		delete u;
		return;
	}
	int i;
	static List<Vertex *>tmp;
	tmp.num=0;
	// make tmp a list of all the neighbors of u
	for(i=0;i<u->neighbor.num;i++) {
		tmp.Add(u->neighbor[i]);
	}
	static List<Triangle *> sides;
	sides.num=0;
	for(i=0;i<u->face.num;i++) {
		if(u->face[i]->HasVertex(v)){
			sides.Add(u->face[i]);
		}
	}
	// update texture mapping
	for(i=0;i<u->face.num;i++) {
		int j;
		if(u->face[i]->HasVertex(v)) continue;
		for(j=0;j<sides.num;j++) {
			if(u->face[i]->texat(u)==sides[j]->texat(u)) {
				u->face[i]->settexat(u,sides[j]->texat(v));
				break; // only change tex coords once eh!
			}
		}
		if(j==sides.num) {
			; // didn't upgrade uv here
		}
		// added supporrt for color or 2nd uvw here:
		for(j=0;j<sides.num;j++) {
			if(u->face[i]->vcat(u)==sides[j]->vcat(u)) {
				u->face[i]->setvcat(u,sides[j]->vcat(v));
				break; // only change tex coords once eh!
			}
		}
		if(j==sides.num) {
			; // didn't upgrade uvw2 here
		}
	}

	// delete triangles on edge uv:
	for(i=u->face.num-1;i>=0;i--) {
		if(u->face[i]->HasVertex(v)) {
			delete(u->face[i]);
		}
	}
	// update remaining triangles to have v instead of u
	for(i=u->face.num-1;i>=0;i--) {
		u->face[i]->ReplaceVertex(u,v);
	}
	delete u; 
	// recompute the edge collapse costs for neighboring vertices
	if(recompute) for(i=0;i<tmp.num;i++) {
		ComputeEdgeCostAtVertex(tmp[i]);
		SortUp(tmp[i]->heapspot);
		SortDown(tmp[i]->heapspot);
	}
}

Vertex *MinimumCostEdge(){
	// Find the edge that when collapsed will affect model the least.
	// This funtion actually returns a Vertex, the second vertex
	// of the edge (collapse candidate) is stored in the vertex data.
	Vertex *hp=HeapPop();
	return hp;
}

void shareuv(Mesh *mesh,Triangle *t) {
	// it so happens that neighboring faces that share vertices
	// sometimes share uv coordinates at those verts but have
	// their own entries in the tex vert list
	if(!mesh->getNumTVerts()) return;
	for(int i=0;i<3;i++) {
		Vertex *currentvert = t->vertex[i];
		for(int j=0;j<currentvert->face.num;j++) {
			Triangle *n=currentvert->face[j];
			if(t == n ) continue;
			DWORD tx1 = t->texat(currentvert);
			DWORD tx2 = n->texat(currentvert);
			if(tx1==tx2) continue;
			UVVert uv1=mesh->getTVert(tx1);
			UVVert uv2=mesh->getTVert(tx2);
			if(uv1.x==uv2.x && uv1.y==uv2.y && uv1.z==uv2.z) {
				t->settexat(currentvert,tx2);
			}
		}
	}
}
static void CollapseTVerts(Mesh *mesh){
	int i,n;
	List<int> flag;
	List<UVVert> tvert;
	flag.SetSize(mesh->getNumTVerts());
	for(i=0;i<flag.num;i++) {
		flag[i]=0;
	}
	for(i=0;i<triangles.num;i++) {
		flag[triangles[i]->tex[0]] =1;
		flag[triangles[i]->tex[1]] =1;
		flag[triangles[i]->tex[2]] =1;
	}
	// make mapping
	n=0;
	for(i=0;i<flag.num;i++) {
		if(flag[i]) {
			tvert.Add(mesh->getTVert(i));
			flag[i]=n;
			n++;
			VERIFY(tvert.num==n);
		}
	}
	mesh->setNumTVerts(n);
	for(i=0;i<n;i++) {
		mesh->setTVert(i,tvert[i]);
	}
	for(i=0;i<triangles.num;i++) {
		triangles[i]->tex[0] = flag[triangles[i]->tex[0]];
		triangles[i]->tex[1] = flag[triangles[i]->tex[1]];
		triangles[i]->tex[2] = flag[triangles[i]->tex[2]];
	}
}

// I just used this for debugging:
void hackcheck() {
	for(int i=0;i<vertices.num;i++) {
		List<int> s;
		Vertex *currentvert = vertices[i];
		for(int j=0;j<currentvert->face.num;j++) {
			Triangle *t=currentvert->face[j];
			s.AddUnique(t->texat(currentvert));
		}
		(void) s.num;
	}
}

void ComputeProgressiveMesh(Mesh *mesh, List<int> &order,List<int> &PM, PMarg _pmarg)
{
	pmarg=_pmarg;
	int i;
	vertices.num=0;
	vertices.allocate(mesh->numVerts);
	for(i=0;i<mesh->numVerts;i++) {
		Vector p(mesh->verts[i].x,mesh->verts[i].y,mesh->verts[i].z);
		Vertex *v = new Vertex(p,i,mesh->vertSel[i]);
	}
	triangles.num=0;
	triangles.allocate(mesh->numFaces);
	for(i=0;i<mesh->numFaces;i++) {
		Triangle *t=new Triangle(
					      vertices[mesh->faces[i].v[0]],
					      vertices[mesh->faces[i].v[1]],
					      vertices[mesh->faces[i].v[2]],
						  (mesh->tvFace)?mesh->tvFace[i].t:NULL,
						  (mesh->vcFace)?mesh->vcFace[i].t:NULL
						  );
		shareuv(mesh,t);
	}
	heap.allocate(vertices.num);
	ComputeAllEdgeCollapseCosts(); // cache all edge collapse costs
	PM.SetSize		(vertices.num);
	order.SetSize	(vertices.num);
	while(vertices.num) {
		// get the next vertex to collapse
		Vertex *mn = MinimumCostEdge();
		// Collapse this edge
		order[vertices.num-1]=mn->id;
		PM[mn->id]=(mn->collapse)?mn->collapse->id:-1;
		Collapse(mn,mn->collapse);
	}
	VERIFY(triangles.num==0);
	VERIFY(vertices.num==0);
	heap.num=0;
}


void DoProgressiveMesh(Mesh *mesh,List<int> &order,List<int> &PM,float ratio,
   					   int gentexture,int maintainsmooth){
	VERIFY(order.num==mesh->numVerts);
	VERIFY(PM.num==mesh->numVerts);
	int i;
	int target = (int) (ratio * mesh->numVerts);
	vertices.num=0;
	triangles.num=0;
	List<Vertex *>vclist;
	vclist.SetSize(mesh->numVerts);
	for(i=0;i<mesh->numVerts;i++) {
		Vector p(mesh->verts[i].x,mesh->verts[i].y,mesh->verts[i].z);
		Vertex *v = new Vertex(p,i,mesh->vertSel[i]);
	}
	for(i=0;i<mesh->numVerts;i++) {
		vertices[i]->collapse = (PM[i]==-1)?NULL:vertices[PM[i]];
		vclist[i] = vertices[order[i]];
	}
	for(i=0;i<mesh->numFaces;i++) {
		Triangle *t=new Triangle(
					      vertices[mesh->faces[i].v[0]],
					      vertices[mesh->faces[i].v[1]],
					      vertices[mesh->faces[i].v[2]],
						  ((mesh->tvFace)?mesh->tvFace[i].t:NULL),
						  ((mesh->vcFace)?mesh->vcFace[i].t:NULL)
						  );
		shareuv(mesh,t);
	}
	while(vclist.num > target) {
		// get the next vertex to collapse
		Vertex *mn = vclist[vclist.num-1]; 
		vclist.num--;
		// Collapse this edge
		Collapse(mn,mn->collapse,0);
	}
	//mesh->setNumFaces(0);

	mesh->setNumVerts(vertices.num);
	for(i=0;i<vertices.num;i++) {
		vertices[i]->id=i;  // reassign id's
		Vector p=vertices[i]->position;
		mesh->setVert(i,Point3(p.x,p.y,p.z));
		if(vertices[i]->selected) {
			mesh->vertSel.Set(i);
		}
		else {
			mesh->vertSel.Clear(i);
		}
	}
	mesh->setNumFaces(triangles.num);
	if(gentexture && mesh->tvFace) {
		CollapseTVerts(mesh);
	}
	// according to the documentation these should be automatically called
	// by setNumFaces() - oh well.
	if(mesh->tvFace) mesh->setNumTVFaces(triangles.num);
	if(mesh->vcFace) mesh->setNumVCFaces(triangles.num);
	for(i=0;i<triangles.num;i++) {
		Face *f= &(mesh->faces[i]);
		f->v[0]		= triangles[i]->vertex[0]->id;
		f->v[1]		= triangles[i]->vertex[1]->id;
		f->v[2]		= triangles[i]->vertex[2]->id;
		f->smGroup	= 0;
		f->flags	= EDGE_ALL;

		if(gentexture && mesh->tvFace) {
			TVFace *tvf = &(mesh->tvFace[i]);
			tvf->setTVerts(triangles[i]->tex[0],triangles[i]->tex[1],triangles[i]->tex[2]);
			//tvf->t[1] = triangles[i]->tex[1];
		}
		if(gentexture && mesh->vcFace) {
			TVFace *tvf = &(mesh->vcFace[i]);
			tvf->setTVerts(triangles[i]->vc[0],triangles[i]->vc[1],triangles[i]->vc[2]);
			//tvf->t[1] = triangles[i]->tex[1];
		}
	}
	mesh->InvalidateGeomCache();  // apparently this isn't necessary 
	mesh->InvalidateTopologyCache();  // apparently this isn't necessary 
	while(triangles.num) {
		delete triangles[triangles.num-1];
	}
	while(vertices.num) {
		delete vertices[vertices.num-1];
	}
	VERIFY(vertices.num==0);
	VERIFY(triangles.num==0);
}

