class IUIFontControl{
public:
	typedef CGameFont::EAligment ETextAlignment;
	virtual ~IUIFontControl()											 {};

	virtual void			SetTextColor(u32 color)						= 0;
	virtual u32				GetTextColor()						const	= 0;
	virtual void			SetFont(CGameFont* pFont)					= 0;
	virtual CGameFont*		GetFont()							const	= 0;
	virtual void			SetTextAlignment(ETextAlignment alignment)	= 0;
	virtual ETextAlignment	GetTextAlignment()					const	= 0;
};

class IUITextControl : public IUIFontControl{
public:
	virtual ~IUITextControl()											 {};
	virtual void SetText(const char* text)								= 0;
	virtual const char* GetText()								const	= 0;
};

class IUISimpleWindow{
public:
	virtual ~IUISimpleWindow()						{};

	virtual void		Draw()									const	= 0;
	virtual void		Update()										= 0;
	virtual void		SetWndPos(const Ivector2& pos)					= 0;
	virtual Ivector2	GetWndPos()								const	= 0;
	virtual void		SetWndSize(const Ivector2& size)				= 0;
	virtual Ivector2	GetWndSize()							const	= 0;
	virtual void		SetWndRect(const Irect& rect)					= 0;
	virtual Irect		GetWndRect()							const	= 0;
	virtual void		SetHeight(int height)							= 0;
	virtual int			GetHeight()								const	= 0;
	virtual void		SetWidth(int width)								= 0;
	virtual int			GetWidth()								const	= 0;
	virtual void		Show(bool bShow)								= 0;
	virtual bool		IsShown()								const	= 0;
};