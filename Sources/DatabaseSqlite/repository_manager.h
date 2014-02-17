#pragma once

#include "gameobjects_repository.h"
#include "users_repository.h"

class RepositoryManager
{
public:
	virtual GameObjectsRepositoryPtr get_gameobjects_repository() = 0;
	virtual UsersRepositoryPtr get_users_repository() = 0;
};
