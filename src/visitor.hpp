/*
 *  Este archivo es para la clase "visitor".
 *  Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */
#pragma once

#include <cxxabi.h>

#include <memory>

#include "parser.hpp"
#include "semantic.hpp"

template <typename DerivedVisitor>
class Visitor {
  Semantic& semantic;

 public:
  Visitor<DerivedVisitor>(Semantic& sem) : semantic(sem){};
  void updateSemanticAnalyzer(int pos, int lineno);

  int getSemanticLineno() { return semantic.getLineno(); }
  int getSemanticPosition() { return semantic.getPosition(); }
  int getSemanticLineStart() { return semantic.getLineStart(); }
  const std::string& getSemanticCurrLine() { return semantic.getCurrLine(); }
  const std::string& getSemanticFileName() { return semantic.getFileName(); }

  template <typename Node>
  void visit(std::unique_ptr<Node>& node) {
    semantic.setPosition(node->getPosition());
    semantic.setLineno(node->getLineno());
    semantic.setLineStart(node->getLineStart());
    static_cast<DerivedVisitor*>(this)->visitImpl(node.get());
  }
};

class SymbolTableVisitor : public Visitor<SymbolTableVisitor> {
  SymbolTable& symbolTable;

 public:
  explicit SymbolTableVisitor(SymbolTable& st, Semantic& sem)
      : symbolTable(st), Visitor<SymbolTableVisitor>(sem) {};

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

class TypeCheckerVisitor : public Visitor<TypeCheckerVisitor> {
  Types currentReturnType;
  SymbolTable& symbolTable;

 public:
  explicit TypeCheckerVisitor(SymbolTable& st, Semantic& sem)
      : symbolTable(st), Visitor<TypeCheckerVisitor>(sem) {};

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
