#include <iostream>

import mjsonx;

int main()
{
  using namespace mjsonx;

  struct mjson : JSON_Object {

    type::integer age{ 1 };
    type::string name{ "M" };
    type::array_same<type::string> body;
    type::array<type::string, type::array<type::string, type::integer>> other;
  };

  /*
   * {
   *    "age": 1,
   *    "name": "M",
   *    "body": ["Fe!", "Lam" ],
   *    "other" : []
   * }
   */

  std::cout << m.to_string();

  return 0;
}
