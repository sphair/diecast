#pragma once

#include "Utilities/tick_timer.h"
#include "GameWorld/Components/component_serializer.h"

class ServerUser;
class ZoneManager; typedef std::shared_ptr<ZoneManager> ZoneManagerPtr;
class Player; typedef std::shared_ptr<Player> PlayerPtr;
class PlayerManager; typedef std::shared_ptr<PlayerManager> PlayerManagerPtr;
class RepositoryManager; typedef std::shared_ptr<RepositoryManager> RepositoryManagerPtr;
class GameObjectManager; typedef std::shared_ptr<GameObjectManager> GameObjectManagerPtr;

class Server
{
public:
	Server();
	~Server();

	void run(clan::Event &stop_event);

	void set_motd(const std::string &motd);

	void accept_connections();

protected:
	virtual void on_init() {}
	virtual void on_tick(float /*elapsed_time*/) {}
	virtual bool on_net_event_received(const clan::NetGameEvent &/*e*/, const PlayerPtr &/*player*/) { return false; }
	
	const ZoneManagerPtr &get_zone_manager() const { return zone_manager; }
	const GameObjectManagerPtr &get_gameobject_manager() const { return gameobject_manager; }
	const PlayerManagerPtr &get_player_manager() const { return player_manager; }
	const ComponentSerializerPtr &get_component_serializer() const { return component_serializer; }

private:
	void initialize_network();

	void on_client_connected(clan::NetGameConnection *connection);
	void on_client_disconnected(clan::NetGameConnection *connection, const std::string &reason);

	void on_net_event_received(clan::NetGameConnection *connection, const clan::NetGameEvent &e);
	void on_net_event_login(const clan::NetGameEvent &e, ServerUser *user);
	void on_net_event_ping(const clan::NetGameEvent &e, ServerUser *user);

	clan::SlotContainer slots;

	clan::NetGameServer network;
	clan::NetGameEventDispatcher_v1<ServerUser *> netevents;

	ComponentSerializerPtr component_serializer;
	RepositoryManagerPtr repository_manager;
	PlayerManagerPtr player_manager;
	GameObjectManagerPtr gameobject_manager;
	ZoneManagerPtr zone_manager;

	TickTimer tick_timer;

	std::string motd;
	bool accepting_connections;
};
