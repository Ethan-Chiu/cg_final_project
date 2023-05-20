#pragma once

#include "Ethane/Core/UUID.h"
#include "AssetType.h"

namespace Ethane {

	class Asset
	{
	public:
		UUID Handle;
		
		virtual ~Asset() {}

		virtual AssetType GetAssetType() const { return AssetType::None; }

		virtual bool operator==(const Asset& other) const
		{
			return Handle == other.Handle;
		}

		virtual bool operator!=(const Asset& other) const
		{
			return !(*this == other);
		}
	};

}