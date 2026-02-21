module;
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>
export module mjsonx.types;

import mjsonx.utils;

namespace mjsonx::type
{
  template<typename T>
  class JSON_type {
  protected:
    std::optional<T> object; // 可能为空
    T default_value;

  public:
    explicit JSON_type(T default_value) : default_value(default_value) { this->object = std::nullopt; }
    explicit JSON_type() { this->object = std::nullopt; } // 默认构造
    [[nodiscard]] std::optional<T> get_object() const { return this->object; }
    void apply(T value) { this->object.emplace(value); }
    void apply() { this->object.emplace(this->default_value); }
    virtual ~JSON_type() = default;
    [[nodiscard]] virtual std::string to_string() const = 0; // TODO: Use std::string_view
  };

  // JSON Object;

  class boolean_t : public JSON_type<bool> {
  public:
    explicit boolean_t(bool default_value) : JSON_type(default_value) { }
    explicit boolean_t() = default;
    std::string to_string() const override
    { return (this->object.has_value()) ? (this->object.value() ? "true" : "false") : "null"; }
  };

  template<typename IntType>
  class integer_base_t : public JSON_type<IntType> {
  public:
    explicit integer_base_t(IntType default_value) : JSON_type<IntType>(default_value) { }
    explicit integer_base_t() = default;
    std::string to_string() const override
    {
      if (this->object.has_value()) return serialize_integer<IntType>(this->object.value());
      return "null";
    }
  };

  class floating_t : public JSON_type<double> {
  public:
    explicit floating_t(double default_value) : JSON_type(default_value) { }
    explicit floating_t() = default;
    std::string to_string() const override
    {
      if (this->object.has_value()) return serialize_floating(this->object.value());
      return "null";
    }
  };

  class string_t : public JSON_type<std::string> {
  public:
    explicit string_t(const std::string &default_value) : JSON_type(default_value) { }
    explicit string_t() = default;
    std::string to_string() const override
    {
      if (this->object.has_value()) return "\"" + this->object.value() + "\"";
      return "null";
    }
  };

  export template<typename T>
  concept json_object = std::is_same_v<boolean_t, T> || std::is_same_v<integer_base_t<int64_t>, T> ||
                        std::is_same_v<integer_base_t<uint64_t>, T> || std::is_same_v<string_t, T>; // 单独导出

  // 这是一个一维数组单类型特化
  template<json_object json_type, int N = 0> // N 为预分配
  class array_element_same_t : public JSON_type<std::vector<json_type>> {
  public:
    explicit array_element_same_t(const std::vector<json_type> &default_value) :
        JSON_type<std::vector<json_type>>(default_value)
    {
      static_assert(N >= 0, "Reserve size must be non-negative");
      if constexpr (N > 0) this->default_value.reserve(N);
    }
    explicit array_element_same_t() : JSON_type<std::vector<json_type>>()
    {
      static_assert(N >= 0, "Reserve size must be non-negative");
      if constexpr (N > 0) this->default_value.reserve(N);
    }

    std::string to_string() const override
    {
      if (!this->object.has_value()) return "[]";

      std::string result = "[";
      const auto &vec = this->object.value();
      for (const auto &&m : vec)
      {
        result += m.to_string();
        result += ",";
      }
      if (result.length() > 1) result.back() = ']';
      else result += "]";
      return result;
    }
  };

  template<typename... Elements>
  class array_t;

  // 辅助：判断类型是否是 array
  template<typename T>
  struct is_array : std::false_type { };
  template<typename... E>
  struct is_array<array_t<E...>> : std::true_type { };


  // 递归 variant：可以包含自身
  template<typename... Elements>
  class array_t : public JSON_type<std::vector<std::variant<Elements...>>> {
  public:
    using VariantType = std::variant<Elements...>;
    using VectorType = std::vector<VariantType>;

    // 关键：Elements... 可以包含 array<...> 实现嵌套
    using Base = JSON_type<VectorType>;

    explicit array_t(size_t reserve_n = 0) : Base(VectorType())
    {
      if (reserve_n > 0) this->default_value.reserve(reserve_n);
    }

    // 支持嵌套数组的 to_string
    std::string to_string() const override
    {
      if (!this->object.has_value()) return "[]";

      std::string result = "[";
      const auto &vec = this->object.value();
      for (const auto &&m : vec)
      {
        result += m.to_string();
        result += ",";
      }
      if (result.length() > 1) result.back() = ']';
      else result += "]";
      return result;
    }
  };


} // namespace mjsonx::type

namespace mjsonx::type
{
  export using boolean = boolean_t;
  export using const_boolean = boolean_t const;

  export using integer = integer_base_t<int64_t>;
  export using const_integer = integer_base_t<int64_t> const;

  export using uinteger = integer_base_t<uint64_t>;
  export using const_uinteger = integer_base_t<uint64_t> const;

  export using floating = floating_t;
  export using const_floating = floating_t const;

  export using string = string_t;
  export using const_string = string_t const;

  export template<json_object json_type>
  using array_same = array_element_same_t<json_type>;

  export template<json_object json_type>
  using const_array_same = array_element_same_t<json_type> const;

  export template<typename... Elements>
  using array = array_t<Elements...>;

  export template<typename... Elements>
  using const_array = array_t<Elements...> const;

} // namespace mjsonx::type
