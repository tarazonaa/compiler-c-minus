#include "parser.hpp"
class CodeGenerator {
  ProgramNode* tree;

 public:
  CodeGenerator(ProgramNode* ast) : tree(ast) {};
  void generateCode();
};
