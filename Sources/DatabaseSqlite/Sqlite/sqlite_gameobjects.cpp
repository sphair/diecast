#include "precomp.h"
#include "sqlite_gameobjects.h"

using namespace clan;

// USED TABLES: GameObjects, GameObjectProperties, GameObjectComponents

SqliteGameObjectsRepository::SqliteGameObjectsRepository(const std::string &db_path)
: db(db_path), queue(true)
{
}

DBTransaction SqliteGameObjectsRepository::begin_transaction()
{
	return db.begin_transaction(DBTransaction::deferred);
}

int SqliteGameObjectsRepository::get_queue_count() const
{
	return queue.get_items_queued();
}

void SqliteGameObjectsRepository::get_info(int gameobject_id, const std::function<void(GameObjectsRepository::GameObjectInfo)> &func)
{
//	log_event("Database", "SqliteGameObjectsRepository::get_info %1", gameobject_id);

	queue.queue(
		[this, func, gameobject_id] 
		{
			GameObjectsRepository::GameObjectInfo info = get_info(gameobject_id);
			if(info.gameobject_id != 0)
			{
				info.components = get_components(gameobject_id);
				info.properties = get_properties(gameobject_id);
				info.children = get_children_infos(info.gameobject_id);
			}

			if(func != nullptr)
				queue.work_completed([func, info] { func(info); }); 
		}
	);
}

void SqliteGameObjectsRepository::create(const std::string &type, const std::function<void(int)> &func)
{
//	log_event("Database", "SqliteGameObjectsRepository::create");

	queue.queue(
		[this, func, type] 
		{
			int gameobject_id = create(type);
			if(func != nullptr)
				queue.work_completed([func, gameobject_id] { func(gameobject_id); }); 
		}
	);
}

void SqliteGameObjectsRepository::save_dirty_properties(std::vector<GameObjectsRepository::GameObjectDirtyPropertyInfo> &properties, const std::function<void()> &func)
{
//	log_event("Database", "SqliteGameObjectsRepository::save_dirty_properties");

	queue.queue(
		[this, func, properties] 
		{
			save_dirty_properties(properties);
			if(func != nullptr)
				queue.work_completed([func] { func(); }); 
		}
	);
}

void SqliteGameObjectsRepository::add_component(int gameobject_id, const std::string &component_type, const std::string &component_name, const std::function<void()> &func)
{
//	log_event("Database", "SqliteGameObjectsRepository::add_component");

	queue.queue(
		[=] 
		{
			add_component(gameobject_id, component_type, component_name);
			if(func != nullptr)
				queue.work_completed([func] { func(); }); 
		}
	);
}

void SqliteGameObjectsRepository::remove_component(int gameobject_id, const std::string &component_type, const std::string &component_name, const std::function<void()> &func) 
{
//	log_event("Database", "SqliteGameObjectsRepository::remove_component");

	queue.queue(
		[=] 
		{
			add_component(gameobject_id, component_type, component_name);
			if(func != nullptr)
				queue.work_completed([func] { func(); }); 
		}
	);
}

void SqliteGameObjectsRepository::add_property(int gameobject_id, GameObjectsRepository::PropertyType property_type, const std::string &property_name, const std::string &property_value, const std::function<void()> &func)
{
//	log_event("Database", "SqliteGameObjectsRepository::add_property");

	queue.queue(
		[=] 
		{
			add_property(gameobject_id, property_type, property_name, property_value);
			if(func != nullptr)
				queue.work_completed([func] { func(); }); 
		}
	);
}

void SqliteGameObjectsRepository::remove_property(int gameobject_id, const std::string &property_name, const std::function<void()> &func)
{
//	log_event("Database", "SqliteGameObjectsRepository::remove_property");

	queue.queue(
		[=] 
		{
			remove_property(gameobject_id, property_name);
			if(func != nullptr)
				queue.work_completed([func] { func(); }); 
		}
	);
}

void SqliteGameObjectsRepository::set_parent_id(int gameobject_id, int parent_gameobject_id, const std::function<void()> &func)
{
//	log_event("Database", "SqliteGameObjectsRepository::set_parent_id");

	queue.queue(
		[=] 
		{
			set_parent_id(gameobject_id, parent_gameobject_id);
			if(func != nullptr)
				queue.work_completed([func] { func(); }); 
		}
	);
}

//////////////////////////////////////

std::list<GameObjectsRepository::GameObjectInfo> SqliteGameObjectsRepository::get_children_infos(int parent_gameobject_id)
{
	if(parent_gameobject_id == 0)
		throw Exception("Lacking parent_gameobject_id");

	std::list<GameObjectsRepository::GameObjectInfo> children_infos;

	DBCommand command = db.create_command(
		"SELECT GameObjectId, Type FROM GameObjects WHERE ParentGameObjectId=?1");
	command.set_input_parameter_int(1, parent_gameobject_id);

	DBReader reader = db.execute_reader(command);
	while (reader.retrieve_row())
	{
		GameObjectInfo gameobject;
		gameobject.gameobject_id = reader.get_column_int(0);
		gameobject.type = reader.get_column_string(1);
		gameobject.parent_gameobject_id = parent_gameobject_id;

		children_infos.push_back(gameobject);
	}
	reader.close();

	for(auto it = children_infos.begin(); it != children_infos.end(); ++it)
	{
		(*it).components = get_components((*it).gameobject_id);
		(*it).properties = get_properties((*it).gameobject_id);

		(*it).children = get_children_infos((*it).gameobject_id);
	}

	return children_infos;
}

GameObjectsRepository::GameObjectInfo SqliteGameObjectsRepository::get_info(int gameobject_id)
{
	if(gameobject_id == 0)
		throw Exception("Lacking gameobject_id");

	DBCommand command = db.create_command("SELECT Type, ParentGameObjectId FROM GameObjects WHERE GameObjectId=?1");
	command.set_input_parameter_int(1, gameobject_id);

	GameObjectInfo gameobject;
	gameobject.gameobject_id = 0;

	DBReader reader = db.execute_reader(command);
	if (reader.retrieve_row())
	{
		gameobject.gameobject_id = gameobject_id;
		gameobject.type = reader.get_column_string(0);
		gameobject.parent_gameobject_id = reader.get_column_int(1);
	}

	reader.close();
	return gameobject;
}

int SqliteGameObjectsRepository::get_next_gameobject_id()
{
	auto command = db.create_command("SELECT MAX(GameObjectId) FROM GameObjects");
	return db.execute_scalar_int(command) + 1;
}

int SqliteGameObjectsRepository::create(const std::string &type)
{
	int gameobject_id = get_next_gameobject_id();

	DBCommand command = db.create_command("INSERT INTO GameObjects (GameObjectId, Type) VALUES (?1, ?2)");
	command.set_input_parameter_int(1, gameobject_id);
	command.set_input_parameter_string(2, type);

	db.execute_non_query(command);

	return gameobject_id;
}

void SqliteGameObjectsRepository::save_dirty_properties(std::vector<GameObjectsRepository::GameObjectDirtyPropertyInfo> properties)
{
//	std::cout << string_format("Thread: %1", GetCurrentThreadId()) << std::endl;

	//	DBTransaction transaction = db.begin_transaction();

	// TODO: Handle Component properties when saving SQL

	for(size_t i = 0; i < properties.size(); ++i)
	{
		DBCommand command = db.create_command("UPDATE GameObjectProperties SET Value=?3 WHERE GameObjectId=?1 AND Name=?2");
		command.set_input_parameter_int(1, properties[i].gameobject_id);
		command.set_input_parameter_string(2, properties[i].property_name);
		command.set_input_parameter_string(3, properties[i].property_value);

		db.execute_non_query(command);
	}

//	transaction.commit();
}

std::vector<GameObjectsRepository::GameObjectComponentInfo> SqliteGameObjectsRepository::get_components(int gameobject_id)
{
	if(gameobject_id == 0)
		throw Exception("Lacking gameobject_id");

	DBCommand command = db.create_command(
		"SELECT "
		"   GameObjectComponents.Type, "
		"   GameObjectComponents.Name "
		"FROM "
		"   GameObjectComponents "
		"WHERE "
		"   GameObjectId=?1");
	command.set_input_parameter_int(1, gameobject_id);

	std::vector<GameObjectComponentInfo> results;

	DBReader reader = db.execute_reader(command);
	while (reader.retrieve_row())
	{
		GameObjectComponentInfo component_info;

		component_info.type = reader.get_column_string(0);
		component_info.name = reader.get_column_string(1);

		results.push_back(component_info);
	}
	reader.close();

	return results;
}

std::vector<GameObjectsRepository::GameObjectPropertyInfo> SqliteGameObjectsRepository::get_properties(int gameobject_id)
{
	if(gameobject_id == 0)
		throw Exception("Lacking gameobject_id");

	DBCommand command = db.create_command(
		"SELECT "
		"   GameObjectProperties.Type, "
		"   GameObjectProperties.Name, "
		"   GameObjectProperties.Value "
		"FROM "
		"   GameObjectProperties "
		"WHERE "
		"   GameObjectId=?1");
	command.set_input_parameter_int(1, gameobject_id);

	std::vector<GameObjectPropertyInfo> results;

	DBReader reader = db.execute_reader(command);
	while (reader.retrieve_row())
	{
		GameObjectPropertyInfo property_info;

		property_info.type_id = static_cast<GameObjectsRepository::PropertyType>(reader.get_column_int(0));
		property_info.name = reader.get_column_string(1);
		property_info.value = reader.get_column_string(2);

		results.push_back(property_info);
	}
	reader.close();

	return results;
}

void SqliteGameObjectsRepository::add_component(int gameobject_id, const std::string &component_type, const std::string &component_name)
{
	if(gameobject_id == 0)
		throw Exception("Lacking gameobject_id");
	if(component_type.length() == 0)
		throw Exception("Lacking component_type");

	if(component_name.empty())
	{
		DBCommand command = db.create_command("INSERT INTO GameObjectComponents (GameObjectId, Type) VALUES (?1,?2)");
		command.set_input_parameter_int(1, gameobject_id);
		command.set_input_parameter_string(2, component_type);
		db.execute_non_query(command);
	}
	else
	{
		DBCommand command = db.create_command("INSERT INTO GameObjectComponents (GameObjectId, Type, Name) VALUES (?1,?2,?3)");
		command.set_input_parameter_int(1, gameobject_id);
		command.set_input_parameter_string(2, component_type);
		command.set_input_parameter_string(3, component_name);
		db.execute_non_query(command);
	}
}

void SqliteGameObjectsRepository::remove_component(int gameobject_id, const std::string &component_type, const std::string &component_name)
{
	if(gameobject_id == 0)
		throw Exception("Lacking gameobject_id");
	if(component_type.length() == 0)
		throw Exception("Lacking component_type");

	if(component_name.empty())
	{
		DBCommand command = db.create_command("DELETE FROM GameObjectComponents Where GameObjectId=?1 AND Type=?2");
		command.set_input_parameter_int(1, gameobject_id);
		command.set_input_parameter_string(2, component_type);
		db.execute_non_query(command);
	}
	else
	{
		DBCommand command = db.create_command("DELETE FROM GameObjectComponents Where GameObjectId=?1 AND Type=?2 AND Name=?3");
		command.set_input_parameter_int(1, gameobject_id);
		command.set_input_parameter_string(2, component_type);
		command.set_input_parameter_string(3, component_name);
		db.execute_non_query(command);
	}
}

void SqliteGameObjectsRepository::add_property(int gameobject_id, GameObjectsRepository::PropertyType property_type, const std::string &property_name, const std::string &property_value)
{
	if(gameobject_id == 0)
		throw Exception("Lacking gameobject_id");
	if(property_type == 0)
		throw Exception("Lacking property_type");
	if(property_name.length() == 0)
		throw Exception("Lacking property_name");

	DBCommand command = db.create_command("INSERT INTO GameObjectProperties (GameObjectId, Type, Name, Value) VALUES (?1,?2,?3,?4)");
	command.set_input_parameter_int(1, gameobject_id);
	command.set_input_parameter_int(2, property_type);
	command.set_input_parameter_string(3, property_name);
	command.set_input_parameter_string(4, property_value);

	db.execute_non_query(command);
}

void SqliteGameObjectsRepository::remove_property(int gameobject_id, const std::string &property_name)
{
	if(gameobject_id == 0)
		throw Exception("Lacking gameobject_id");

	DBCommand command = db.create_command("DELETE FROM GameObjectProperties Where GameObjectId=?1 AND Name=?2");
	command.set_input_parameter_int(1, gameobject_id);
	command.set_input_parameter_string(2, property_name);

	db.execute_non_query(command);
}

void SqliteGameObjectsRepository::set_parent_id(int gameobject_id, int parent_gameobject_id)
{
	if(gameobject_id == 0)
		throw Exception("Lacking gameobject_id");

	DBCommand command = db.create_command("UPDATE GameObjects SET ParentGameObjectId=?2 WHERE GameObjectId=?1");
	command.set_input_parameter_int(1, gameobject_id);
	command.set_input_parameter_int(2, parent_gameobject_id);

	db.execute_non_query(command);
}
