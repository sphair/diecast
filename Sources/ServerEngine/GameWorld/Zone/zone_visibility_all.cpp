#include "precomp.h"
#include "zone_visibility.h"
#include "../server_gameobject.h"

bool ZoneVisibilityAll::is_gameobject_visible(const ServerGameObjectPtr &gameobject, const ServerGameObjectPtr &, const PlayerPtr &)
{
	if(gameobject->hasProperty("Replicate"))
	{
		return gameobject->get<bool>("Replicate");
	}
	return true;
}

ZoneVisibilityPtr ZoneVisibility::all(new ZoneVisibilityAll());
