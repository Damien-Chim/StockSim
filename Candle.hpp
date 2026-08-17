#pragma once
enum class Colour {
	GREEN,
	RED,
	GREY // still unresolved
};

struct Candle {
	int start_time;
	int open;
	int high;
	int low;
	int close;
	int volume;
	Colour colour;
};