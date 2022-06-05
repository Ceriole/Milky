#pragma once

#include "Milky/Scene/Entity.h"

namespace Milky {

	enum class SelectionType
	{
		None = -1,

		Entity,
		AssetFolder,
		Asset,

		Any = 255
	};

	class SelectionContext
	{
	public:
		SelectionContext();
		SelectionContext(const SelectionContext& context);
		~SelectionContext() = default;

		bool Empty() const;
		bool Has(const SelectionType type = SelectionType::Any) const;
		bool Has(const Entity entity) const;
		bool Has(const std::vector<Entity>& entities) const;
		bool CanSelect(const SelectionType type) const;

		size_t Count(const SelectionType type = SelectionType::Any) const;

		void Clear(const SelectionType type = SelectionType::Any);

		bool Set(const Entity entity);
		bool Add(const Entity entity);
		bool Add(const std::vector<Entity>& entities);

		bool Remove(const Entity entity);
		bool Remove(const std::vector<Entity>& entities);

		bool Toggle(const Entity entity);

		const Entity GetEntity() const;
		const std::vector<Entity> GetEntities() const;

		SelectionType Type() const { return m_Type; };
	private:
		std::vector<Entity> m_SelectedEntities;
		SelectionType m_Type;
	};

}
