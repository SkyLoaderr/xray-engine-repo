#include "stdafx.h"

static Bunny gBunny;

int main(int argc, char* argv[])
{
	// 1) Build an OPCODE_Model using a creation structure:
	OPCODE_Model Sample;

	// 1-1) Build a quantized no-leaf tree
	OPCODECREATE OPCC;
	OPCC.NbTris		= gBunny.GetNbFaces();
	OPCC.NbVerts	= gBunny.GetNbVerts();
	OPCC.Tris		= gBunny.GetFaces();
	OPCC.Verts		= gBunny.GetVerts();
//	OPCC.Rules		= SPLIT_COMPLETE | SPLIT_BESTAXIS;
	OPCC.Rules		= SPLIT_COMPLETE | SPLIT_SPLATTERPOINTS;
	OPCC.NoLeaf		= true;
	OPCC.Quantized	= false;
	Sample.Build(OPCC);

	// 2) Create a tree collider and setup it:
	AABBTreeCollider TC;
	TC.SetFirstContact		(false);	// report all contacts
	TC.SetFullBoxBoxTest	(false);	// use coarse BV-BV tests
	TC.SetFullPrimBoxTest	(false);	// use coarse primitive-BV tests
	TC.SetTemporalCoherence	(false);	// don't use temporal coherence

	// 3) Setup object callbacks. Geometry & topology are NOT stored in the collision system,
	// in order to save some ram. So, when the system needs them to perform accurate intersection
	// tests, you're requested to provide the triangle-vertices corresponding to a given face index.
	struct Local
	{
		static void ColCallback(udword triangleindex, VertexPointers& triangle, udword userdata)
		{
			// Get correct triangle in the app-controlled database
			udword* Tri = &gBunny.GetFaces()[triangleindex*3];
			// Setup pointers to vertices for the collision system
			triangle.Vertex[0] = &gBunny.GetVerts()[Tri[0]];
			triangle.Vertex[1] = &gBunny.GetVerts()[Tri[1]];
			triangle.Vertex[2] = &gBunny.GetVerts()[Tri[2]];
		}
	};

	// 3-1) Setup callbacks
	TC.SetCallbackObj0(Local::ColCallback);
	TC.SetCallbackObj1(Local::ColCallback);

	// Of course, you should make this callback as fast as possible. And you're also not supposed
	// to modify the geometry *after* the collision trees have been built. The alternative was to
	// store the geometry & topology in the collision system as well (as in RAPID) but we have found
	// this approach to waste a lot of ram in many cases.

	// 4) Perform a collision query
 
	// 4-1) Setup cache
	static BVTCache ColCache;
	ColCache.Model0 = &Sample;
	ColCache.Model1 = &Sample;

	// 4-2) Collision query
	Matrix4x4 World0, World1;
	World0.Identity();
	World1.Identity();
	World1.SetTrans(0.5f, 0.0f, 0.0f);	// A little translation
	bool IsOk = TC.Collide(ColCache, World0, World1);

	// 4-3) Get collision status => if true, objects overlap
	bool Status = TC.GetContactStatus();
	if(Status)	printf("Overlap detected!\n");

	// 4-4) Number of colliding pairs and list of pairs
	udword NbPairs = TC.GetNbPairs();
	Pair* p = TC.GetPairs();	// list of colliding triangles

	// 5) Stats
	printf("%d bytes used in the collision tree.\n", Sample.GetUsedBytes());
	printf("%d BV-BV overlap tests.\n", TC.GetNbBVBVTests());
	printf("%d Triangle-Triangle overlap tests.\n", TC.GetNbPrimPrimTests());
	printf("%d Triangle-BV overlap tests.\n", TC.GetNbBVPrimTests());

	return 0;
}
