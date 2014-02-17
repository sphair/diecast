#pragma once

#include "../gameobjects_repository.h"

class SqliteGameObjectsRepository : public GameObjectsRepository
{
public:
	SqliteGameObjectsRepository(const std::string &db_path);

	clan::DBTransaction begin_transaction();
	int get_queue_count() const;

	void get_info(int gameobject_id, const std::function<void(GameObjectsRepository::GameObjectInfo)> &func) override;

	void create(const std::string &type, const std::function<void(int)> &func) override;

	void save_dirty_properties(std::vector<GameObjectsRepository::GameObjectDirtyPropertyInfo> &properties, const std::function<void()> &func) override;

	void add_component(int gameobject_id, const std::string &component_type, const std::string &component_name, const std::function<void()> &func) override;
	void remove_component(int gameobject_id, const std::string &component_type, const std::string &component_name, const std::function<void()> &func) override;

	void add_property(int gameobject_id, GameObjectsRepository::PropertyType property_type, const std::string &property_name, const std::string &property_value, const std::function<void()> &func) override;
	void remove_property(int gameobject_id, const std::string &property_name, const std::function<void()> &func) override;

	void set_parent_id(int gameobject_id, int parent_gameobject_id, const std::function<void()> &func) override;

private:
	GameObjectsRepository::GameObjectInfo get_info(int gameobject_id);

	std::vector<GameObjectsRepository::GameObjectComponentInfo> get_components(int gameobject_id);
	std::vector<GameObjectsRepository::GameObjectPropertyInfo> get_properties(int gameobject_id);

	std::list<GameObjectsRepository::GameObjectInfo> get_children_infos(int parent_gameobject_id);

	int create(const std::string &type);

	void save_dirty_properties(std::vector<GameObjectsRepository::GameObjectDirtyPropertyInfo> properties);

	void add_component(int gameobject_id, const std::string &component_type, const std::string &component_name);
	void remove_component(int gameobject_id, const std::string &component_type, const std::string &component_name);

	void add_property(int gameobject_id, GameObjectsRepository::PropertyType property_type, const std::string &property_name, const std::string &property_value);
	void remove_property(int gameobject_id, const std::string &property_name);

	void set_parent_id(int gameobject_id, int parent_gameobject_id);

	int get_next_gameobject_id();

	clan::SqliteConnection db;
	clan::WorkQueue queue;
};
typedef std::shared_ptr<SqliteGameObjectsRepository> SqliteGameObjectsRepositoryPtr;