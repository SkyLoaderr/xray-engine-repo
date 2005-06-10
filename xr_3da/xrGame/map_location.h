
#pragma once


class CMapSpot;
class CMiniMapSpot;
class CMapSpotPointer;
class CUICustomMap;
class CInventoryOwner;

class CMapLocation
{
public:
enum ELocationFlags
{
	eSerailizable		= (1<<0),
	eHideInOffline		= (1<<1),
	eTTL				= (1<<2),
	ePosToActor			= (1<<3),
};

private:
	flags32					m_flags;
	shared_str				m_hint;
	CMapSpot*				m_level_spot;
	CMapSpotPointer*		m_level_spot_pointer;
	CMiniMapSpot*			m_minimap_spot;
	CMapSpotPointer*		m_minimap_spot_pointer;
	u16						m_objectID;
	u16						m_refCount;
	int						m_ttl;
	u32						m_actual_time;
	Fvector					m_position_global; //last global position, actual time only current frame 
	Fvector2 				m_position_on_map; //last position on parent map, actual time only current frame
private:
							CMapLocation					(const CMapLocation&){}; //disable copy ctor

protected :
	void					LoadSpot						(LPCSTR type, bool bReload); 
	void					UpdateSpot						(CUICustomMap* map, CMapSpot* sp );
	void					UpdateSpotPointer				(CUICustomMap* map, CMapSpotPointer* sp );
	CMapSpotPointer*		GetSpotPointer					(CMapSpot* sp);

public:
							CMapLocation					(LPCSTR type, u16 object_id);
	virtual					~CMapLocation					();
	virtual		LPCSTR		GetHint							()					{return *m_hint;};
	void					SetHint							(LPCSTR hint)		{m_hint = hint;};

	void					UpdateMiniMap					(CUICustomMap* map);
	void					UpdateLevelMap					(CUICustomMap* map);

	Fvector2				Position						();
	Fvector2				Direction						();
	shared_str				LevelName						();
	u16						RefCount						() {return m_refCount;}
	void					SetRefCount						(u16 c)		{m_refCount=c;}
	u16						AddRef							();// {++m_refCount; return m_refCount;}
	u16						Release							() {--m_refCount; return m_refCount;}
	virtual		bool		Update							(); //returns actual
	Fvector					GetLastPosition					() {return m_position_global;};
	bool					Serializable					() const {return !!m_flags.test(eSerailizable);}
};

class CRelationMapLocation :public CMapLocation
{
	typedef CMapLocation inherited;
	shared_str				m_curr_spot_name;
	u16						m_pInvOwnerEntityID;
	u16						m_pInvOwnerActorID;
public:
							CRelationMapLocation			(const shared_str& type, u16 object_id, u16 pInvOwnerActorID, u16 pInvOwnerEntityID);
	virtual					~CRelationMapLocation			();
	virtual bool			Update							(); //returns actual
};