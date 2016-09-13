#pragma once
#include <Defines.h>

namespace igad
{
	///
	/// This base class is shared 
	/// 
	class IDableBase
	{	
	protected:

	public:
		static uint _count;

		/*
		IDableBase()
		{
			static uint dymmu = _count;
		}
		*/
	};

	template<typename T>
	class IDable : private IDableBase
	{
	protected:
		IDable()
		{			
			static uint dymmy = ++IDableBase::_count;
			_id = dymmy;
			_totalNo++;
		}

	public:

		///
		/// Get the type ID
		// ReSharper disable once CppMemberFunctionMayBeStatic
		uint GetTypeID() const { return _id; }

		static uint GetID() { return _id; }

		static int GetCount() { return _totalNo; }

	private:

		static uint _totalNo;

		static uint _id;
	};

	template<typename T>
	uint IDable<T>::_id;

	template<typename T>
	uint IDable<T>::_totalNo = 0;
}
