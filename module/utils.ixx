module;
#include <array>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
export module mjsonx.utils;

export namespace mjsonx
{
  template<typename T>
  inline std::ostringstream &get_thread_local_oss()
  {
    thread_local std::ostringstream oss;
    // 清空缓冲区
    oss.str("");
    oss.clear();
    return oss;
  }

  // 整数序列化
  template<typename IntType> // 任意 int 类型
  std::string serialize_integer(IntType value)
  {
    auto &oss = get_thread_local_oss<IntType>();
    oss << value;
    return oss.str();
  }

  // 浮点数序列化
  std::string serialize_floating(double value)
  {
    auto &oss = get_thread_local_oss<double>();
    oss << std::defaultfloat; // 禁用科学计数法
    if (value == static_cast<double>(static_cast<int64_t>(value))) { oss << static_cast<int64_t>(value); } // 整数类浮点
    else
    {
      oss << std::setprecision(15) << value;
    } // JSON 通常保留15位小数
    return oss.str();
  }

  constexpr std::array SPACE_TABLE = { ' ', '\f', '\t' };
  constexpr std::array NEXT_TABLE = { '\n', '\r' };

  // constexpr std::array NOTE_TABLE = {';', '#'}; // JSON 标准未提及注释

  template<typename Table>
  constexpr bool isInTable(const Table &table, char c)
  { return std::find(table.begin(), table.end(), c) != table.end(); }

  constexpr bool isWhitespace(char c) { return isInTable(SPACE_TABLE, c); }
  constexpr bool isNextLine(char c) { return isInTable(NEXT_TABLE, c); }
  // constexpr bool isNote(char c) { return isInTable(NOTE_TABLE, c); }

} // namespace mjsonx
