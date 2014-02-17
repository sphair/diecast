#pragma once

class UsersRepository
{
public:
	struct UserInfo
	{
		int user_id;
		std::string user_name;
		std::string real_name;
		clan::DateTime last_login_date;
		clan::DateTime created_date;
	};

public:
	// Returns id of user is successful, 0 otherwise. This also updates the LastLoginDate.
	virtual void login(const std::string &user_name, const std::string &password, const std::function<void(int)> &func) = 0;

	// Retrieves a user with a specific user id. Throws exception if not found.
	virtual void get_info(int user_id, const std::function<void(UserInfo)> &func) = 0;

	// Retrieves a user with a specific user name. Throws exception if not found.
	virtual void get_info(const std::string &user_name, const std::function<void(UserInfo)> &func) = 0;
};
typedef std::shared_ptr<UsersRepository> UsersRepositoryPtr;