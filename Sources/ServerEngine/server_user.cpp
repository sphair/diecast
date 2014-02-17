
#include "precomp.h"
#include "server_user.h"

using namespace clan;

ServerUser::ServerUser(NetGameConnection *connection)
: connection(0), logged_in(false)
{
	this->connection = connection;

	if(connection)
		connection->set_data("serveruser", this);
}

NetGameConnection *ServerUser::get_connection() const
{
	return connection;
}

ServerUser *ServerUser::get_user(NetGameConnection *connection)
{
	if(connection)
		return reinterpret_cast<ServerUser *>(connection->get_data("serveruser"));
	else
		return 0;
}

void ServerUser::send_event(const NetGameEvent &event)
{
	if(connection)
		connection->send_event(event);
}

void ServerUser::set_logged_in()
{
	logged_in = true;
}

bool ServerUser::is_logged_in() const
{
	return logged_in;
}
