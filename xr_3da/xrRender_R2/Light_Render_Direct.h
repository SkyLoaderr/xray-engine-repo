#pragma once

class CLight_Render_Direct
{
public:
	void							Create			();
	void							Destroy			();
	void							compute_xfd_1	(u32 m_phase);
	void							compute_xfd_2	(u32 m_phase);
	BOOL							compute_xfp_1	(u32 m_phase, light* L);	// true, if intersect frustum
	void							compute_xfp_2	(u32 m_phase, light* L);
	void							compute_xfs_1	(u32 m_phase, light* L);
	void							compute_xfs_2	(u32 m_phase, light* L);
	void							render			();

	CLight_Render_Direct			();
	~CLight_Render_Direct			();
};
