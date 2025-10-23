#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <utility>

class Player
{
public:
  Player(char);
  void move(int, int);
  std::pair<int, int> getPosition();
  void getInfo() const;
  char role;

private:
  std::pair<int, int> position;
  int position_x;
  int position_y;
};

#endif
