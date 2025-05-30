/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el header file del generador de código
 * */
#pragma once
#include <fstream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "parser.hpp"
#include "semantic.hpp"
class CodeGenerator {
  Semantic& semantic;
  std::ofstream fileToWrite;
  bool isInGlobals;
  std::unordered_set<std::string> emittedGlobals;
  std::unordered_map<std::string, int> offsets;
  std::unordered_map<std::string, int> localOffsets;
  int currentStackOffset = 0;

 public:
  CodeGenerator(Semantic& semantic);

  void generate();
  void setup();
  void writeToFile(std::string& content);
  template <typename Node>
  void generateForNode(Node* tree);

  // Visitors como en el visitor.hpp
  void visitImpl(ProgramNode* node);
  void visitImpl(ExpressionNode* node);
  void visitImpl(TermNode* node);
  void visitImpl(AdditiveExpressionNode* node);
  void visitImpl(ParamNode* node);
  void visitImpl(SelectionStatementNode* node);
  void visitImpl(StatementNode* node);
  void visitImpl(CompoundStatementNode* node);
  void visitImpl(ExpressionStatementNode* node);
  void visitImpl(IterationStatementNode* node);
  void visitImpl(ReturnStatementNode* node);
  void visitImpl(SimpleExpressionNode* node);
  void visitImpl(DeclarationNode* node);
  void visitImpl(AssignmentExpressionNode* node);
  void visitImpl(FactorNode* node);
  void visitImpl(VarDeclarationNode* node);
  void visitImpl(FunDeclarationNode* node);
  void visitImpl(VarNode* node);
  void visitImpl(CallNode* node);
};
