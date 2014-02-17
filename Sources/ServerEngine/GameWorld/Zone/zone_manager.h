#pragma once

#include "zone.h"

class Player; typedef std::shared_ptr<Player> PlayerPtr;
class ServerGameObject; typedef std::shared_ptr<ServerGameObject> ServerGameObjectPtr;
class GameObjectManager; typedef std::shared_ptr<GameObjectManager> GameObjectManagerPtr;

// Contains all active zones
class ZoneManager
{
public:
	ZoneManager(const GameObjectManagerPtr &gameobject_manager);
	~ZoneManager();

// Attributes:
public:
	ZonePtr find_zone(int zone_id) const;
	ZonePtr find_zone_with_gameobject(const ServerGameObjectPtr &gameobject) const;

// Operations:
public:
	ZonePtr get_or_create_zone(const ServerGameObjectPtr &gameobject, Zone::LifeCycleMode lifecycle);

	void remove_player(const PlayerPtr &player);

private:
	ZonePtr create_zone(const ServerGameObjectPtr &parent_gameobject, Zone::LifeCycleMode lifecycle);

	GameObjectManagerPtr gameobject_manager;

	std::vector<ZonePtr> zones;
};

typedef std::shared_ptr<ZoneManager> ZoneManagerPtr;