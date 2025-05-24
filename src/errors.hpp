/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el header file de errores.
 * */
#pragma once
#include <stdexcept>
#include <string>

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

class ParserSyntaxError : public std::exception {
 private:
  std::string message;

 public:
  ParserSyntaxError(const std::string& msg) : message(msg) {}

  const char* what() const noexcept override { return message.c_str(); }
};

class SemanticError : public std::runtime_error {
 private:
  std::string fileName;
  int lineno;
  int positionInLine;
  std::string line;

 public:
  SemanticError()
      : std::runtime_error(""),
        lineno(0),
        fileName(""),
        positionInLine(0),
        line("") {}

  SemanticError(const std::string& msg) : std::runtime_error(msg) {};
  SemanticError(const std::string& message, const std::string& fileName,
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
