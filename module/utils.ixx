module;
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
    static thread_local std::ostringstream oss;
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
    // 适配 JSON 浮点数规范，去掉无意义的小数位
    oss << std::defaultfloat; // 禁用科学计数法
    if (value == static_cast<double>(static_cast<int64_t>(value))) { oss << static_cast<int64_t>(value); } // 整数类浮点
    else
    {
      oss << std::setprecision(15) << value;
    } // JSON 通常保留15位小数
    return oss.str();
  }


} // namespace mjsonx
