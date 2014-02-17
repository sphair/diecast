#pragma once

class Player; typedef std::shared_ptr<Player> PlayerPtr;
class ComponentSerializer; typedef std::shared_ptr<ComponentSerializer> ComponentSerializerPtr;

class PlayerManager
{
// Construction:
public:
	PlayerManager(const ComponentSerializerPtr &component_serializer);
	~PlayerManager();

// Signals:
public:
	clan::Signal_v1<const PlayerPtr &> sig_player_added;
	clan::Signal_v1<const PlayerPtr &> sig_player_removed;

// Attributes:
public:
	PlayerPtr find_player(int user_id) const;
	PlayerPtr find_player(clan::NetGameConnection *connection) const;

// Operations:
public:
	PlayerPtr add_player(int user_id, clan::NetGameConnection *connection);
	void remove_player(clan::NetGameConnection *connection);
	void remove_player(PlayerPtr player);
	void kick_player(const PlayerPtr &player, const std::string &reason);

	void tick(float time_elapsed);

// Implementation:
private:
	std::vector<PlayerPtr> players;

	ComponentSerializerPtr component_serializer;
};
