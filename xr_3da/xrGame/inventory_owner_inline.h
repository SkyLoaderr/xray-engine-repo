#pragma once

IC	const Fvector	&CInventoryOwner::torch_angle_offset	() const
{
	return				(m_torch_angle_offset);
}

IC	const Fvector	&CInventoryOwner::torch_position_offset	() const
{
	return				(m_torch_position_offset);
}

IC	LPCSTR			CInventoryOwner::torch_bone_name		() const
{
	return				(m_torch_bone_name);
}
