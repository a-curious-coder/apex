#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player {
public:
  Player();
  void move(int, int);
  int position();
  void getInfo() const;

private:
  int position_x;
  int position_y;
  int type;
};

#endif
