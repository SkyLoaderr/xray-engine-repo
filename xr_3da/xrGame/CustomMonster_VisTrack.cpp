#include "stdafx.h"

#include "entity.h"
#include "custommonster_vistrack.h"

namespace AI {
	void	VisiTrack::o_new(CObject* E)
	{
		track.push_back(Item());
		Item&	I			= track.back();
		I.E					= E;
		I.Cache.verts[0].set(0,0,0);
		I.Cache.verts[1].set(0,0,0);
		I.Cache.verts[2].set(0,0,0);
		I.fuzzy				= 0.f;
	}
	void	VisiTrack::o_delete(CObject* E)
	{
		vector<Item>::iterator I=track.begin(),TE=track.end();
		for (; I!=TE; I++)
			if (I->E==E) {
				track.erase(I);
				return;
			}
	}
	void	VisiTrack::o_update(objSET& seen, CEntity* parent, Fvector& P, float dt)
	{
		if (seen.size()>1) 
		{
			std::sort		(seen.begin(),seen.end());
			CObject** end = std::unique	(seen.begin(),seen.end());
			if (end!=seen.end()) seen.resize(end-seen.begin());
		}
		objSET diff;
		if (!seen.empty()) 
		{
			objSET::iterator E = remove(seen.begin(),seen.end(),parent);
			seen.resize(E-seen.begin());
			
			// B-A = objects, that become visible
			{
				objSET::iterator	E = set_difference(
					seen.begin(), seen.end(),
					query.begin(),query.end(),
					diff.begin() );
				diff.resize(E-diff.begin());
				for (DWORD i=0; i<diff.size(); i++)
					o_new(diff[i]);
			}
			// A-B = objects, that are invisible
			{
				objSET::iterator	E = set_difference(
					query.begin(),query.end(),
					seen.begin(), seen.end(),
					diff.begin() );
				diff.resize(E-diff.begin());
				for (DWORD i=0; i<diff.size(); i++)
					o_delete(diff[i]);
			}
		}
		query = seen;
		vector<Item>::iterator I=track.begin(),E=track.end();
		for (; I!=E; I++)
		{
			Fvector			OP;
			Fmatrix			mE;
			const Fbox&		B = I->E->CFORM()->GetBBox();
			const Fmatrix&	M = I->E->svTransform;

			// build OBB
			Fvector			c,r;
			B.getcenter		(c);
			B.getradius		(r);
		
			Fmatrix			T,mR;
			T.translate		(c);
			mR.mul			(M,T);

			// build ellipse matrix
			Fmatrix			mS;
			mS.scale		(r);
			mE.mul			(mR,mS); 

			OP.random_dir	();
			OP.mul			(.8f);
			mE.transform_tiny(OP);


			Fvector			D;	
			D.sub			(OP,P);
			float			f	= D.magnitude();
			if				(f>fuzzy_guaranteed) {
				D.div	(f);
				if (pCreator->ObjectSpace.RayTest(P,D,f,false,&I->Cache))	I->fuzzy-=fuzzy_update_novis*dt;
				else														I->fuzzy+=fuzzy_update_vis*dt;
			} else {
				I->fuzzy+=fuzzy_update_vis*dt;
			}
			clamp(I->fuzzy,-1.f,1.f);
		}
	}
	void VisiTrack::o_dump()
	{
		pApp->pFont->Size	(0.015f);
		pApp->pFont->OutSet	(0.45f,-0.9f);
		vector<Item>::iterator I=track.begin(),E=track.end();
		for (; I!=E; I++)
		{
			if (I->fuzzy>0) pApp->pFont->Color(D3DCOLOR_XRGB(0,255,0));
			else			pApp->pFont->Color(D3DCOLOR_XRGB(255,0,0));

			char cls[10];
			CLSID2TEXT(I->E->SUB_CLS_ID,cls);
			pApp->pFont->OutNext("%2d %1.3f %-9s %s",I-track.begin(),I->fuzzy,cls,I->E->cName());
		}
	}
};
