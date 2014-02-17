#include "precomp.h"
#include "sqlite_users.h"

using namespace clan;

// USED TABLES: Users

SqliteUsersRepository::SqliteUsersRepository(const std::string &db_path)
: db(db_path), queue(true)
{
}

DBTransaction SqliteUsersRepository::begin_transaction()
{
	return db.begin_transaction(DBTransaction::deferred);
}

void SqliteUsersRepository::login(const std::string &user_name, const std::string &password, const std::function<void(int)> &func)
{
//	log_event("Database", "SqliteUsersRepository::login");

	queue.queue(
		[this, user_name, password, func] 
		{ 
			int user_id = login(user_name, password);
			if(func != nullptr)
				queue.work_completed([user_id, func] { func(user_id); }); 
		}
	);
}

void SqliteUsersRepository::get_info(int user_id, const std::function<void(UsersRepository::UserInfo)> &func)
{
//	log_event("Database", "SqliteUsersRepository::get_info");

	queue.queue(
		[this, user_id, func] 
		{ 
			auto info = get_info(user_id);
			if(func != nullptr)
				queue.work_completed([info, func] { func(info); }); 
		}
	);
}

void SqliteUsersRepository::get_info(const std::string &user_name, const std::function<void(UsersRepository::UserInfo)> &func)
{
//	log_event("Database", "SqliteUsersRepository::get_info");

	queue.queue(
		[this, user_name, func] 
		{ 
			auto info = get_info(user_name);
			if(func != nullptr)
				queue.work_completed([info, func] { func(info); }); 
		}
	);
}

//////////////////////////////////////

UsersRepository::UserInfo SqliteUsersRepository::get_info(int user_id)
{
	if(user_id == 0)
		throw Exception("Lacking user_id");

	DBCommand command = db.create_command("SELECT UserName, RealName, CreateDate, LastLoginDate FROM Users WHERE UserId=?1");
	command.set_input_parameter_int(1, user_id);

	DBReader reader = db.execute_reader(command);
	if (reader.retrieve_row())
	{
		UserInfo user_info;

		user_info.user_id = user_id;
		user_info.user_name = reader.get_column_string(0);
		user_info.real_name = reader.get_column_string(1);
		user_info.created_date = reader.get_column_datetime(2);
		user_info.last_login_date = reader.get_column_datetime(3);

		reader.close();

		return user_info;
	}

	throw Exception(string_format("Unable to retrieve user %1", user_id));
}

UsersRepository::UserInfo SqliteUsersRepository::get_info(const std::string &user_name)
{
	if(user_name.empty())
		throw Exception("Lacking user_name");

	DBCommand command = db.create_command("SELECT UserId, RealName, CreateDate, LastLoginDate FROM Users WHERE UserName=?1");
	command.set_input_parameter_string(1, user_name);

	DBReader reader = db.execute_reader(command);
	if (reader.retrieve_row())
	{
		UserInfo user_info;

		user_info.user_name = user_name;
		user_info.user_id = reader.get_column_int(0);
		user_info.real_name = reader.get_column_string(1);
		user_info.created_date = reader.get_column_datetime(2);
		user_info.last_login_date = reader.get_column_datetime(3);

		reader.close();

		return user_info;
	}

	throw Exception("Unable to retrieve user " + user_name);
}

int SqliteUsersRepository::login(const std::string &user_name, const std::string &check_password)
{
	int user_id = 0;

	DBCommand command = db.create_command("SELECT UserId, Password FROM Users WHERE UserName=?1");
	command.set_input_parameter_string(1, user_name);

	DBReader reader = db.execute_reader(command);
	if (reader.retrieve_row())
	{
		std::string password = reader.get_column_string(1);
		if(password == check_password)
			user_id = reader.get_column_int(0);
	}
	reader.close();

	if(user_id)
	{
		DateTime last_login_date = DateTime::get_current_local_time();

		command = db.create_command("UPDATE Users SET LastLoginDate=?2 WHERE UserId=?1");
		command.set_input_parameter_int(1, user_id);
		command.set_input_parameter_datetime(2, last_login_date);
		db.execute_non_query(command);
	}

	return user_id;
}
