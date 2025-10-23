#include "player.hpp"
#include <iostream>

int position_x = 0;
int position_y = 0;

void Player::move(int x, int y) {
  position_x += x;
  position_y += y;
};

int Player::position() { return 1; }

void Player::getInfo() const {
  std::cout << "This player's position is unknown" << std::endl;
}
