#pragma	   once
#include "script_export_space.h"
#include "script_space_forward.h"

class CScriptCallback;
class CGameObject;

class CUsableScriptObject
{
public:
							CUsableScriptObject		();
							~CUsableScriptObject	();
		bool				use						(CGameObject* who_use);
		void				set_callback			(const luabind::object &lua_object, LPCSTR method);
		void				set_callback			(const luabind::functor<void> &lua_function);
		void				clear_callback			();
	
		//������� ������������ ��� ��������� �� ������ (���� NULL, �� ���)
		virtual LPCSTR		tip_text				();
		void				set_tip_text			(LPCSTR new_text);
		virtual void		set_tip_text_default	();

		//����� �� ������������ ������ ����������� (�� ����������) �������
		bool				nonscript_usable		();
		void				set_nonscript_usable	(bool usable);
private:
		CScriptCallback		*callback;
		ref_str				m_sTipText;
		bool				m_bNonscriptUsable;
};