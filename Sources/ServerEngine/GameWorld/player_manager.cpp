#include "precomp.h"
#include "player_manager.h"
#include "player.h"
#include "Components/replicated_container.h"
#include "../Network/definitions_netevents.h"

using namespace clan;

/////////////////////////////////////////////////////////////////////////////
// Construction:

PlayerManager::PlayerManager(const ComponentSerializerPtr &component_serializer)
: component_serializer(component_serializer)
{
}

PlayerManager::~PlayerManager()
{
	players.clear();
}

/////////////////////////////////////////////////////////////////////////////
// Attributes:

PlayerPtr PlayerManager::find_player(int user_id) const
{
	for(size_t i = 0; i < players.size(); ++i)
	{
		if(players[i]->get_user_id() == user_id)
			return players[i];
	}
	return 0;
}

PlayerPtr PlayerManager::find_player(NetGameConnection *connection) const
{
	for(size_t i = 0; i < players.size(); ++i)
	{
		if(players[i]->get_connection() == connection)
			return players[i];
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Operations:

PlayerPtr PlayerManager::add_player(int user_id, NetGameConnection *connection)
{
	auto existing_player = find_player(user_id);
	if(existing_player)
		kick_player(existing_player, "You logged in from somewhere else");

	auto player = PlayerPtr(new Player(user_id, connection, component_serializer));
	players.push_back(player);

	sig_player_added.invoke(player);

	return player;
}

void PlayerManager::kick_player(const PlayerPtr &player, const std::string &reason)
{
	player->send_event(NetGameEvent(STC_LOGOUT, reason));

	// TODO, this seems to not clean up properly, so we get a crash
	remove_player(player);
}

void PlayerManager::remove_player(NetGameConnection *connection)
{
	auto player = find_player(connection);

	if(player)
		remove_player(player);
}

void PlayerManager::remove_player(PlayerPtr player)
{
	auto it = std::find(players.begin(), players.end(), player);
	if (it != players.end())
		players.erase(it);

	sig_player_removed.invoke(player);
}

void PlayerManager::tick(float /*time_elapsed*/)
{
	for(auto it = players.begin(); it != players.end(); ++it)
	{
		(*it)->get_replication_container()->sync_dirty_gameobjects();
	}

	for(auto it = players.begin(); it != players.end(); ++it)
	{
		(*it)->get_replication_container()->clear_dirty_gameobjects();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Implementation:
