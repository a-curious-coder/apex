#include "player.hpp"
#include <string>

class Jammer : public Player {
public:
  std::string shape;
  void getInfo() const;
};
