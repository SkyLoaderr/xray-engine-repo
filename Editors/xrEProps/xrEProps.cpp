#include "stdafx.h"
#pragma hdrstop

#include "xrEProps.h"

void XR_EPROPS_API CheckWindowPos(TForm* form)
{
	if (form->Left+form->Width>Screen->Width) 	form->Left	= Screen->Width-form->Width;
	if (form->Top+form->Height>Screen->Height)	form->Top 	= Screen->Height-form->Height;
	if (form->Left<0) 							form->Left	= 0;
	if (form->Top<0) 							form->Top 	= 0;
}
//---------------------------------------------------------------------------

 