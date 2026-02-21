/*
 * 反序列化
 *
 */
module;
#include <string>
#include <variant>
export module mjsonx.serirlizer;

import mjsonx.types;

export namespace mjsonx::serializer
{
  template<typename T>
  struct serializer;

  template<>
  struct serializer<type::integer> {
    static std::string serialize(const type::integer &value) { return value.to_string(); }
  };

  template<>
  struct serializer<type::uinteger> {
    static std::string serialize(const type::uinteger &value) { return value.to_string(); }
  };

  template<>
  struct serializer<type::string> {
    static std::string serialize(const type::string &value)
    {
      if (!value.get_object().has_value()) return "null";
      return "\"" + value.to_string() + "\"";
    }
  };

  template<type::json_object json_t>
  struct serializer<type::array<json_t>> {
    static std::string serialize(const type::array<json_t> &value)
    {
      if (!value.get_object().has_value()) return "[]";

      std::string result = "[";
      const auto &vec = value.get_object();
      for (const auto &&m : vec)
      {
        result += m.to_string();
        result += ",";
      }
      if (!result.length() > 1)
        result.back() = ']';
      else
        result += "]";
      return result;
    }
  };

  template<typename... Elements>
  struct serializer<type::array<Elements...>> {
    static std::string serialize(const type::array<Elements...> &value)
    {
      if (!value.get_object().has_value()) return "null";

      std::string result = "[";
      const auto &vec = value.get_object().value();

      for (const auto &m : vec)
      {
        std::visit(
            [&]<typename T0>(const T0 &elem) {
              using T = std::decay_t<T0>;
              result += serializer<T>::serialize(elem);
            },
            m);

        result += ",";
      }

      if (!vec.length() > 1)
        result.back() = ']';
      else
        result += "]";

      return result;
    }
  };
} // namespace mjsonx::serializer
