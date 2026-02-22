module;
#include <optional>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
export module mjsonx.ast;

export namespace mjsonx::parser::ast
{
  struct array; // 前置声明

  using value_t = std::optional<std::variant<std::string_view, std::int64_t, double, bool, array>>;

  struct array {
    std::vector<value_t> elements;
  };

  using node_t = std::unordered_map<std::string_view, std::optional<value_t>>;
} // namespace mjsonx::parser::ast
