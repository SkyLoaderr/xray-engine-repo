// Sector.cpp: implementation of the CSector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "build.h"
#include "Sector.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSector::CSector(DWORD ID)
{
	SelfID = ID;
	TreeRoot=0;
}

CSector::~CSector()
{

}

IC BOOL	ValidateMerge	(Fbox& bb_base, Fbox& bb, float& volume, float SLimit)
{
	// Size
	Fbox	merge;	merge.merge		(bb_base,bb);
	Fvector sz;		merge.getsize	(sz);
	if (sz.x>SLimit)		return FALSE;	// Don't exceed limits (4/3 GEOM)
	if (sz.y>SLimit)		return FALSE;
	if (sz.z>SLimit)		return FALSE;

	// Volume
	volume		= merge.getvolume	();

	// OK
	return TRUE;
}

void CSector::BuildHierrarhy()
{
	Fvector		scene_size;
	float		delimiter;
	BOOL		bAnyNode = FALSE;

	pBuild->scene_bb.getsize(scene_size);
	delimiter = _max(scene_size.x,_max(scene_size.y,scene_size.z));
	delimiter *= 2;

	int		iLevel = 1;
	float	SizeLimit = g_params.m_SS_maxsize/4.f;
	if		(SizeLimit<4.f) SizeLimit=4.f;
	for (; SizeLimit<=delimiter; SizeLimit*=2)
	{
		int iSize			= g_tree.size();
//		Status("Level #%d",iLevel);

		for (int I=0; I<iSize; I++)
		{
			if (g_tree[I]->bConnected)		 continue;
			if (g_tree[I]->Sector != SelfID) continue;

			OGF_Node* pNode					= new OGF_Node(iLevel,WORD(SelfID));
			pNode->AddChield				(I);

			// Find best object to connect with
			for (;;) 
			{
				// Find best object to connect with
				int		best_id		= -1;
				float	best_volume	= flt_max;

				for (int J=0; J<iSize; J++)
				{
					OGF_Base* candidate = g_tree[J];
					if ( candidate->bConnected)			continue;
					if ( candidate->Sector != SelfID)	continue;

					float V;
					if (ValidateMerge(pNode->bbox,candidate->bbox,V,SizeLimit))
					{
						if (V<best_volume)	{
							best_volume		= V;
							best_id			= J;
						}
					}
				}

				// Analyze
				if (best_id<0)		break;
				pNode->AddChield	(best_id);
			}

			if (pNode->chields.size()>1) 
			{
				pNode->CalcBounds		();
				g_tree.push_back		(pNode);
				bAnyNode				= TRUE;
			} else {
				g_tree[I]->bConnected = false;
				delete pNode;
			}
		}
		
		if (iSize != (int)g_tree.size()) iLevel++;
	}
	TreeRoot = 0;
	if (bAnyNode) TreeRoot = g_tree.back();
	else {
		for (DWORD I=0; I<g_tree.size(); I++)
		{
			if (g_tree[I]->bConnected)		 continue;
			if (g_tree[I]->Sector != SelfID) continue;
			R_ASSERT(0==TreeRoot);
			TreeRoot = g_tree[I];
		}
	}
	if (0==TreeRoot) {
		Msg("Can't build hierrarhy for sector #%d",SelfID);
	}
}

void CSector::Validate()
{
	std::sort(Portals.begin(),Portals.end());
	R_ASSERT(std::unique(Portals.begin(),Portals.end())==Portals.end());
	R_ASSERT(TreeRoot);
	R_ASSERT(TreeRoot->Sector == SelfID);
}

void CSector::Save(CFS_Base &fs)
{
	// Root
	vector<OGF_Base *>::iterator F = find(g_tree.begin(),g_tree.end(),TreeRoot);
	R_ASSERT(F!=g_tree.end());
	DWORD ID = F-g_tree.begin();
	fs.write_chunk(fsP_Root,&ID,sizeof(DWORD));

	// Portals
	fs.write_chunk(fsP_Portals,Portals.begin(),Portals.size()*sizeof(WORD));

	// Glows
	fs.write_chunk(fsP_Glows,Glows.begin(),Glows.size()*sizeof(WORD));

	// Lights
	fs.write_chunk(fsP_Lights,Lights.begin(),Lights.size()*sizeof(WORD));
}
