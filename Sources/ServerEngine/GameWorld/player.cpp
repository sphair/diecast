#include "precomp.h"
#include "player.h"
#include "Components/replicated_container.h"

using namespace clan;

Player::Player(int user_id, NetGameConnection *connection, const ComponentSerializerPtr &component_serializer)
: connection(connection), user_id(user_id)
{
	replicated_container = ReplicatedContainerPtr(new ReplicatedContainer(this, component_serializer));
}

Player::~Player()
{
	log_event("Debug", "Destroying player object");
}

void Player::send_event(const NetGameEvent &event)
{
	connection->send_event(event);
}
