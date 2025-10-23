#include "player.hpp"
#include <algorithm>
#include <iostream>
#include <utility>

Player::Player() {};
Player::Player(char initial_role, bool initial_team)
    : role(initial_role), team(initial_team), position{500, 500} {}

void Player::move(float x, float y)
{
  x = x * (x > 0 && x < 1000);
  y = y * (y > 0 && y < 1000);
  position = {x, y};
};

std::pair<float, float> Player::getPosition() { return position; }

void Player::getInfo() const
{
  std::cout << position.first << ", " << position.second << std::endl;
}
