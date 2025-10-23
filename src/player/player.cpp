#include "player.hpp"
#include <algorithm>
#include <iostream>
#include <utility>

Player::Player(char initial_role) : role(initial_role) { position = {0, 0}; };

void Player::move(int x, int y) {
  x = std::clamp(x, 0, 1000);
  y = std::clamp(y, 0, 1000);
  position = {x, y};
};

std::pair<int, int> Player::getPosition() { return position; }

void Player::getInfo() const {
  std::cout << position.first << ", " << position.second << std::endl;
}
