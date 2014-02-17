
#pragma once

class ServerUser
{
public:
	static ServerUser *get_user(clan::NetGameConnection *connection);

public:
	ServerUser(clan::NetGameConnection *connection);

	clan::NetGameConnection *get_connection() const;
	void send_event(const clan::NetGameEvent &event);

	bool is_logged_in() const;
	void set_logged_in();

private:
	clan::NetGameConnection *connection;

	bool logged_in;
};
