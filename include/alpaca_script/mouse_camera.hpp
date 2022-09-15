#pragma once

#include <Game/Core.hpp>

namespace as::mouse_camera {
bool rotate_to(double angle, double deviation = 10) {
  const auto angle_diff = [](double angle, double target) {
    if (target > 360)
      target = 360;
    else if (target < 0)
      target = 0;

    if (angle > 360)
      angle = 360;
    else if (angle < 0)
      angle = 0;

    double diff = target - angle;
    if (diff > 180)
      diff = diff - 360;
    else if (diff < -180)
      diff = diff + 360;

    return diff;
  };

  const auto current_angle = Camera::GetAngle();

  const auto diff = angle_diff(current_angle, angle);
  if (std::abs(diff) < deviation)
    return true;

  const auto canvas = Internal::Client.GetCanvas();

  const auto box = [&]() {
    if (diff > 0) {
      return Box(0, 0, canvas.GetWidth() / 2, canvas.GetHeight());
    } else {
      return Box(canvas.GetWidth() / 2, 0, canvas.GetWidth() / 2,
                 canvas.GetHeight());
    }
  }();

  const auto start = box.GetProfileHybridRandomPoint();

  constexpr auto angle_per_pixel = 2.80;
  const auto angle_to_rotate = diff * angle_per_pixel;

  const auto end = start + Point(NormalRandom(angle_to_rotate, deviation),
                                 UniformRandom(-5, 5));

  Interact::MoveMouse(start);
  MouseDown(Button::BUTTON_MIDDLE);
  Interact::MoveMouse(end);
  MouseUp(Button::BUTTON_MIDDLE);

  const auto new_angle = Camera::GetAngle();
  const auto result_dist = std::abs(angle_diff(angle, new_angle));
  return result_dist < deviation;
}

bool rotate_to(Tile tile, double deviation = 10) {
  if (!tile)
    return false;

  constexpr auto pi = std::atan(1) * 4;
  Tile player_pos = Minimap::GetPosition();
  Point me = Internal::TileToMainscreen(player_pos, 0, 0, 0);
  Point target = Internal::TileToMainscreen(tile, 0, 0, 0);

  double theta = Camera::GetAngle() +
                 (((std::atan2((target.Y - me.Y), (target.X - me.X)) + pi / 2) *
                   180 / pi) -
                  0);

  while (theta < 0)
    theta += 360;
  while (theta > 360)
    theta -= 360;

  // return true;
  return rotate_to(theta, deviation);
}

bool set_pitch(double pitch, double tolerance = 10, double deviation = 10) {
  const auto current_pitch = Camera::GetPitch();
  const auto diff = current_pitch - pitch;
  if (std::abs(diff) < tolerance)
    return true;

  const auto canvas = Internal::Client.GetCanvas();

  const auto canvas_width = canvas.GetWidth();
  const auto canvas_height = canvas.GetHeight();

  const auto box = [&]() {
    if (diff > 0) {
      return Box(0, canvas_height / 2, canvas_width, canvas_height / 2);
    } else {
      return Box(0, 0, canvas_width, canvas_height / 2);
    }
  }();

  const auto start = box.GetProfileHybridRandomPoint();

  const auto end =
      start + Point(UniformRandom(-5, 5), NormalRandom(-diff * 0.5, deviation));

  Interact::MoveMouse(start);
  MouseDown(Button::BUTTON_MIDDLE);
  Interact::MoveMouse(end);
  MouseUp(Button::BUTTON_MIDDLE);

  const auto new_pitch = Camera::GetPitch();
  const auto result_dist = std::abs(new_pitch - pitch);
  return result_dist < deviation;
}

} // namespace as::mouse_camera