#pragma once
#include "inventory.h"
#include "PHShellCreator.h"

class CLAItem;

class CTorch :
	public CInventoryItem,
	public CPHShellSimpleCreator
{
typedef	CInventoryItem	inherited;
	float			fBrightness;
	CLAItem*		lanim;
	float			time2hide;
	u16				guid_bone;
public:
	Fvector			m_pos;
	IRender_Light*	light_render;
	IRender_Glow*	glow_render;
	Fvector			m_focus;
public:
					CTorch				(void);
	virtual			~CTorch				(void);

	virtual void	Load				(LPCSTR section);
	virtual BOOL	net_Spawn			(LPVOID DC);
	virtual void	net_Destroy			();

	virtual void	OnH_A_Chield		();
	virtual void	OnH_B_Independent	();

	virtual void	UpdateCL			();
	virtual void	renderable_Render	();

			void	Switch				();
			void	Switch				(bool light_on);
#pragma todo("Dima to Oles : make H_Parent variation to return const CObject in the CObject class")
	IC		const CObject *H_Parent		() const
	{
		return				(Parent);
	}
	
	virtual bool			use_parent_ai_locations	() const
	{
		return				(!H_Parent());
	}
};
