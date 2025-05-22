#include "errors.hpp"

int LexerSyntaxError::getLineNo() const { return lineno; }
int LexerSyntaxError::getPositionInLine() const { return positionInLine; }
const std::string &LexerSyntaxError::getLine() const { return line; }
const std::string &LexerSyntaxError::getFileName() const { return fileName; }

std::string LexerSyntaxError::format() const {
  return "Error in file '" + fileName + "' at line " + std::to_string(lineno) +
         ", position " + std::to_string(positionInLine) + ":\n" + line + "\n" +
         std::string(positionInLine, ' ') + "^ " + what();
}
