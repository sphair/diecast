#pragma once

class GameObjectsRepository
{
public:
	enum PropertyType
	{
		TYPE_UNSIGNED_INT = 1,
		TYPE_INT = 2,
		TYPE_FLOAT = 3,
		TYPE_BOOL = 4,
		TYPE_DOUBLE = 5,
		TYPE_CHAR = 6,
		TYPE_STRING = 100,
		TYPE_CL_VEC2I = 101,			// x y
		TYPE_CL_VEC3F = 102,			// x y z
		TYPE_CL_COLORF = 103			// r g b a
	};

	struct GameObjectPropertyInfo
	{
		PropertyType type_id;
		std::string name;
		std::string value;
	};

	struct GameObjectComponentInfo
	{
		std::string type;
		std::string name;
	};

	struct GameObjectInfo
	{
		int gameobject_id;
        int parent_gameobject_id;
		std::string type;

		std::vector<GameObjectComponentInfo> components;
		std::vector<GameObjectPropertyInfo> properties;

		std::list<GameObjectInfo> children;
	};

	struct GameObjectDirtyPropertyInfo
	{
		int gameobject_id;
		std::string component_type;
		std::string component_name;
		std::string property_name;
		std::string property_value;
	};

public:
	virtual void get_info(int gameobject_id, const std::function<void(GameObjectsRepository::GameObjectInfo)> &func) = 0;

	virtual void create(const std::string &type, const std::function<void(int)> &func) = 0;

	virtual void save_dirty_properties(std::vector<GameObjectDirtyPropertyInfo> &properties, const std::function<void()> &func) = 0;

	virtual void add_component(int gameobject_id, const std::string &component_type, const std::string &component_name, const std::function<void()> &func) = 0;
	virtual void remove_component(int gameobject_id, const std::string &component_type, const std::string &component_name, const std::function<void()> &func) = 0;

	virtual void add_property(int gameobject_id, PropertyType property_type, const std::string &property_name, const std::string &property_value, const std::function<void()> &func) = 0;
	virtual void remove_property(int gameobject_id, const std::string &property_name, const std::function<void()> &func) = 0;

	virtual void set_parent_id(int gameobject_id, int parent_gameobject_id, const std::function<void()> &func) = 0;
};
typedef std::shared_ptr<GameObjectsRepository> GameObjectsRepositoryPtr;
