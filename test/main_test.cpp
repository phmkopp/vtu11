#include "vtu11.hpp"

#include <iostream>

int main( )
{
  if( nice( ) != 7 )
  {
    std::cout << "Not nice." << std::endl;
  }
  else
  {
    std::cout << "Good." << std::endl;
  }

  return 0;
}

