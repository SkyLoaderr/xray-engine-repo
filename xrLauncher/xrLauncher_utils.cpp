#include "stdafx.h"

System::Void on_btn_mouse_down(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!btn->Enabled)
				return;

			btn->ImageIndex = 1;
}

System::Void on_btn_mouse_up(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!btn->Enabled)
				return;

			btn->ImageIndex = 0;
}

System::Void on_btn_enabled_changed(System::Object *  sender, System::EventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(btn->Enabled)
				btn->ImageIndex = 0;
			btn->ImageIndex = 2;
}

System::Void on_btn_key_down(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!(e->KeyCode == System::Windows::Forms::Keys::Space))
				return;

			if(!btn->Enabled)
				return;

			btn->ImageIndex = 1;
}

System::Void on_btn_key_up(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!(e->KeyCode == System::Windows::Forms::Keys::Space))
				return;

			if(!btn->Enabled)
				return;

			btn->ImageIndex = 0;
}



void convert(System::String* src, char* dst)
{
/*	LPCSTR f_name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(info_file_name);
	Marshal::FreeHGlobal(str2);
*/
	for(int i=0; i< src->Length; ++i)
		dst[i] = (char)src->get_Chars(i);
}

void setCoreParam(LPCSTR str)
{
	if(testCoreParam(str))
		return;

	string1024 new_param;
	strconcat(new_param,str,Core.Params);
	strcat(Core.Params,new_param);
}

void resetCoreParam(LPCSTR str)
{
	if(!testCoreParam(str))
		return;

	char* c = strstr(Core.Params,str);
	if(!c)
		return;

	int len = xr_strlen(str);
	int core_len = xr_strlen(Core.Params);
	int start = c-Core.Params;
	string1024 new_param;
	strncpy(new_param,Core.Params,start);

	strcpy(new_param+start,Core.Params+start+len);

	strcpy(Core.Params,new_param);
}

bool testCoreParam(LPCSTR str)
{
	return !!strstr(Core.Params,str);
}