// xrXRC.h: interface for the xrXRC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRXRC_H__9AA25268_621F_4FCA_BD75_AF2E9822B8E3__INCLUDED_)
#define AFX_XRXRC_H__9AA25268_621F_4FCA_BD75_AF2E9822B8E3__INCLUDED_
#pragma once

#include "xrCDB.h"

class ENGINE_API xrXRC  
{
	CDB::COLLIDER*	CL;
public:
	IC void			ray_options		(DWORD f)		
	{ 
		CL->ray_options(f); 
	}
	IC void			ray_query		(const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f)
	{
		Device.Statistic.clRAY.Begin();
        cdb_query_ray(CL,m_def,r_start,r_dir,r_range);
		Device.Statistic.clRAY.End	();
	}
	IC void			ray_query		(const Fmatrix& parent, const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f)
	{
    	// transform
    	Fmatrix M;
        Fvector S,D;
        M.invert(parent);
	    M.transform_tiny(S,r_start);
    	M.transform_dir(D,r_dir);
		ray_query(m_def,S,D,r_range);
	}

	IC void			box_options		(DWORD f)
	{
		CL->box_options(f);
	}
	IC void			box_query		(const CDB::MODEL *m_def, const Fvector& b_center, const Fvector& b_dim)
	{
		Device.Statistic.clBOX.Begin();
        cdb_query_box(CL,m_def,b_center,b_dim);
		Device.Statistic.clBOX.End	();
	}
	IC void			box_query		(const Fmatrix& parent, const CDB::MODEL *m_def, const Fbox& src)
	{
    	Fbox dest;
		dest.xform(src,parent);
        Fvector c,d;
        dest.getcenter(c);
        dest.getradius(d);
    	box_query(m_def,c,d);
	}

	IC void			frustum_options	(DWORD f)
	{
		CL->frustum_options(f);
	}
	IC void			frustum_query	(const CDB::MODEL *m_def, const CFrustum& F)
	{
		Device.Statistic.clFRUSTUM.Begin();
		CL->frustum_query(m_def,F);
		Device.Statistic.clFRUSTUM.End	();
	}
	
	IC CDB::RESULT*	r_begin			()	{	return CL->r_begin();		};
	IC CDB::RESULT*	r_end			()	{	return CL->r_end();			};
	IC void			r_free			()	{	CL->r_free();				}
	IC int			r_count			()	{	return CL->r_count();		};
	IC void			r_clear			()	{	CL->r_clear();				};
	
	xrXRC();
	~xrXRC();
};
ENGINE_API extern xrXRC XRC;

#endif // !defined(AFX_XRXRC_H__9AA25268_621F_4FCA_BD75_AF2E9822B8E3__INCLUDED_)
