///////////////////////////////////////////////////////////////
// map_location.h
// структура для локации на карте
///////////////////////////////////////////////////////////////

struct SMapLocation
{
	SMapLocation() 
	{
		level_num = 0;
		x = y = 0;
		name.SetText("");
		text.SetText("");

		attached_to_object = false;
		object_id = 0xffff;
	}

	//номер уровня
	int level_num;
	//координаты на уровне
	float x;
	float y;
	//имя локации
	CUIString name;
	//текст описания
	CUIString text;

	//присоединина ли локация к объекту
	bool attached_to_object;
	//id объекта на уровне
	u16 object_id;

	//размеры и положение иконки
	int icon_x, icon_y, icon_width, icon_height;
};