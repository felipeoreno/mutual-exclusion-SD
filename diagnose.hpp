#ifndef DIAGNOSE_HPP
#define DIAGNOSE_HPP

#include <iostream>
#include <stdlib.h>
#include <string>

inline void diagnose(bool isGood, const std::string& action)
{
  if (isGood)
    std::cout << action.c_str() << " OK" << std::endl;
  else
  {
    std::cerr << action.c_str() << " error" << std::endl;
    exit(1);
  }
}

#endif