#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <utility>

class Player
{
public:
  Player();
  Player(char, bool);
  void move(float, float);
  std::pair<float, float> getPosition();
  void getInfo() const;
  char role;
  bool team;

private:
  std::pair<float, float> position;
};

#endif
