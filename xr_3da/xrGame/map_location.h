///////////////////////////////////////////////////////////////
// map_location.h
// ��������� ��� ������� �� �����
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

	//����� ������
	int level_num;
	//���������� �� ������
	float x;
	float y;
	//��� �������
	CUIString name;
	//����� ��������
	CUIString text;

	//������������ �� ������� � �������
	bool attached_to_object;
	//id ������� �� ������
	u16 object_id;

	//������� � ��������� ������
	int icon_x, icon_y, icon_width, icon_height;
};