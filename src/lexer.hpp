#pragma once

#include <stdexcept>
#include <string>
#include <tuple>

enum class TokenType {
  // Palabras clave
  ELSE,
  IF,
  INT,
  RETURN,
  VOID,
  WHILE,

  // Simbolos especiales
  ADD,
  SUB,
  TIMES,
  DIV,
  LT,
  LTE,
  GT,
  GTE,
  EQ,
  NOT_EQ,
  ASSIGN,
  SEMI,
  COMMA,
  O_PAREN,
  C_PAREN,
  O_BRACKET,
  C_BRACKET,
  O_BRACE,
  C_BRACE,

  // IDS y NUMS
  ID,
  NUM,

  ERROR,

  // End file
  ENDFILE
};

enum StateType {
  // EN un indentificador
  INID,
  // En un numero
  INNUM,
  // En mayor que
  INGT,
  // En menor que
  INST,
  // En un slash (para comentarios y división)
  INSLASH,
  // En comentario
  INCOMMENT,
  // En asterisco para salir de comentario
  INASTERISK,
  // En un igual (Para asignación e igualdades)
  INEQ,
  // EN exclamación (para no igual que)
  INEXC,
  // En un array
  INARRAY,
  // En error
  INERROR,
  // Terminado
  DONE,
  // Iniciando
  START,
};

std::string tokenTypeToString(TokenType token);

class Lexer {
 private:
  int lineno = 1;
  int lineStart = 0;
  std::string program;
  int position = 0;
  int programLength = 0;
  std::string fileName;

 public:
  Lexer(const std::string& fileName) : fileName(fileName) {};
  void globales(const std::string& prog, int pos, int progLong);
  std::tuple<TokenType, std::string, int> getToken(bool imprime = true);
  TokenType reservedLookup(const std::string& token);
  // Regresa errores
  void throwSyntaxError(TokenType token);
  std::string messageForError(TokenType token);
  int getLineNo() const;
};

class LexerSyntaxError : public std::runtime_error {
 private:
  std::string fileName;
  int lineno;
  int positionInLine;
  std::string line;

 public:
  LexerSyntaxError()
      : std::runtime_error(""),
        lineno(0),
        fileName(""),
        positionInLine(0),
        line("") {}
  LexerSyntaxError(const std::string& message, const std::string& fileName,
                   int lineno, int positionInLine, const std::string& linea)
      : std::runtime_error(message),
        lineno(lineno),
        fileName(fileName),
        positionInLine(positionInLine),
        line(linea) {}
  std::string format() const;
  int getLineNo() const;
  int getPositionInLine() const;
  const std::string& getLine() const;
  const std::string& getFileName() const;
};
