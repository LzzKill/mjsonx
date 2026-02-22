module;
#include <stdexcept>
#include <string_view>
#include <format>
export module mjsonx.parser;

import mjsonx.lexer;
import mjsonx.types;

export namespace mjsonx::parser
{
  class parser_error : public std::runtime_error {
  public:
    parser_error(std::string_view msg, Place_t place) :
        runtime_error(
            std::format("{} in place: {}line {}column, all in file {} pos", msg, place.line, place.column, place.pos))
    { }
  };
}