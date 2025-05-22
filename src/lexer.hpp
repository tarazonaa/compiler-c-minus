/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el header file del lexer.
 * */
#pragma once

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
