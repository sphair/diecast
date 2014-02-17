/////////////////////////////////////////////////////
//
#pragma once
#include "precomp.h"
#pragma warning(push)
#pragma warning(disable : 4481)

#include "IComponent.h"

 
 
 

namespace Totem
{

template<class ComponentType, class UserData = void*>
class Component : public IComponent<UserData> 
{
public:
	Component(const std::string &name) 
		: name(name)
	{
	}
	virtual ~Component()
	{
	}

	unsigned int getRuntimeTypeId() const override { return IComponent<UserData>::template getRuntimeTypeId<ComponentType>(); }
	const std::string &getName() const override { return name; }

	Component &operator= (const Component &rhs)
	{
		if(this == &rhs)
			return *this;

		throw clan::Exception("Assignment operation between ComponentTypes are not supported");
	}
	
protected:
	std::string name;
};

} // namespace Totem

#pragma warning(pop)

//
/////////////////////////////////////////////////////////
