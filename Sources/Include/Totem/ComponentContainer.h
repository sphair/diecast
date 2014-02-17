/////////////////////////////////////////////////////
//
#pragma once
#include "precomp.h"

#include "IComponent.h"
#include "Component.h"

 
 
#include <vector>
#include <unordered_map>

namespace Totem
{

template<class UserData = void*>
class ComponentContainer
{
public:
	virtual ~ComponentContainer()
	{
		for(unsigned int i = 0; i < components.size(); i++)
			sign_ComponentRemoved.invoke(components[i]);
	}

	std::shared_ptr<IComponent<UserData>> addComponent(std::shared_ptr<IComponent<UserData>> component)
	{
		checkDuplicationAndAdd(component->getRuntimeTypeId(), component->getName());
		components.push_back(component);
		sign_ComponentAdded.invoke(component);
		return component;
	}

	template<class ComponentType>
	std::shared_ptr<ComponentType> addComponent(std::shared_ptr<ComponentType> component)
	{
		checkDuplicationAndAdd(component->getRuntimeTypeId(), component->getName());
		components.push_back(component);
		sign_ComponentAdded.invoke(component);
		return component;
	}

	//////////////////////////////////////////////////////////////////////////////

	template<class ComponentType>
	bool hasComponent(const std::string &name = std::string())
	{
		for(unsigned int i = 0; i < components.size(); i++)
		{
			if(IComponent<UserData>::template isType<ComponentType>(components[i]))
			{
				if(!name.empty())
				{
					if(components[i]->getName() == name)
						return true;
				}
				else
				{
					return true;
				}
			}
		}
		return false;
	}

	template<class ComponentType>
	std::shared_ptr<ComponentType> getComponent(const std::string &name = std::string())
	{
		for(unsigned int i = 0; i < components.size(); i++)
		{
			if(IComponent<UserData>::template isType<ComponentType>(components[i]))
			{
				if(!name.empty())
				{
					if(components[i]->getName() == name)
						return std::static_pointer_cast<ComponentType>(components[i]);
				}
				else
				{
					return std::static_pointer_cast<ComponentType>(components[i]);
				}
			}
		}
		throw clan::Exception(clan::string_format("Couldn't find component with name %1", name));
	}

	std::vector<std::shared_ptr<IComponent<UserData>>> &getComponents() { return components; }

	void updateComponents(const float &deltaTime)
	{
		for(unsigned int i = 0; i < components.size(); i++)
			components[i]->update(deltaTime);
	}

	template<class ComponentType>
	void removeComponent(const std::string &name = std::string(), bool upholdOrderInList = false)
	{
		for(unsigned int i = 0; i < components.size(); i++)
		{
			if(IComponent<UserData>::template isType<ComponentType>(components[i]))
			{
				if(!name.empty())
				{
					if(components[i]->getName() == name)
					{
						sign_ComponentRemoved.invoke(components[i]);
						if(upholdOrderInList)
						{
							components.erase(components.begin()+i);
						}
						else
						{
							components[i] = components.back();
							components.pop_back();
						}
						return;
					}
				}
				else
				{
					sign_ComponentRemoved.invoke(components[i]);
					if(upholdOrderInList)
					{
						components.erase(components.begin()+i);
					}
					else
					{
						components[i] = components.back();
						components.pop_back();
					}
					return;
				}
			}
		}
		throw clan::Exception(clan::string_format("Couldn't find component with name %1", name));
	}

	clan::Signal_v1<std::shared_ptr<IComponent<UserData>>> &componentAdded() { return sign_ComponentAdded; }
	clan::Signal_v1<std::shared_ptr<IComponent<UserData>>> &componentRemoved() { return sign_ComponentRemoved; }

protected:
	void checkDuplicationAndAdd(unsigned int typeId, const std::string &name)
	{
		auto it = namesForComponentTypes.find(typeId);
		if(it != namesForComponentTypes.end())
		{
			const std::vector<std::string> &names = it->second;
			for(unsigned int i = 0; i < names.size(); i++)
				if(names[i] == name)
					throw clan::Exception(clan::string_format("Found duplicate component name: %1", name));
			it->second.push_back(name);
		}
		else
		{
			std::vector<std::string> new_list;
			new_list.push_back(name);
			namesForComponentTypes[typeId] = new_list;
		}
	}

	std::unordered_map<unsigned int, std::vector<std::string>> namesForComponentTypes;
	std::vector<std::shared_ptr<IComponent<UserData>>> components;

	/// Signal that's emitted when a component was added to the component container.
	clan::Signal_v1<std::shared_ptr<IComponent<UserData>>> sign_ComponentAdded;
	/// Signal that's emitted when a component was removed from the component container.
	clan::Signal_v1<std::shared_ptr<IComponent<UserData>>> sign_ComponentRemoved;
};

} //namespace Totem

//
/////////////////////////////////////////////////////////