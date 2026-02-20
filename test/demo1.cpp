#include <iostream>

import mjsonx.types;
import mjsonx.utils;

int main()
{
  mjsonx::type::boolean m(true);

  std::cout << m.to_string();

  return 0;
}