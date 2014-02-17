#include "precomp.h"
#include "sqlite_database.h"

using namespace clan;

SqliteDatabase::SqliteDatabase(const std::string &db_path)
{
	gameobjects = SqliteGameObjectsRepositoryPtr(new SqliteGameObjectsRepository(db_path));
	users = SqliteUsersRepositoryPtr(new SqliteUsersRepository(db_path));
}

DBTransaction SqliteDatabase::begin_gameobjects_transaction()
{
	return gameobjects->begin_transaction();
}

DBTransaction SqliteDatabase::begin_users_transaction()
{
	return users->begin_transaction();
}

