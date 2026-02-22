module;
#include <format>
#include <stdexcept>
#include <string>
export module mjsonx.lexer;

import mjsonx.utils;
import mjsonx.types;

export namespace mjsonx::lexer
{
  enum class TokenType_t {
    _LBRACE,
    _RBRACE, // { }
    _LBRACKET,
    _RBRACKET, // [ ]
    _COLON,
    _COMMA, // : ,
    _STRING,
    _STRING_SPECIAL,
    _NUMBER,
    _FLOATING, // "abc", "ab\fc", 123, 12.31
    _TRUE,
    _FALSE,
    _NULL, // true, false, null
    _EOF
  };

  struct Place_t {
    unsigned int line{ 1 };
    unsigned int column{ 1 };
    unsigned int pos{ 0 };
  };

  struct LexerStruct_t {
    TokenType_t type;
    std::string word;
    Place_t place;
  };

  class LexerError : public std::runtime_error {
  public:
    LexerError(std::string_view msg, Place_t place) :
        runtime_error(
            std::format("{} in place: {}line {}column, all in file {} pos", msg, place.line, place.column, place.pos))
    { }
  };

  class Lexer {
    std::string input;
    Place_t place;
    char current;

  public:
    // 流式解析器
    explicit Lexer(std::string input) : input(std::move(input)), place({ 0, 0, 0 }), current(EOF) { this->next(); }
    LexerStruct_t getNext()
    {
      std::string text{};

      while (this->current != EOF)
      {
        // 跳过空白和换行
        if (isWhitespace(this->current) || isNextLine(this->current))
        {
          this->next();
          continue;
        }

        // 字符串
        if (this->current == '\"') { return makeStringLexerStruct(); }

        // 数字（包括负数）
        if (std::isdigit(this->current) || (this->current == '-' && std::isdigit(this->peek())))
        {
          return makeNumberLexerStruct<(this->current == '-')>();
        }
#define CASE_M(c, type)                                                                                                \
  case c: {                                                                                                            \
    this->next();                                                                                                      \
    return makeLexerStruct(type);                                                                                      \
  }
        switch (this->current)
        {
          CASE_M('{', TokenType_t::_LBRACE)
          CASE_M('}', TokenType_t::_RBRACE)
          CASE_M('[', TokenType_t::_LBRACKET)
          CASE_M(']', TokenType_t::_RBRACKET)
          CASE_M(':', TokenType_t::_COLON)
          CASE_M(',', TokenType_t::_COMMA)
          default: break;
        }
#undef CASE_M

        if (std::isalpha(this->current)) { return makeKeywordLexerStruct(); }

        // 非法字符
        throw LexerError(std::string_view("unexpected char: " + this->current), this->place);
      }

      return makeLexerStruct(TokenType_t::_EOF);
    }


  private:
    char next()
    {
      auto &[line, column, pos] = this->place;
      if (pos < this->input.length())
      {
        this->current = this->input[pos++];
        column++;
        if (this->current == '\r')
        {
          line++;
          column = 0;
          if (this->peek() == '\n') // CRLF这玩意谁琢磨的呢
            column++;
        }
        if (this->current == '\n')
        { // ONLY LF
          line++;
          column = 0;
        }
      }
      else
        this->current = EOF;
      return this->current;
    }

    char peek() const
    {
      const auto &pos = this->place.pos;
      return pos < this->input.length() ? this->input[pos] : EOF;
    }

    template<bool minus>
    LexerStruct_t makeNumberLexerStruct()
    {
      bool isFloat{ false };
      std::string result{};
      if constexpr (minus) result.push_back('-');
      result.push_back(this->current);

      while (std::isdigit(this->peek()) or this->peek() == '.')
      { // 保证下一个是数字或者点
        if (this->next() == '.')
        {
          if (isFloat)
          {
            throw LexerError("multiple dots", this->place);
            continue;
          }
          isFloat = true;
          result.push_back(this->current);
          continue;
        }
        result.push_back(this->current);
      }
      this->next();

      return this->makeLexerStruct(isFloat ? TokenType_t::_FLOATING : TokenType_t::_NUMBER, result);
    }

    LexerStruct_t makeStringLexerStruct()
    {
      std::string result{};
      bool hasSpecial{ false };
      while (this->next() != '"' and this->current != EOF)
      {
        if (this->current == '\\')
        {
          switch (this->next())
          {
            case 'n':
              result.push_back('\n');
              hasSpecial = true;
              break;
            case 'r':
              result.push_back('\r');
              hasSpecial = true;
              break;
            case 't':
              result.push_back('\t');
              hasSpecial = true;
              break;
            case '\\':
              result.push_back('\\');
              hasSpecial = true;
              break;
            case '\"':
              result.push_back('\"');
              hasSpecial = true;
              break;
            case '\'':
              result.push_back('\'');
              hasSpecial = true;
              break;
            default: throw LexerError(std::string_view("Unknown escape: \\" + this->current) , this->place);
          }
        }
        else
          result.push_back(this->current);
      }
      if (this->current == EOF) throw LexerError("Unclosed string", this->place);
      this->next();
      return this->makeLexerStruct(hasSpecial ? TokenType_t::_STRING_SPECIAL : TokenType_t::_STRING, result);
    }

    LexerStruct_t makeKeywordLexerStruct()
    {
      std::string word;

      while (isalpha(this->current) && this->current != EOF)
      {
        word.push_back(this->current);
        this->next();
      }

      // 判断关键字
      if (word == "true")
        return this->makeLexerStruct(TokenType_t::_TRUE, word);
      if (word == "false")
        return this->makeLexerStruct(TokenType_t::_FALSE, word);
      if (word == "null")
        return this->makeLexerStruct(TokenType_t::_NULL, word);

      // 不是关键字，报错（JSON 不允许裸标识符）
      throw LexerError(std::string_view( "unknown keyword: " + word), this->place);
    }

    inline LexerStruct_t makeLexerStruct(TokenType_t token_type, const std::string &value = "") const
    { return LexerStruct_t{ token_type, std::move(value), this->place }; }
  };
} // namespace mjsonx::lexer
