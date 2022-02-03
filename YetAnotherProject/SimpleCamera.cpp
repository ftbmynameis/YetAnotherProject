#include "SimpleCamera.hpp"

#include <cmath>
#include "utility.hpp"

SimpleCamera::SimpleCamera(const vec3f& initial_pos)
    : _initial_pos(initial_pos), _up_vec(0.0f, 1.0f, 0.0f)
{
    reset();
}

void SimpleCamera::on_key_down(WPARAM key)
{
    switch (key)
    {
    case 'W':
        _pressed_keys.w = true;
        break;
    case 'A':
        _pressed_keys.a = true;
        break;
    case 'S':
        _pressed_keys.s = true;
        break;
    case 'D':
        _pressed_keys.d = true;
        break;
    case VK_LEFT:
        _pressed_keys.left = true;
        break;
    case VK_RIGHT:
        _pressed_keys.right = true;
        break;
    case VK_UP:
        _pressed_keys.up = true;
        break;
    case VK_DOWN:
        _pressed_keys.down = true;
        break;
    case VK_ESCAPE:
        reset();
        break;
    }
}

void SimpleCamera::on_key_up(WPARAM key)
{
    switch (key)
    {
    case 'W':
        _pressed_keys.w = false;
        break;
    case 'A':
        _pressed_keys.a = false;
        break;
    case 'S':
        _pressed_keys.s = false;
        break;
    case 'D':
        _pressed_keys.d = false;
        break;
    case VK_LEFT:
        _pressed_keys.left = false;
        break;
    case VK_RIGHT:
        _pressed_keys.right = false;
        break;
    case VK_UP:
        _pressed_keys.up = false;
        break;
    case VK_DOWN:
        _pressed_keys.down = false;
        break;
    }
}

void SimpleCamera::reset()
{
    _pos = _initial_pos;
    _yaw = util::math::pi();
    _pitch = 0.0f;
    _look_dir = { 0, 0, -1 };
}

void SimpleCamera::update(float frametime)
{
    // Calculate the move vector in camera space.
    vec3f move(0, 0, 0);

    if (_pressed_keys.a)
        move.x -= 1.0f;
    if (_pressed_keys.d)
        move.x += 1.0f;
    if (_pressed_keys.w)
        move.z -= 1.0f;
    if (_pressed_keys.s)
        move.z += 1.0f;

    if (fabs(move.x) > 0.1f && fabs(move.z) > 0.1f)
    {
        const auto move_normalized = vec::normalice(move);
        move.x = move_normalized.x;
        move.z = move_normalized.y;
    }

    float moveInterval = _move_speed * frametime;
    float rotateInterval = _move_speed * frametime;

    if (_pressed_keys.left)
        _yaw += rotateInterval;
    if (_pressed_keys.right)
        _yaw -= rotateInterval;
    if (_pressed_keys.up)
        _pitch += rotateInterval;
    if (_pressed_keys.down)
        _pitch -= rotateInterval;

    
    // Prevent looking too far up or down.
    _pitch = std::min(_pitch, util::math::pi4());
    _pitch = std::max(-util::math::pi4(), _pitch);

    // Move the camera in model space.
    float x = move.x * -cosf(_yaw) - move.z * sinf(_yaw);
    float z = move.x * sinf(_yaw) - move.z * cosf(_yaw);
    _pos.x += x * moveInterval;
    _pos.z += z * moveInterval;

    // Determine the look direction.
    float r = cosf(_pitch);
    _look_dir.x = r * sinf(_yaw);
    _look_dir.y = sinf(_pitch);
    _look_dir.z = r * cosf(_yaw);
}

mat4f SimpleCamera::get_view() const
{
    return mat::look_to(_pos, _look_dir, _up_vec);
}