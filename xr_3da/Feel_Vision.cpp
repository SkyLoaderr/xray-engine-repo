#include "stdafx.h"
#include "feel_vision.h"
#include "xr_object.h"
#include "xr_collide_form.h"
#include "xr_creator.h"

namespace Feel {
	void	Vision::o_new		(CObject* O)
	{
		feel_visible.push_back	(feel_visible_Item());
		feel_visible_Item&	I	= feel_visible.back();
		I.O						= O;
		I.Cache.verts[0].set	(0,0,0);
		I.Cache.verts[1].set	(0,0,0);
		I.Cache.verts[2].set	(0,0,0);
		I.fuzzy					= 0.f;
		I.cp_LP.set				(0,0,0);
	}
	void	Vision::o_delete	(CObject* O)
	{
		vector<feel_visible_Item>::iterator I=feel_visible.begin(),TE=feel_visible.end();
		for (; I!=TE; I++)
			if (I->O==O) {
				feel_visible.erase(I);
				return;
			}
	}
	void	Vision::feel_visible_update	(objSET& seen, CObject* parent, Fvector& P, float dt)
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
		vector<feel_visible_Item>::iterator I=feel_visible.begin(),E=feel_visible.end();
		for (; I!=E; I++)
		{
			// verify relation
			if (positive(I->fuzzy) && I->O->Position().similar(I->cp_LR_dst,lr_granularity) && P.similar(I->cp_LR_src,lr_granularity))
				continue;

			I->cp_LR_dst		= I->O->Position();
			I->cp_LR_src		= P;

			// Fetch data
			Fvector				OP;
			Fmatrix				mE;
			const Fbox&			B = I->O->CFORM()->GetBBox();
			const Fmatrix&		M = I->O->svXFORM();

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
};
