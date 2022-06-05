#include "SelectionContext.h"

namespace Milky {

	SelectionContext::SelectionContext()
	{
		m_Type = SelectionType::None;
	}

	SelectionContext::SelectionContext(const SelectionContext& context)
	{
		m_SelectedEntities = context.m_SelectedEntities;
		m_Type = context.m_Type;
	}

	bool SelectionContext::Empty() const
	{
		return m_Type != SelectionType::None;
	}

	bool SelectionContext::Has(const SelectionType type) const
	{
		if (type == SelectionType::Any)
			return m_Type != SelectionType::None;
		return m_Type == type;
	}

	bool SelectionContext::Has(const Entity entity) const
	{
		if (m_Type != SelectionType::Entity || !entity)
			return false;
		for (auto& selectedEntity : m_SelectedEntities)
			if (selectedEntity == entity)
				return true;
		return false;
	}

	bool SelectionContext::Has(const std::vector<Entity>& entities) const
	{
		if (m_Type != SelectionType::Entity || entities.empty())
			return false;
		int selected = 0;
		for (auto& entity : entities)
			for (auto& selectedEntity : m_SelectedEntities)
				if (entity == selectedEntity)
					selected++;
		return selected == entities.size();
	}

	bool SelectionContext::CanSelect(const SelectionType type) const
	{
		if (type == SelectionType::Any)
			return m_Type == SelectionType::None;
		return m_Type == SelectionType::None || m_Type == type;
	}

	size_t SelectionContext::Count(const SelectionType type) const
	{
		switch (m_Type)
		{
		case SelectionType::Entity:
			return m_SelectedEntities.size();
		case SelectionType::Asset:
			return 0;
		case SelectionType::AssetFolder:
			return 0;
		}
		return 0;
	}

	void SelectionContext::Clear(const SelectionType type)
	{
		switch (type)
		{
		case SelectionType::Any:
			// all vectors
			m_SelectedEntities.clear();
			break;
		case SelectionType::Entity:
			m_SelectedEntities.clear();
			break;
		case SelectionType::Asset:
			break;
		case SelectionType::AssetFolder:
			break;
		}
		m_Type = SelectionType::None;
	}

	bool SelectionContext::Set(const Entity entity)
	{
		if (!entity)
			return false;
		Clear();
		return Add(entity);
	}

	bool SelectionContext::Add(const Entity entity)
	{
		if (!entity || !CanSelect(SelectionType::Entity) || Has(entity))
			return false;
		m_SelectedEntities.push_back(entity);
		m_Type = SelectionType::Entity;
		return true;
	}

	bool SelectionContext::Add(const std::vector<Entity>& entities)
	{
		if(entities.empty() || !CanSelect(SelectionType::Entity) || Has(entities))
			return false;
		bool added = false;
		for (auto& entity : entities)
			added |= Add(entity);
		return false;
	}

	bool SelectionContext::Remove(Entity entity)
	{
		if (!entity || !Has(entity))
			return false;
		m_SelectedEntities.erase(std::remove(m_SelectedEntities.begin(), m_SelectedEntities.end(), entity));
		if (m_SelectedEntities.size() == 0)
			m_Type = SelectionType::None;
		return true;
	}

	bool SelectionContext::Remove(const std::vector<Entity>& entities)
	{
		if(entities.empty())
			return false;
		for (auto& entity : entities)
			Remove(entity);
		return true;
	}

	bool SelectionContext::Toggle(const Entity entity)
	{
		if (!entity || !CanSelect(SelectionType::Entity))
			return false;
		if (Has(entity))
			return Remove(entity);
		else
			return Add(entity);
	}

	const Entity SelectionContext::GetEntity() const
	{
		if (m_Type != SelectionType::Entity)
			return Entity{};
		if (m_SelectedEntities.size() == 1)
			return m_SelectedEntities.back();
		return Entity{};
	}

	const std::vector<Entity> SelectionContext::GetEntities() const
	{
		if (m_Type != SelectionType::Entity)
			return std::vector<Entity>();
		return std::vector<Entity>(m_SelectedEntities);
	}
}
