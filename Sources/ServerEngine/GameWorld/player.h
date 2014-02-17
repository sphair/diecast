#pragma once

class ReplicatedContainer; typedef std::shared_ptr<ReplicatedContainer> ReplicatedContainerPtr;
class ComponentSerializer; typedef std::shared_ptr<ComponentSerializer> ComponentSerializerPtr;

class Player
{
// Construction:
public:
	Player(int user_id, clan::NetGameConnection *connection, const ComponentSerializerPtr &component_serializer);
	~Player();

// Attributes:
public:
	clan::NetGameConnection *get_connection() const { return connection; }

	int get_user_id() const { return user_id; }

    ReplicatedContainerPtr get_replication_container() const { return replicated_container; }

// Operations:
public:
	void send_event(const clan::NetGameEvent &event);

// Implementation:
private:
	clan::NetGameConnection *connection;

    ReplicatedContainerPtr replicated_container;

    int user_id;
};

typedef std::shared_ptr<Player> PlayerPtr;
