#pragma once

class Player; typedef std::shared_ptr<Player> PlayerPtr;
class ServerGameObject; typedef std::shared_ptr<ServerGameObject> ServerGameObjectPtr;
class ZoneVisibilityAll;

class ZoneVisibility
{
public:
	virtual void prepare_visibility_checks() { };
	virtual bool is_gameobject_visible(const ServerGameObjectPtr &gameobject, const ServerGameObjectPtr &parent_gameobject, const PlayerPtr &player) = 0;

	static std::shared_ptr<ZoneVisibility> all;
};

class ZoneVisibilityAll : public ZoneVisibility
{
public:
	bool is_gameobject_visible(const ServerGameObjectPtr &gameobject, const ServerGameObjectPtr &parent_gameobject, const PlayerPtr &player) override;
};

typedef std::shared_ptr<ZoneVisibility> ZoneVisibilityPtr;
