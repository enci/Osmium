#pragma once
#include <Defines.h>

namespace Osm
{
	class IDableBase
	{	
	protected:
		static uint _count;
	};

	template<typename T>
	class IDable : private IDableBase
	{
	protected:
		IDable()
		{			
			static uint dymmy = ++IDableBase::_count;
			_id = dymmy;
		}

	public:

		// ReSharper disable once CppMemberFunctionMayBeStatic
		uint GetTypeID() const { return _id; }

		static uint GetID() { return _id; }

	private:

		static uint _id;
	};

	template<typename T>
	uint IDable<T>::_id;
}
