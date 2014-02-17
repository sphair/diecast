#pragma once

class Player; typedef std::shared_ptr<Player> PlayerPtr;
class ServerGameObject; typedef std::shared_ptr<ServerGameObject> ServerGameObjectPtr;
class ZoneVisibility; typedef std::shared_ptr<ZoneVisibility> ZoneVisibilityPtr;

// Contains players and gameobjects in a zone
class Zone
{
// Enums:
public:
	enum LifeCycleMode
	{
		DESTROY_WHEN_EMPTY,
		KEEP_WHEN_EMPTY
	};

// Construction:
public:
	Zone(const ServerGameObjectPtr &container_gameobject, LifeCycleMode lifecycle);
	~Zone();

// Attributes:
public:
	std::string get_name() const;
	int get_id() const;

	ServerGameObjectPtr find_gameobject(int id) const;

    ServerGameObjectPtr get_container_gameobject() const;

	bool should_be_removed() const;

// Operations:
public:
	void add_player(const PlayerPtr &player, const ZoneVisibilityPtr &visibility);
	void remove_player(const PlayerPtr &player);

	void trigger_visibility_check(const PlayerPtr &player);
	void trigger_visibility_check();

private:
	void on_add_gameobject(const ServerGameObjectPtr &gameobject, bool moved);
	void on_remove_gameobject(const ServerGameObjectPtr &gameobject, bool moved);
	void on_attach_gameobject(const ServerGameObjectPtr &gameobject);
	void on_detach_gameobject(const ServerGameObjectPtr &gameobject);

	clan::SlotContainer slots;

	ServerGameObjectPtr container_gameobject;
	LifeCycleMode lifecycle;

	std::map<PlayerPtr, ZoneVisibilityPtr> players;
};
typedef std::shared_ptr<Zone> ZonePtr;
