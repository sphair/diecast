#pragma once
#include "precomp.h"

#include "Property.h"

 
#include <unordered_map>
#include <vector>
 

namespace Totem
{

class PropertyFactory
{
public:
	template<class PropertyType>
	static std::shared_ptr<Totem::Property<PropertyType>> createProperty(const std::string &name)
	{
		return std::make_shared<Totem::Property<PropertyType>>(name);
	}
};

template<class UserData = void*>
class PropertyContainer
{
public:
	/**
	 * Constructor
	 *
	 */
	PropertyContainer() {}

	/**
	 * Destructor
	 */
	virtual ~PropertyContainer()
	{
		removeAllProperties();
	}

	//--------------------------------------------------------------
	
	/**
	 * Check whether the specified property name exist in this PropertyContainer.
	 * This function works on the interface (IProperty) of the property,
	 * thus no type-casting is involved in it's usage.
	 *
	 * @param name The name of the property.
	 * @return Returns true if the property exist in this PropertyContainer, false if it doesn't.
	 */
	bool hasProperty(const std::string& name);

	/**
	 * Add a property of specified type T and name to this PropertyContainer with a default value.
	 * If readOnly is specified to true, one can only change the property by
	 * directly calling property.set(value, ..., overrideReadonly=true), all other pipes, like
	 * via operators, will throw an exception.
	 *
	 * @param name The name of the property used to store and associate the property in the PropertyContainer.
	 * @param defaultValue The initial value this property should be set to when added to the PropertyContainer.
	 * @param readOnly Flag this property as read-only if it should only be read. Defaults to false.
	 * @return Returns a shared_ptr (pimpl) reference to the property that was added to the PropertyContainer.
	 */
	template<class T>
	Property<T> add(const std::string& name, const T &defaultValue)
	{
		auto it = properties.find(name);
		if(it != properties.end())
		{
			std::shared_ptr<Property<T>> property;
	#ifdef _DEBUG
			property = std::dynamic_pointer_cast< Property<T> >(it->second);
			if(!property)
				throw clan::Exception(("Property " + name + " already exists, but with another type").c_str());
	#else
			property = std::static_pointer_cast< Property<T> >(it->second);
	#endif
			return *property.get();
		}

		auto property = PropertyFactory::createProperty<T>(name);
		property->set(defaultValue, true);
		properties[property->getName()] = property;

		//return *property;
		sign_PropertyAdded.invoke(std::static_pointer_cast<IProperty>(property));
		//return getProperty<T>(name);
		return *property.get();
	}

	/**
	 * Add a property of specified type T and name to this PropertyContainer with a default value and userdata of type UserData.
	 * If readOnly is specified to true, one can only change the property by
	 * directly calling property.set(value, ..., overrideReadonly=true), all other pipes, like
	 * via operators, will throw an exception.
	 *
	 * @param name The name of the property used to store and associate the property in the PropertyContainer.
	 * @param defaultValue The initial value this property should be set to when added to the PropertyContainer.
	 * @param userData The userdata is of type specified for the PropertyContainer's class template, and provides optional userdata description of the property.
	 * @param readOnly Flag this property as read-only if it should only be read. Defaults to false.
	 * @return Returns a shared_ptr (pimpl) reference to the property that was added to the PropertyContainer.
	 */
	template<class T>
	Property<T> add(const std::string& name, const T &defaultValue, const UserData &userData)
	{
		auto it = properties.find(name);
		if(it != properties.end())
		{
			std::shared_ptr<Property<T>> property;
	#ifdef _DEBUG
			property = std::dynamic_pointer_cast< Property<T> >(it->second);
			if(!property)
				throw clan::Exception(("Property " + name + " already exists, but with another type").c_str());
	#else
			property = std::static_pointer_cast< Property<T> >(it->second);
	#endif
			return *property.get();
		}

		auto property = PropertyFactory::createProperty<T>(name);
		property->set(defaultValue, true);
		properties[property->getName()] = property;

		//return *property;
		sign_PropertyWithUserDataAdded.invoke(std::static_pointer_cast<IProperty>(property), userData);
		//return getProperty<T>(name);
		return *property.get();
	}

	/**
	 * Add a property interface to this PropertyContainer.
	 * There can be situations where this is required.
	 *
	 * @param property The interface of the property.
	 */
	void add(std::shared_ptr<IProperty> property);

	/**
	 * Get a shared_ptr (pimpl) reference to a property of specified name from the PropertyContainer.
	 *
	 * @param name The name of the property.
	 * @return Returns a shared_ptr (pimpl) reference to the property.
	 */
	template<class T>
	Property<T> get(const std::string& name)
	{
		auto it = properties.find(name);
		if(it != properties.end())
		{
			std::shared_ptr<Property<T>> property;
	#ifdef _DEBUG
			property = std::dynamic_pointer_cast< Property<T> >(it->second);
			if(!property)
				throw clan::Exception(("Tried to get shared property " + name + ", but the type was wrong").c_str());
	#else
			property = std::static_pointer_cast< Property<T> >(it->second);
	#endif
			return *property.get();
		}
		else
			throw clan::Exception(("Unable to get property " + name).c_str());
	}

	/**
	 * Remove the property of specified name from PropertyContainer with
	 * option to postpone deletion until the next time update
	 * is called on the PropertyContainer.
	 *
	 * @param name The name of the property.
	 * @param postponeDelete Flag whether to postpone the deletion of this property (if true), or delete it immediately (if false). Defaults to false.
	 */
	void removeProperty(const std::string& name, bool postponeDelete = false);

	/**
	 * Remove the property of specified name from PropertyContainer with
	 * option to postpone deletion until the next time update
	 * is called on the PropertyContainer.
	 *
	 * @param name The name of the property.
	 * @param userData The userdata is of type specified for the PropertyContainer's class template, and provides optional userdata description of the property.
	 * @param postponeDelete Flag whether to postpone the deletion of this property (if true), or delete it immediately (if false). Defaults to false.
	 */
	void removeProperty(const std::string& name, const UserData &userData, bool postponeDelete = false);
	
	/**
	 * Remove all properties from PropertyContainer.
	 *
	 */
	void removeAllProperties();

	/**
	 * Remove all properties from PropertyContainer.
	 *
	 * @param userData The userdata is of type specified for the PropertyContainer's class template, and provides optional userdata description of the property.
	 */
	void removeAllProperties(const UserData &userData);
	
	/**
	 * Delete all properties that was deleted last time update was called
	 * and marked postponeDelete.
	 *
	 */
	void clearDeletedProperties();

	/**
	 * Clears the dirty flag on all properties.
	 *
	 */
	void clearDirtyProperties();

	/**
	 * Get a reference to the entire map of properties owned by the PropertyContainer.
	 *
	 * @return Returns a reference to the PropertyContainer's property map.
	 */
	std::unordered_map<std::string, std::shared_ptr<IProperty>> &getProperties() { return properties; }

	//--------------------------------------------------------------
	
	/**
	 * Handles deletion of all properties pending for deletion in this PropertyContainer.
	 *
	 */
	void updateProperties();

	//--------------------------------------------------------------

	/// Provide an assignment operator to leviate level W4 warning
	PropertyContainer &operator= (const PropertyContainer &rhs)
	{
		if(this == &rhs)
			return *this;

		throw clan::Exception("Assignment operation between PropertyContainer are not supported");
	}

	//--------------------------------------------------------------

	/**
	 * Function that gives the outside access to the PropertyContainer's
	 * propertyAdded signal. It's through this function call we can
	 * register slots to the propertyAdded signal.
	 *
	 * @return Returns the propertyAdded signal of this property handler.
	 */
	clan::Signal_v1<std::shared_ptr<IProperty>> &propertyAdded() { return sign_PropertyAdded; }

	/**
	 * Function that gives the outside access to the PropertyContainer's
	 * propertyWithUserDataAdded signal. It's through this function call we can
	 * register slots to the propertyWithUserDataAdded signal.
	 *
	 * @return Returns the propertyWithUserDataAdded signal of this property container.
	 */
	clan::Signal_v2<std::shared_ptr<IProperty>, const UserData&> &propertyWithUserDataAdded() { return sign_PropertyWithUserDataAdded; }

	clan::Signal_v1<std::shared_ptr<IProperty>> &propertyRemoved() { return sign_PropertyRemoved; }
	clan::Signal_v2<std::shared_ptr<IProperty>, const UserData&> &propertyWithUserDataRemoved() { return sign_PropertyWithUserDataRemoved; }

protected:
	/// The map of all properties owned by this PropertyContainer.
	std::unordered_map<std::string, std::shared_ptr<IProperty>> properties;
	/// The list of all properties pending deletion in this PropertyContainer.
	std::vector<std::shared_ptr<IProperty>> deletedProperties;
	/// Signal that's emitted when a property with NO userdata is added to the property container.
	clan::Signal_v1<std::shared_ptr<IProperty>> sign_PropertyAdded;
	/// Signal that's emitted when a property with userdata is added to the property container.
	clan::Signal_v2<std::shared_ptr<IProperty>, const UserData&> sign_PropertyWithUserDataAdded;
	/// Signal that's emitted when a property with NO userdata is removed from the property container.
	clan::Signal_v1<std::shared_ptr<IProperty>> sign_PropertyRemoved;
	/// Signal that's emitted when a property with userdata is removed from the property container.
	clan::Signal_v2<std::shared_ptr<IProperty>, const UserData&> sign_PropertyWithUserDataRemoved;
};

//------------------------------------------------------

template<class UserData>
inline bool PropertyContainer<UserData>::hasProperty(const std::string& name)
{
	if(properties.empty())
		return false;

	auto it = properties.find(name);
	if(it != properties.end())
		return true;
	else
		return false;
}

template<class UserData>
inline void PropertyContainer<UserData>::add(std::shared_ptr<IProperty> property)
{
	auto it = properties.find(property->getName());
	if(it == properties.end())
		properties[property->getName()] = property;
}

template<class UserData>
inline void PropertyContainer<UserData>::removeProperty(const std::string& name, bool postponeDelete)
{
	auto it = properties.find(name);
	if(it != properties.end())
	{
		std::shared_ptr<IProperty> property = (*it).second;
		if(postponeDelete)
			deletedProperties.push_back(property);
		properties.erase(it);

		sign_PropertyRemoved.invoke(property);
	}
}

template<class UserData>
inline void PropertyContainer<UserData>::removeProperty(const std::string& name, const UserData &userData, bool postponeDelete)
{
	auto it = properties.find(name);
	if(it != properties.end())
	{
		std::shared_ptr<IProperty> property = (*it).second;
		if(postponeDelete)
			deletedProperties.push_back(property);
		properties.erase(it);

		sign_PropertyWithUserDataRemoved.invoke(property, userData);
	}
}

template<class UserData>
inline void PropertyContainer<UserData>::removeAllProperties()
{
	for(auto it = properties.begin(); it != properties.end(); ++it)
		sign_PropertyRemoved.invoke(it->second);

	properties.clear();
	clearDeletedProperties();
}

template<class UserData>
inline void PropertyContainer<UserData>::removeAllProperties(const UserData &userData)
{
	for(auto it = properties.begin(); it != properties.end(); ++it)
		sign_PropertyWithUserDataRemoved.invoke(it->second, userData);

	properties.clear();
	clearDeletedProperties();
}

template<class UserData>
inline void PropertyContainer<UserData>::updateProperties()
{
	clearDeletedProperties();
}

template<class UserData>
inline void PropertyContainer<UserData>::clearDeletedProperties()
{
	deletedProperties.clear();
}

template<class UserData>
inline void PropertyContainer<UserData>::clearDirtyProperties()
{
	for(auto it = properties.begin(); it != properties.end(); ++it)
		it->second->clearDirty();
}

} //namespace Totem 
