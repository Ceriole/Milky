#include "SelectionContext.h"

#include "EditorContext.h"

namespace Milky {

	SelectionContext::SelectionContext(const Ref<EditorContext>& context)
		: m_Context(context), m_Type(SelectionType::None)
	{}

	bool SelectionContext::Empty() const
	{
		return m_Type == SelectionType::None;
	}

	bool SelectionContext::Has(const SelectionType type) const
	{
		if (type == SelectionType::Any)
			return m_Type != SelectionType::None;
		return m_Type == type;
	}

	bool SelectionContext::Has(const UUID uuid) const
	{
		if (!uuid)
			return false;
		for (auto& selectedUUID : m_Selected)
			if (selectedUUID == uuid)
				return true;
		return false;
	}

	bool SelectionContext::Has(const std::vector<UUID>& uuids) const
	{
		if (uuids.empty())
			return false;
		int selected = 0;
		for (UUID uuid : uuids)
			if(Has(uuid))
				selected++;
		return selected == uuids.size();
	}

	bool SelectionContext::CanSelect(const SelectionType type) const
	{
		return type == SelectionType::Any ? m_Type == SelectionType::None : m_Type == SelectionType::None || m_Type == type;
	}

	size_t SelectionContext::Count(const SelectionType type) const
	{
		return type == SelectionType::Any || type != m_Type ? 0 : m_Selected.size();
	}

	void SelectionContext::Clear(const SelectionType type)
	{
		if (type != SelectionType::Any && (type != m_Type || type == SelectionType::None))
			return;
		m_Selected.clear();
		m_Type = SelectionType::None;
	}

	bool SelectionContext::Set(const UUID uuid)
	{
		if (!uuid)
			return false;
		Clear();
		return Add(uuid);
	}

	bool SelectionContext::Add(const UUID uuid)
	{
		SelectionType type = m_Context->GetTypeOfUUID(uuid);
		if (!uuid || !CanSelect(type) || Has(uuid))
			return false;
		m_Selected.push_back(uuid);
		m_Type = type;
		return true;
	}

	bool SelectionContext::Add(const std::vector<UUID>& uuids)
	{
		if(uuids.empty() || !m_Context->IsSameType(uuids))
			return false;
		bool added = false;
		for (auto& uuid : uuids)
			added |= Add(uuid);
		return added;
	}

	bool SelectionContext::Remove(UUID uuid)
	{
		if (!uuid || !Has(uuid))
			return false;
		m_Selected.erase(std::remove(m_Selected.begin(), m_Selected.end(), uuid));
		if (m_Selected.size() == 0)
			m_Type = SelectionType::None;
		return true;
	}

	bool SelectionContext::Remove(const std::vector<UUID>& uuids)
	{
		if(uuids.empty())
			return false;
		for (auto& uuid : uuids)
			Remove(uuid);
		return true;
	}

	bool SelectionContext::Toggle(const UUID uuid)
	{
		if (!uuid || !CanSelect(SelectionType::Entity))
			return false;
		if (Has(uuid))
			return Remove(uuid);
		else
			return Add(uuid);
	}

	const UUID SelectionContext::Get() const
	{
		if (m_Type == SelectionType::None || m_Selected.empty())
			return 0;
		return m_Selected.back();
	}

	const std::vector<UUID> SelectionContext::GetAll() const
	{
		return m_Selected;
	}
}
