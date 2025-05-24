/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el file de errores.
 * */
#include "errors.hpp"

#include "colors.hpp"

int LexerSyntaxError::getLineNo() const { return lineno; }
int LexerSyntaxError::getPositionInLine() const { return positionInLine; }
const std::string &LexerSyntaxError::getLine() const { return line; }
const std::string &LexerSyntaxError::getFileName() const { return fileName; }

std::string LexerSyntaxError::format() const {
  return "Error in file '" + fileName + "' at line " + std::to_string(lineno) +
         ", position " + std::to_string(positionInLine) + ":\n" + line + "\n" +
         std::string(positionInLine, ' ') + "^ " + what();
}

std::string SemanticError::format() const {
  std::string pointer =
      std::string(positionInLine, ' ') + Style::bold_red("^") + "\n|";

  std::string header =
      Style::bold_red("[Error]\n") + Style::bold(std::string(what())) + "\n|";

  std::string location = Style::bold_yellow(" --> ") + fileName + ":" +
                         std::to_string(lineno) + ":" +
                         std::to_string(positionInLine) + "\n|";

  std::string codeLine = line + "\n|\n|";

  return header + location + codeLine + pointer;
}
