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

	struct EditorContext;

	class SelectionContext
	{
	public:
		SelectionContext(const Ref<EditorContext>& context);
		SelectionContext(const SelectionContext& context) = default;
		~SelectionContext() = default;

		bool Empty() const;
		bool Has(const SelectionType type = SelectionType::Any) const;
		bool Has(const UUID uuid) const;
		bool Has(const std::vector<UUID>& uuids) const;

		bool CanSelect(const SelectionType type) const;

		size_t Count(const SelectionType type = SelectionType::Any) const;

		void Clear(const SelectionType type = SelectionType::Any);

		bool Set(const UUID entity);
		bool Add(const UUID uuid);
		bool Add(const std::vector<UUID>& entities);

		bool Remove(const UUID uuid);
		bool Remove(const std::vector<UUID>& uuid);

		bool Toggle(const UUID uuid);

		const UUID Get() const;
		const std::vector<UUID> GetAll() const;

		SelectionType Type() const { return m_Type; };
	private:
		Ref<EditorContext> m_Context;
		std::vector<UUID> m_Selected;
		SelectionType m_Type;
	};

}
