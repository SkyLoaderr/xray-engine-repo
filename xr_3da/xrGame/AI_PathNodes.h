#pragma once

class CCustomMonster;

class CTravelNode
{
public:
	Fvector	P;
	BOOL	floating;
};
struct PContour
{
	Fvector v1,v2,v3,v4;
};
struct PSegment
{
	Fvector v1,v2;
};

class CPathNodes : public AI::Path
{
public:
	BOOL				bNeedRebuild;
	DWORD				DestNode;

	DWORD				TravelStart;
	vector<CTravelNode>	TravelPath;
	vector<PSegment>	Segments;

	float				fSpeed;
	bool				m_bCollision;

	CPathNodes()	{
		bNeedRebuild	= FALSE;
		DestNode		= 0;
		m_bCollision	= true;
	}

	void				BuildTravelLine	(const Fvector& current_pos);

	void				Calculate		(CCustomMonster* Me, Fvector& p_dest, Fvector& p_src, float speed, float dt);
	void				Direction		(Fvector& dest);
};
