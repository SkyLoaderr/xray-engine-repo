#pragma once

class CInventoryOwner;

class CTrade {
	bool	TradeState;					// режим торговли. true - включен

	typedef enum tagTraderType {
		TT_NONE,
		TT_TRADER,
		TT_STALKER,
		TT_ACTOR,
	} EOwnerType;

	struct SInventoryOwner {
		EOwnerType	type;
		CEntity		*base;

		void Set (EOwnerType t, CEntity	*b) { type = t; base = b;}
	};

	SInventoryOwner	pThis;
	SInventoryOwner pPartner;

	CInventoryOwner	*pInvOwner;			// указатель

public:
	
	CTrade(CInventoryOwner	*p_io);
	~CTrade();

	void		UpdateInventoryOwnerInfo();							// заполнение pThis, вызывается потомком данного класса


	bool		CanTrade();		// Устанавливает pPartner
	bool		IsInTradeState() {return TradeState;}

	void		StartTrade();

private:
	bool		SetPartner(CEntity *p);

};