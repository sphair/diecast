
#pragma once

class TickTimer
{
public:
	TickTimer();
	void reset();
	int update();
	float get_tick_length() const { return tick_length / 1000.0f; }

private:
	clan::ubyte64 last_tick_time;

	static const unsigned int tick_length = 16;
};
