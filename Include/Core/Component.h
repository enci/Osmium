#pragma once
#include <memory>
#include <vector>
#include <Core/IDable.h>

namespace Osm
{

template <class E>
class Component : public IDable<Component<E>>
{
public:
	explicit Component(E& entity) : _entity(entity) {}

	Component(Component& other) = delete;

	virtual ~Component() {}

	E& GetEntity() const { return _entity; }

protected:
	E& _entity;
};

template <class E>
class ComponentContainer
{
public:
	/// Create a component of a certain kind
	template<class T>
	T* CreateComponent();

	/// Get a component of a certain kind
	template<class T>
	T* GetComponent();

	/// Remove a component of a certain kind
	template<class T>
	void RemoveComponent();

private:
	std::vector<std::unique_ptr<Component<E>>> _components;
};

template <class E>
template <class T>
T* ComponentContainer<E>::CreateComponent()
{
	E* _this = static_cast<E*>(this);
	ASSERT(_this);
	T* component = new T(*_this);
	_components.push_back(std::unique_ptr<Component<E>>(component));
	return component;
}

template<class E>
template<class T>
inline T* ComponentContainer<E>::GetComponent()
{
	for (auto& c : _components)
	{
		T* found = dynamic_cast<T*>(c.get());
		if (found)
			return  found;
	}
	return nullptr;
}

template <class E>
template <class T>
inline void ComponentContainer<E>::RemoveComponent()
{
	auto itr = _components.begin();
	T* found = nullptr;
	for (; itr != _components.end(); ++itr)
	{
		found = dynamic_cast<T*>(itr.get());
		if (found)
			break;
	}

	if (found)
		_components.erase(itr);
}

}