#pragma once

#include "../users_repository.h"

class SqliteUsersRepository : public UsersRepository
{
public:
	SqliteUsersRepository(const std::string &db_path);

	clan::DBTransaction begin_transaction();

	void login(const std::string &user_name, const std::string &password, const std::function<void(int)> &func) override;

	void get_info(int user_id, const std::function<void(UsersRepository::UserInfo)> &func) override;
	void get_info(const std::string &user_name, const std::function<void(UsersRepository::UserInfo)> &func) override;

private:
	int login(const std::string &user_name, const std::string &password);

	UsersRepository::UserInfo get_info(int user_id);
	UsersRepository::UserInfo get_info(const std::string &user_name);

	clan::SqliteConnection db;
	clan::WorkQueue queue;
};
typedef std::shared_ptr<SqliteUsersRepository> SqliteUsersRepositoryPtr;
