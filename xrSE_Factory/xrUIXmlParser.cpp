#include "stdafx.h"
#include "xrUIXmlParser.h"
#include "../ui_base.h"

shared_str CUIXml::correct_file_name	(LPCSTR path, LPCSTR fn)
{
	if(0==xr_strcmp(path,"ui") || 0==xr_strcmp(path,"UI"))
	{
		return UI()->get_xml_name(fn);
	}else
		return fn;
}
