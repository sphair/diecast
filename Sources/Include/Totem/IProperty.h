#pragma once
#include "precomp.h"

 
 

namespace Totem
{

class IProperty
{
public:
	virtual const std::string &getName() const = 0;
	virtual bool isNull() const = 0;
	virtual bool isDirty() const = 0;
	virtual void clearDirty() = 0;

	template<typename PropertyType>
	static bool isType(const std::shared_ptr<IProperty> &property)
	{
		return (property->getRuntimeTypeId() == getRuntimeTypeId<PropertyType>());
	}
	template<typename PropertyType>
	static bool isType(const IProperty &property)
	{
		return (property.getRuntimeTypeId() == getRuntimeTypeId<PropertyType>());
	}

	virtual unsigned int getRuntimeTypeId() const = 0;

	template<typename PropertyType>
	static unsigned int getRuntimeTypeId()
	{
		static unsigned int typeId(typeid(PropertyType).hash_code());
		return typeId;
	}
};

} //namespace Totem

