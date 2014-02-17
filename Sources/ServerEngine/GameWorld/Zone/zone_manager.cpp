#include "precomp.h"
#include "zone_manager.h"
#include "zone.h"
#include "../gameobject_manager.h"
#include "../server_gameobject.h"

using namespace Totem;
using namespace clan;

/////////////////////////////////////////////////////////////////////////////
// Construction:

ZoneManager::ZoneManager(const GameObjectManagerPtr &gameobject_manager)
: gameobject_manager(gameobject_manager)
{
}

ZoneManager::~ZoneManager()
{
	zones.clear();
}

/////////////////////////////////////////////////////////////////////////////
// Attributes:

ZonePtr ZoneManager::find_zone(int zone_id) const
{
	for (size_t i = 0; i < zones.size(); ++i)
	{
		if(zones[i]->get_id() == zone_id)
			return zones[i];
	}
	return nullptr;
}

ZonePtr ZoneManager::find_zone_with_gameobject(const ServerGameObjectPtr &gameobject) const
{
	for(size_t i = 0; i < zones.size(); ++i)
	{
		if(zones[i]->find_gameobject(gameobject->get_id()) != nullptr)
			return zones[i];
	}	
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////
// Operations:

ZonePtr ZoneManager::get_or_create_zone(const ServerGameObjectPtr &gameobject, Zone::LifeCycleMode lifecycle)
{
	auto zone = find_zone(gameobject->get_id());
	if(zone == nullptr)
	{
		zone = create_zone(gameobject, lifecycle);
        log_event("Zones", "Initialized zone %1 (%2 %3) with %4 gameobjects", gameobject->get_id(), gameobject->get_type(), gameobject->get_name(), gameobject->get_children().size());
	}
	return zone;
}
	
void ZoneManager::remove_player(const PlayerPtr &player)
{
	for (auto it = zones.begin(); it != zones.end(); ++it)
	{
		(*it)->remove_player(player);	
	}

	for (auto it = zones.begin(); it != zones.end();)
	{
		if((*it)->should_be_removed())
		{
			it = zones.erase(it);
		}
		else
		{
			++it;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Implementation:

ZonePtr ZoneManager::create_zone(const ServerGameObjectPtr &parent_gameobject, Zone::LifeCycleMode lifecycle)
{
	auto zone = ZonePtr(new Zone(parent_gameobject, lifecycle));
	zones.push_back(zone);

//	log_event("Zones", "Initialized zone %1 (%2 %3)", zonecontainer->get_id(), zonecontainer->get_type(), zonecontainer->get_name());

	return zone;
}
