#pragma once
#include "precomp.h"
#pragma warning(push)
#pragma warning(disable : 4481)

#include "IPropertyList.h"

 
 
#include <vector>

namespace Totem
{

template<class PropertyType>
class PropertyListData
{
public:
	std::vector<PropertyType> value;
	std::string name;
	bool dirty;
	clan::Signal_v3<unsigned int, const PropertyType &, const PropertyType &> valueChanged;
	clan::Signal_v2<unsigned int, const PropertyType &> valueAdded;
	clan::Signal_v2<unsigned int, const PropertyType &> valueErased;
	clan::Signal_v0 valuesCleared;
	clan::Signal_v2<unsigned int, unsigned int> listResized;
};

template<class PropertyType>
class PropertyListIndexValue
{
public:
	PropertyListIndexValue(std::shared_ptr<PropertyListData<PropertyType>> data, unsigned int index)
		: data(data), index(index)
	{
	}

	const PropertyType &get() const { return data->value[index]; }
	PropertyType &get() { return data->value[index]; }

	void set(const PropertyType &rhs, bool invokeValueChanged = true)
	{
		PropertyType oldValue = data->value[index];
		data->value[index] = rhs;
		data->dirty = true; 
		if(invokeValueChanged)
			data->valueChanged.invoke(index, oldValue, rhs);
	}

	void operator= (const PropertyType &rhs) { set(rhs); }

	PropertyListIndexValue<PropertyType> &operator= (const PropertyListIndexValue<PropertyType> &rhs)
	{
		if(this == &rhs)
			return *this;
		throw clan::Exception("Operation not supported");
	}
	operator const PropertyType &() const { return get(); }

	PropertyListIndexValue<PropertyType> &operator+= (const PropertyListIndexValue<PropertyType>& rhs)
	{
		if(this == &rhs)
			return *this;

		set(get() + rhs.get());
		return *this;
	}

	PropertyListIndexValue<PropertyType> &operator+= (const PropertyType& rhs)
	{
		set(get() + rhs);
		return *this;
	}

	PropertyListIndexValue<PropertyType> &operator-= (const PropertyListIndexValue<PropertyType>& rhs)
	{
		if(this == &rhs)
			return *this;

		set(get() - rhs.get());
		return *this;
	}

	PropertyListIndexValue<PropertyType> &operator-= (const PropertyType& rhs)
	{
		set(get() - rhs);
		return *this;
	}

	PropertyListIndexValue<PropertyType> &operator*= (const PropertyListIndexValue<PropertyType>& rhs)
	{
		if(this == &rhs)
			return *this;

		set(get() * rhs.get());
		return *this;
	}

	PropertyListIndexValue<PropertyType> &operator*= (const PropertyType& rhs)
	{
		set(get() * rhs);
		return *this;
	}

	PropertyListIndexValue<PropertyType> &operator/= (const PropertyListIndexValue<PropertyType>& rhs)
	{
		if(this == &rhs)
			return *this;

		set(get() / rhs.get());
		return *this;
	}

	PropertyListIndexValue<PropertyType> &operator/= (const PropertyType& rhs)
	{
		set(get() / rhs);
		return *this;
	}

private:
	std::shared_ptr<PropertyListData<PropertyType>> data;
	unsigned int index;
};

template<class PropertyType>
class PropertyList : public IPropertyList
{
public:
	PropertyList() {}
	PropertyList(const PropertyList &copy)
		: data(copy.data)
	{
	}
	PropertyList(const std::string &name)
		: data(std::make_shared<PropertyListData<PropertyType>>())
	{
		data->name = name;
		data->dirty = false; 
	}

	void push_back(const PropertyType& value, bool invokeValueAdded = true) 
	{ 
		data->value.push_back(value); 

		if(invokeValueAdded)
			data->valueAdded.invoke(data->value.size()-1, value);
	}

	void erase(unsigned int index, bool invokeValueErased = true)
	{
		PropertyType value = data->value[index];
		data->value.erase(data->value.begin()+index);
		if(invokeValueErased)
			data->valueErased.invoke(index, value);
	}

	void clear(bool invokeValuesCleared = true)
	{
		data->value.clear();
		if(invokeValuesCleared)
			data->valuesCleared.invoke();
	}

	unsigned int size() const { return data->value.size(); }
	bool empty() const { return data->value.empty(); }

	void resize(unsigned int size, bool invokeListResized = true)
	{
		unsigned int oldSize = data->value.size();
		data->value.resize(size);
		if(invokeListResized)
			data->listResized(oldSize, size);
	}

	void resize(unsigned int size, const PropertyType &value, bool invokeListResized = true)
	{
		unsigned int oldSize = data->value.size();
		data->value.resize(size, value);
		if(invokeListResized)
			data->listResized(oldSize, size);
	}

	PropertyListIndexValue<PropertyType> at(unsigned int index)
	{
		if(index >= data->value.size())
			throw clan::Exception("Index was out of bounds for shared property list");

		return PropertyListIndexValue<PropertyType>(data, index);
	}

	unsigned int getRuntimeTypeId() const override { return IPropertyList::getRuntimeTypeId<PropertyType>(); }

	const std::vector<PropertyType> &get() const { return data->value; }
	std::vector<PropertyType> &get() { return data->value; }
	const std::string &getName() const override { return data->name; }
	bool isNull() const override { return data == nullptr; }
	bool isDirty() const override { return data->dirty; }
	void clearDirty() override { data->dirty = false; }

	clan::Signal_v3<unsigned int, const PropertyType &, const PropertyType &> &valueChanged() { return data->valueChanged; }
	clan::Signal_v2<unsigned int, const PropertyType &> &valueAdded() {return data->valueAdded; }
	clan::Signal_v2<unsigned int, const PropertyType &> &valueErased() { return data->valueErased; }
	clan::Signal_v0 &valuesCleared() { return data->valuesCleared; }
	clan::Signal_v2<unsigned int, unsigned int> &listResized() { return data->listResized; }

	/// Set's property's data to rhs' shared pointer data.
	PropertyList<PropertyType> operator= (const PropertyList<PropertyType>& rhs)
	{
		data = rhs.data;
		return *this;
	}
	/// Get the value of list at given index.
	PropertyListIndexValue<PropertyType> operator[] (const unsigned int& index)
	{
		return at(index);
	}
	/// Instead of propertyList.get() this operator exist for convenience.
	operator const std::vector<PropertyType> &() const { return data->value; }

private:
	std::shared_ptr<PropertyListData<PropertyType>> data;
};

} //namespace Totem

#pragma warning(pop)
