#pragma once

#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include "vec.hpp"
#include "mat4.hpp"

class SimpleCamera
{
public:
	SimpleCamera(const vec3f& initial_pos);

	void on_key_down(WPARAM param);
	void on_key_up(WPARAM param);
	void reset();
	void update(float frametime);
	mat4f get_view() const;

private:
	struct PressedKeys
	{
		bool w;
		bool a;
		bool s;
		bool d;

		bool left;
		bool right;
		bool up;
		bool down;
	};

	PressedKeys _pressed_keys;

	float _pitch_limit;

	float _move_speed;
	float _turn_speed;

	vec3f _initial_pos;
	vec3f _pos;
	vec3f _look_dir;
	float _pitch;
	float _yaw;

	const vec3f _up_vec;
};