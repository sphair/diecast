#include "precomp.h"
#include "zone.h"
#include "zone_visibility.h"
#include "../player.h"
#include "../server_gameobject.h"
#include "../Components/replicated_container.h"
#include "../../Network/definitions_netevents.h"

using namespace clan;

/////////////////////////////////////////////////////////////////////////////
// Construction:

Zone::Zone(const ServerGameObjectPtr &container_gameobject, LifeCycleMode lifecycle)
: container_gameobject(container_gameobject), lifecycle(lifecycle)
{
	slots.connect(container_gameobject->sig_child_added, this, &Zone::on_add_gameobject);
	slots.connect(container_gameobject->sig_child_removed, this, &Zone::on_remove_gameobject);
}

Zone::~Zone()
{
	log_event("Debug", "Destroying zone %1 %2", get_id(), get_name());
}

/////////////////////////////////////////////////////////////////////////////
// Attributes:

std::string Zone::get_name() const
{
	return container_gameobject->get_name();
}

int Zone::get_id() const
{
	return container_gameobject->get_id();
}

ServerGameObjectPtr Zone::find_gameobject(int id) const
{
	return container_gameobject->find_child(id);
}

ServerGameObjectPtr Zone::get_container_gameobject() const
{
    return container_gameobject;
}

bool Zone::should_be_removed() const
{
	return lifecycle == DESTROY_WHEN_EMPTY && players.size() == 0;
}

/////////////////////////////////////////////////////////////////////////////
// Operations:

void Zone::add_player(const PlayerPtr &player, const ZoneVisibilityPtr &visibility)
{
//    log_event("Zones", string_format("Subscribing player %1 to zone %2 (%3 %4)", player->get_user_id(), parent_gameobject->get_id(), parent_gameobject->get_type(), parent_gameobject->get_name()));
    players[player] = visibility;

    ReplicatedContainerPtr replication_container = player->get_replication_container();
    replication_container->add_gameobject(container_gameobject);
}

void Zone::remove_player(const PlayerPtr &player)
{
	auto it = players.find(player);
	if (it != players.end())
	{
		players.erase(it);
    }
}

void Zone::on_add_gameobject(const ServerGameObjectPtr &gameobject, bool moved)
{
	for(auto it = players.begin(); it != players.end(); ++it)
	{
		PlayerPtr player = it->first;
		ZoneVisibilityPtr visibility = it->second;

		visibility->prepare_visibility_checks();

		if(visibility->is_gameobject_visible(gameobject, container_gameobject, player))
		{
			if(moved)
				player->get_replication_container()->attach_gameobject(gameobject);
			else
				player->get_replication_container()->add_gameobject(gameobject);
		}
	}
}

void Zone::on_remove_gameobject(const ServerGameObjectPtr &gameobject, bool moved)
{
	for(auto it = players.begin(); it != players.end(); ++it)
	{
		PlayerPtr player = it->first;
		if(moved)
			player->get_replication_container()->detach_gameobject(gameobject);
		else
			player->get_replication_container()->remove_gameobject(gameobject);
	}
}

void Zone::trigger_visibility_check(const PlayerPtr &player) 
{
	auto it = players.find(player);
	if(it != players.end())
	{
		ZoneVisibilityPtr visibility = it->second;
		visibility->prepare_visibility_checks();

		auto objects = container_gameobject->get_children();
		for (auto it = objects.begin(); it != objects.end(); ++it) 
		{
			ServerGameObjectPtr gameobject = (*it);
			if(visibility->is_gameobject_visible(gameobject, container_gameobject, player))
			{
				player->get_replication_container()->add_gameobject(gameobject);
			}
			else
			{
				player->get_replication_container()->remove_gameobject(gameobject);
			}
		}
	}
}

void Zone::trigger_visibility_check()
{
	for(auto it = players.begin(); it != players.end(); ++it)
	{
		PlayerPtr player = it->first;
		trigger_visibility_check(player);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Implementation:

