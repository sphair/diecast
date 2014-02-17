#include "precomp.h"
#include "server.h"
#include "server_user.h"
#include "GameWorld/player.h"
#include "GameWorld/player_manager.h"
#include "GameWorld/gameobject_manager.h"
#include "GameWorld/Zone/zone_manager.h"
#include "Network/definitions_netevents.h"
#include <DatabaseSqlite/Sqlite/sqlite_database.h>

using namespace clan;

Server::Server()
: accepting_connections(false),
  component_serializer(new ComponentSerializer()),
  repository_manager(new SqliteDatabase("Database/game.db")), 
  player_manager(new PlayerManager(component_serializer)),
  gameobject_manager(new GameObjectManager(repository_manager->get_gameobjects_repository(), component_serializer)),
  zone_manager(new ZoneManager(gameobject_manager))
{
}

Server::~Server()
{
}

void Server::set_motd(const std::string &motd)
{
	this->motd = motd;
}

void Server::accept_connections()
{
	accepting_connections = true;

	log_event("Server", "Accepting connections");
}

void Server::run(Event &stop_event)
{
	try
	{
		initialize_network();

		on_init();

		while (!stop_event.wait(0))
		{
			try
			{
				int ticks = tick_timer.update();

				// for (int i = 0; i < ticks; i++) 
				// {
				//		gameobject_manager->tick(tick_timer.get_tick_length());
				// }
				gameobject_manager->tick(ticks * tick_timer.get_tick_length());
				player_manager->tick(ticks * tick_timer.get_tick_length());
				on_tick(ticks * tick_timer.get_tick_length());

				KeepAlive::process(10);
			}
			catch (Exception &e)
			{
				log_event("Exception", e.get_message_and_stack_trace());
			}
		}

		network.stop();
	}
	catch (Exception &e)
	{
		log_event("Exception", e.get_message_and_stack_trace());
	}

	log_event("Server", "Shutting down server");
}

void Server::initialize_network()
{
	network.start("4556");

	slots.connect(network.sig_client_connected(), this, &Server::on_client_connected);
	slots.connect(network.sig_client_disconnected(), this, &Server::on_client_disconnected);
	slots.connect(network.sig_event_received(), this, &Server::on_net_event_received);

	netevents.func_event(CTS_LOGIN).set(this, &Server::on_net_event_login);
	netevents.func_event(CTS_PING).set(this, &Server::on_net_event_ping);

	log_event("Network", "Network initialized");
}

void Server::on_client_connected(NetGameConnection *connection)
{
	if(accepting_connections == false)
		return;

	log_event("Network", "Client connected");

	// This pointer is actually stored on the userdata of the connection
	// and cleaned up when disconnected
	new ServerUser(connection);

	if(motd.length())
		connection->send_event(NetGameEvent(STC_MOTD, motd));
}

void Server::on_client_disconnected(NetGameConnection *connection, const std::string &reason)
{
	log_event("Network", "Client disconnected: " + reason);

	auto user = ServerUser::get_user(connection);
	if(user)
	{
		if(user->is_logged_in())
		{
			PlayerPtr player = player_manager->find_player(connection);
			zone_manager->remove_player(player);
			player_manager->remove_player(connection);
		}

		delete user;
	}
}

void Server::on_net_event_received(NetGameConnection *connection, const NetGameEvent &e)
{
//	log_event("Network", "Client sent event: %1", e.to_string());

	auto user = ServerUser::get_user(connection);
	if(user)
	{
		bool handled_event = netevents.dispatch(e, user);

		if (user->is_logged_in())
		{
			auto player = player_manager->find_player(connection);
			if(player)
			{	
				handled_event |= on_net_event_received(e, player);
				if(!handled_event)
					handled_event |= gameobject_manager->dispatch_net_event(e, player);
			}
		}

		if (!handled_event)
			log_event("Security", "Unhandled event: %1", e.to_string());
	}
}

void Server::on_net_event_login(const NetGameEvent &e, ServerUser *user)
{
	std::string user_name = e.get_argument(0);
	std::string password = e.get_argument(1);

	repository_manager->get_users_repository()->login(user_name, password, 
		[this, user, user_name](int user_id) 
		{ 
			if(user_id)
			{
				try
				{
					player_manager->add_player(user_id, user->get_connection());

					user->set_logged_in();
					user->send_event(NetGameEvent(STC_LOGIN_SUCCESS));

					log_event("Server", string_format("Client logged in: %1 (%2)", user_id, user_name));
				}
				catch(Exception ex)
				{
					user->send_event(NetGameEvent(STC_LOGIN_FAIL, "Failed to login"));

					log_event("Security", "Failed to login: " + ex.message);
				}
			}
			else
			{
				user->send_event(NetGameEvent(STC_LOGIN_FAIL, "Wrong username or password"));

				log_event("Security", "Client login failed: Wrong username or password");
			}
		}
	);
}

void Server::on_net_event_ping(const NetGameEvent &e, ServerUser *user)
{
	unsigned int time = e.get_argument(0);
	if(e.get_argument_count() > 1) 
	{
		// unsigned int previous_time = e.get_argument(1);
	}

	user->send_event(NetGameEvent(STC_PONG, time));
}
