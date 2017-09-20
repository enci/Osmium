#pragma once
#include <memory>
#include <vector>
#include <Core/IDable.h>
#include <Defines.h>

namespace Osm
{

template <class E>
class Component : public IDable<Component<E>>
{
public:
	explicit Component(E& entity) : _owner(entity), _enabled(true) {}

	Component(Component& other) = delete;

	virtual ~Component()			{}

	E& GetOwner() const				{ return _owner; }

	bool GetEnbled() const			{ return _enabled; }

	void SetEnbled(bool enabled)		{ _enabled = enabled; }

#ifdef INSPECTOR
	virtual void Inspect() {}
#endif

protected:
	E& _owner;

protected:
	bool _enabled;
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

	/// Get all components of a certain kind
	template<class T>
	std::vector<T*> GetComponents();

	/// Remove a component of a certain kind
	template<class T>
	void RemoveComponent();

	/// Set enabled for all components
	void SetComponentsEnabled(bool enbled);

protected:
	std::vector<std::unique_ptr<Component<E>>> _components;
};


template <class E>
template <class T>
T* ComponentContainer<E>::CreateComponent()
{
	E* _this = static_cast<E*>(this);
	T* component = new T(*_this);
	_components.push_back(std::unique_ptr<Component<E>>(component));
	return component;
}

template<class E>
template<class T>
T* ComponentContainer<E>::GetComponent()
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
std::vector<T*> ComponentContainer<E>::GetComponents()
{
	std::vector<T*> components;
	for (auto& c : _components)
	{
		T* found = dynamic_cast<T*>(c.get());
		if (found)
			components.push_back(found);
	}
	return components;
}

template <class E>
template <class T>
void ComponentContainer<E>::RemoveComponent()
{
	auto itr = _components.begin();
	T* found = nullptr;
	for (; itr != _components.end(); ++itr)
	{
		found = dynamic_cast<T*>((*itr).get());
		if (found)
			break;
	}

	if (found)
		_components.erase(itr);
}

template <class E>
void ComponentContainer<E>::SetComponentsEnabled(bool enabled)
{
	for (auto& c : _components)
	{
		c->SetEnabled(enabled);
	}
}

}
