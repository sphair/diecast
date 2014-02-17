
#pragma once

class Player; typedef std::shared_ptr<Player> PlayerPtr;

class NetworkReceiver
{
public:
	virtual bool dispatch_net_event(const clan::NetGameEvent &event, const PlayerPtr &player) = 0;
};
