#pragma once
#include "Time.hpp"
enum class Colour {
	GREEN,
	RED,
	GREY // still unresolved
};

struct Candle {
	Timestamp start_time;
	int open;
	int high;
	int low;
	int close;
	int volume;
	Colour colour;
};