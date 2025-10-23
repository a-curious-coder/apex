#include "jammer.hpp"
#include <iostream>

std::string shape = "star";
Jammer::Jammer() {};
void Jammer::getInfo() const
{
  std::cout << "This player's position is Jammer" << std::endl;
  std::cout << shape << std::endl;
}
