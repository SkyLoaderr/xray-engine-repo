#pragma once



DEFINE_VECTOR(ref_shader, SHADER_VECTOR, SHADER_VECTOR_IT);
class CWalmarkManager
{
private:
	SHADER_VECTOR					*m_wallmarks																																		;
	float							m_trace_dist																																		;
	float							m_wallmark_size																																		;
public:
				CWalmarkManager		()																																					;
				~CWalmarkManager	()																																					;
		void	Load				(CInifile *ltx,LPCSTR section)																														;
		void	Clear				()																																					;
static  void	AddWallmark			(const Fvector& dir, const Fvector& start_pos, float range, float wallmark_size,SHADER_VECTOR& wallmarks_vector,int t)								;
static	void	PlaceWallmark		(const Fvector& dir, const Fvector& start_pos, float trace_dist, float wallmark_size,SHADER_VECTOR& wallmarks_vector,CObject* ignore_obj)			;
		void	PlaceWallmark		(const Fvector& dir, const Fvector& start_pos, float trace_dist, float wallmark_size,CObject* ignore_obj)											;
static	void	PlaceWallmarks		( const Fvector& start_pos, float trace_dist, float wallmark_size,SHADER_VECTOR& wallmarks_vector,CObject* ignore_obj)								;
		void	PlaceWallmarks		(const Fvector& start_pos,CObject* ignore_obj)																										;
protected:
private:
		
};