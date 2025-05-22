/*
 * Este archivo es para la implementación del lexer
 * Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */
#include "lexer.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <tuple>

#include "errors.hpp"

void Lexer::globales(const std::string &prog, int pos, int progLong) {
  program = prog;
  position = pos;
  programLength = progLong;
}

int Lexer::getLineNo() const { return lineno; }

std::string Lexer::messageForError(TokenType token) {
  switch (token) {
    case TokenType::NUM:
      return "Error de sintáxis en número";
    case TokenType::ID:
      return "Error de sintáxis en identificador";
    case TokenType::ENDFILE:
      return "Error de sintáxis, $ es un caracter inválido";
    default:
      return "Error de sintáxis";
  }
}

void Lexer::throwSyntaxError(TokenType token) {
  int posicionEnLinea = position - lineStart;
  int lineEnd = program.find('\n', lineStart);
  if (lineEnd == std::string::npos) {
    lineEnd = program.size();
  }

  std::string line = program.substr(lineStart, lineEnd - lineStart);

  std::string message = messageForError(token);

  throw LexerSyntaxError(message, fileName, lineno, posicionEnLinea, line);
}

std::string tokenTypeToString(TokenType type) {
  switch (type) {
    // Palabras clave
    case TokenType::ELSE:
      return "ELSE";
    case TokenType::IF:
      return "IF";
    case TokenType::INT:
      return "INT";
    case TokenType::RETURN:
      return "RETURN";
    case TokenType::VOID:
      return "VOID";
    case TokenType::WHILE:
      return "WHILE";

    // Simbolos especiales
    case TokenType::ADD:
      return "ADD";
    case TokenType::SUB:
      return "SUB";
    case TokenType::TIMES:
      return "TIMES";
    case TokenType::DIV:
      return "DIV";
    case TokenType::LT:
      return "LT";
    case TokenType::LTE:
      return "LTE";
    case TokenType::GT:
      return "GT";
    case TokenType::GTE:
      return "GTE";
    case TokenType::EQ:
      return "EQ";
    case TokenType::NOT_EQ:
      return "NOT_EQ";
    case TokenType::ASSIGN:
      return "ASSIGN";
    case TokenType::SEMI:
      return "SEMI";
    case TokenType::COMMA:
      return "COMMA";
    case TokenType::O_PAREN:
      return "O_PAREN";
    case TokenType::C_PAREN:
      return "C_PAREN";
    case TokenType::O_BRACKET:
      return "O_BRACKET";
    case TokenType::C_BRACKET:
      return "C_BRACKET";
    case TokenType::O_BRACE:
      return "O_BRACE";
    case TokenType::C_BRACE:
      return "C_BRACE";
    // IDS y NUMS
    case TokenType::ID:
      return "ID";
    case TokenType::NUM:
      return "NUM";

    // End file
    case TokenType::ENDFILE:
      return "ENDFILE";

    case TokenType::ERROR:
      return "ERROR";

    default:
      return "UNKNOWN";
  }
}

TokenType Lexer::reservedLookup(const std::string &token) {
  if (token == "while") return TokenType::WHILE;
  if (token == "int") return TokenType::INT;
  if (token == "void") return TokenType::VOID;
  if (token == "return") return TokenType::RETURN;
  if (token == "if") return TokenType::IF;
  if (token == "else") return TokenType::ELSE;

  return TokenType::ID;
}

std::tuple<TokenType, std::string, int> Lexer::getToken(bool imprime) {
  std::string tokenString;
  TokenType tokenType;
  StateType state = StateType::START;
  bool save = true;

  while (state != StateType::DONE) {
    if (position >= programLength) {
      tokenType = TokenType::ENDFILE;
      state = StateType::DONE;
      save = false;
      break;
    }

    char ch = program[position];
    save = true;
    if (state == StateType::START) {
      if (std::isdigit(ch)) {
        state = StateType::INNUM;
      } else if (std::isalpha(ch)) {
        state = StateType::INID;
      } else if (ch == '=') {
        state = StateType::INEQ;
      } else if (ch == '<') {
        state = StateType::INST;
      } else if (ch == '>') {
        state = StateType::INGT;
      } else if (ch == '/') {
        state = StateType::INSLASH;
      } else if (ch == '!') {
        state = StateType::INEXC;
      } else if (ch == '*') {
        state = StateType::DONE;
        tokenType = TokenType::TIMES;
      } else if (ch == '+') {
        state = StateType::DONE;
        tokenType = TokenType::ADD;
      } else if (ch == '-') {
        state = StateType::DONE;
        tokenType = TokenType::SUB;
      } else if (ch == '[') {
        state = StateType::DONE;
        tokenType = TokenType::O_BRACKET;
      } else if (ch == ']') {
        state = StateType::DONE;
        tokenType = TokenType::C_BRACKET;
      } else if (ch == '(') {
        state = StateType::DONE;
        tokenType = TokenType::O_PAREN;
      } else if (ch == ')') {
        state = StateType::DONE;
        tokenType = TokenType::C_PAREN;
      } else if (ch == '{') {
        state = StateType::DONE;
        tokenType = TokenType::O_BRACE;
      } else if (ch == '}') {
        state = StateType::DONE;
        tokenType = TokenType::C_BRACE;
      } else if (ch == ',') {
        state = StateType::DONE;
        tokenType = TokenType::COMMA;
      } else if (ch == ';') {
        state = StateType::DONE;
        tokenType = TokenType::SEMI;
      } else if (ch == ' ' or ch == '\t' or ch == '\n') {
        save = false;
        if (ch == '\n') {
          lineno++;
          lineStart = position + 1;
        };
      } else if (ch == '$') {
        if (position + 1 < programLength) {
          char charNext = program[position + 1];

          throwSyntaxError(TokenType::ENDFILE);
        }
      }
    } else if (state == StateType::INNUM) {
      if (!std::isdigit(ch)) {
        if (std::isalpha(ch)) {
          state = StateType::INERROR;
          throwSyntaxError(TokenType::NUM);
        }
        state = StateType::DONE;
        if (position <= programLength) {
          position--;
        }
        tokenType = TokenType::NUM;
        save = false;
      }
    } else if (state == StateType::INID) {
      if (!std::isalpha(ch)) {
        if (std::isdigit(ch)) {
          state = StateType::INERROR;
          throwSyntaxError(TokenType::ID);
        }
        state = StateType::DONE;
        if (position <= programLength) {
          position--;
        }
        tokenType = TokenType::ID;
        save = false;
      }
    } else if (state == StateType::INEQ) {
      if (ch == '=') {
        state = StateType::DONE;
        tokenType = TokenType::EQ;
      } else if (ch == ' ') {
        state = StateType::DONE;
        if (position <= programLength) {
          position--;
        }
        tokenType = TokenType::ASSIGN;
        save = false;
      }
    } else if (state == StateType::INEXC) {
      if (ch == '=') {
        state = StateType::DONE;
        if (position <= programLength) {
          position--;
        }
        tokenType = TokenType::NOT_EQ;
        save = false;
      }
    } else if (state == StateType::INSLASH) {
      if (ch == '*') {
        save = false;
        state = StateType::INCOMMENT;
      } else {
        save = true;
        state = StateType::DONE;
        if (position <= programLength) {
          position--;
        }
        tokenType = TokenType::DIV;
      }
    } else if (state == StateType::INCOMMENT) {
      save = false;
      if (position == programLength) {
        state = StateType::DONE;
        tokenType = TokenType::ENDFILE;
      } else if (ch == '*') {
        state = StateType::INASTERISK;
      } else if (ch == '\n') {
        lineno++;
        lineStart += position + 1;
      }
    } else if (state == StateType::INASTERISK) {
      save = false;
      if (ch == '/') {
        save = false;
        tokenString = "";
        state = StateType::START;
      } else {
        state = StateType::INCOMMENT;
      }
    } else if (state == StateType::INERROR) {
      save = false;
      if (!std::isalnum(ch)) {
        state = StateType::START;
        if (position <= programLength) {
          position--;
        }
        tokenType = TokenType::ERROR;
      }
    }

    if (save) {
      tokenString += ch;
    }

    if (state == StateType::DONE) {
      if (tokenType == TokenType::ID) {
        tokenType = reservedLookup(tokenString);
      }
    }

    position++;
  }

  if (imprime) {
    std::cout << std::left << std::setw(6) << lineno << std::setw(20)
              << tokenTypeToString(tokenType) << std::setw(20) << tokenString
              << std::endl;
  }

  return std::make_tuple(tokenType, tokenString, lineno);
}
