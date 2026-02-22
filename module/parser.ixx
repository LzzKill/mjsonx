module;
#include <cstdint>
#include <format>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
export module mjsonx.parser;

import mjsonx.lexer;
import mjsonx.types;

export namespace mjsonx::parser::ast
{
  struct array; // 前置声明

  using value_t = std::optional<std::variant<std::string_view, std::int64_t, double, bool, array>>;

  struct array {
    std::vector<value_t> elements;
  };

  using node_t = std::unordered_map<std::string_view, std::optional<value_t>>;
} // namespace mjsonx::parser::ast

export namespace mjsonx::parser
{
  class parser_error : public std::runtime_error {
  public:
    parser_error(std::string_view msg, lexer::Place_t place) :
        runtime_error(
            std::format("{} in place: {}line {}column, all in file {} pos", msg, place.line, place.column, place.pos))
    { }
  };

  class parser {

  };

} // namespace mjsonx::parser
