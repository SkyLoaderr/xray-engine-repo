#include "stdafx.h"
#include "feel_vision.h"
#include "xr_object.h"

namespace Feel {
	void	Vision::o_new(CObject* E)
	{
		track.push_back(Item());
		Item&	I			= track.back();
		I.E					= E;
		I.Cache.verts[0].set(0,0,0);
		I.Cache.verts[1].set(0,0,0);
		I.Cache.verts[2].set(0,0,0);
		I.fuzzy				= 0.f;
		I.cp_LP.set			(0,0,0);
	}
	void	Vision::o_delete(CObject* E)
	{
		vector<Item>::iterator I=track.begin(),TE=track.end();
		for (; I!=TE; I++)
			if (I->E==E) {
				track.erase(I);
				return;
			}
	}
	void	Vision::o_update(objSET& seen, CObject* parent, Fvector& P, float dt)
	{
		if (seen.size()>1) 
		{
			std::sort		(seen.begin(),seen.end());
			CObject** end = std::unique	(seen.begin(),seen.end());
			if (end!=seen.end()) seen.resize(end-seen.begin());
		}
		objSET diff;

		// B-A = objects, that become visible
		if (!seen.empty()) 
		{
			objSET::iterator E = remove(seen.begin(),seen.end(),parent);
			seen.resize(E-seen.begin());

			{
				objSET::iterator	E = set_difference(
					seen.begin(), seen.end(),
					query.begin(),query.end(),
					diff.begin() );
				diff.resize(E-diff.begin());
				for (DWORD i=0; i<diff.size(); i++)
					o_new(diff[i]);
			}
		}

		// A-B = objects, that are invisible
		if (!query.empty()) 
		{
			objSET::iterator	E = set_difference(
				query.begin(),query.end(),
				seen.begin(), seen.end(),
				diff.begin() );
			diff.resize(E-diff.begin());
			for (DWORD i=0; i<diff.size(); i++)
				o_delete(diff[i]);
		}

		// Copy results and perform traces
		PSGP.memCopy		(&query,&seen,sizeof(query));
		o_trace				(P,dt);
	}
	void Vision::o_trace(Fvector& P, float dt)
	{
		vector<Item>::iterator I=track.begin(),E=track.end();
		for (; I!=E; I++)
		{
			// verify relation
			if (positive(I->fuzzy) && I->E->Position().similar(I->cp_LR_dst,lr_granularity) && P.similar(I->cp_LR_src,lr_granularity))
				continue;

			I->cp_LR_dst		= I->E->Position();
			I->cp_LR_src		= P;

			// Fetch data
			Fvector				OP;
			Fmatrix				mE;
			const Fbox&			B = I->E->CFORM()->GetBBox();
			const Fmatrix&		M = I->E->svXFORM();

			// Build OBB + Ellipse and X-form point
			Fvector				c,r;
			Fmatrix				T,mR,mS;
			B.getcenter			(c);
			B.getradius			(r);
			T.translate			(c);
			mR.mul_43			(M,T);
			mS.scale			(r);
			mE.mul_43			(mR,mS); 
			mE.transform_tiny	(OP,I->cp_LP);

			// 
			Fvector				D;	
			D.sub				(OP,P);
			float				f = D.magnitude();
			if (f>fuzzy_guaranteed) 
			{
				D.div		(f);
				if (pCreator->ObjectSpace.RayTest(P,D,f,false,&I->Cache)) 
				{
					// INVISIBLE, choose next point
					I->fuzzy				-=	fuzzy_update_novis*dt;
					clamp					(I->fuzzy,-.5f,1.f);
					I->cp_LP.random_dir		();
					I->cp_LP.mul			(.7f);
				}
				else 
				{
					// VISIBLE
					I->fuzzy				+=	fuzzy_update_vis*dt;
					clamp					(I->fuzzy,-.5f,1.f);
				}
			}
			else {
				// VISIBLE, 'cause near
				I->fuzzy				+=	fuzzy_update_vis*dt;
				clamp					(I->fuzzy,-.5f,1.f);
			}
		}
	}

	void Vision::o_dump		()
	{
		pApp->pFont->Size		(0.015f);
		pApp->pFont->OutSet		(0.45f,-0.9f);
		vector<Item>::iterator I=track.begin(),E=track.end();
		for (; I!=E; I++)
		{
			if (I->fuzzy>0)		pApp->pFont->Color(D3DCOLOR_XRGB(0,255,0));
			else				pApp->pFont->Color(D3DCOLOR_XRGB(255,0,0));

			char cls			[10];
			CLSID2TEXT			(I->E->SUB_CLS_ID,cls);
			pApp->pFont->OutNext("%2d %1.3f %-9s %s",I-track.begin(),I->fuzzy,cls,I->E->cName());
		}
	}
};
