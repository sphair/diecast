#pragma once

#include "../repository_manager.h"
#include "sqlite_gameobjects.h"
#include "sqlite_users.h"

class SqliteDatabase : public RepositoryManager
{
public:
	SqliteDatabase(const std::string &db_path);

	virtual GameObjectsRepositoryPtr get_gameobjects_repository() override { return gameobjects; }
	virtual UsersRepositoryPtr get_users_repository() override { return users; }

	clan::DBTransaction begin_gameobjects_transaction();
	clan::DBTransaction begin_users_transaction();

private:
	SqliteGameObjectsRepositoryPtr gameobjects;
	SqliteUsersRepositoryPtr users;
};
