/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el header file del parser.
 * */
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "errors.hpp"
#include "lexer.hpp"

enum NodeKind { Statement, Expression, Program, Var, Declaration };
enum StatementKind { Exp, Comp, If, While, Return };
enum ExpressionKind { Assign, Simple };
enum ExpressionType { Void, Integer };
enum DeclarationKind { VarD, FunD };

// Declaramos el "árbol" como clase template.
template <typename Derived>
class TreeNode {
 protected:
  int lineno;
  int position;
  int linestart;
  NodeKind nodekind;

 public:
  TreeNode(NodeKind k, int line, int pos)
      : nodekind(k), lineno(line), position(pos) {};

  int getLineno() const;
  int getPosition() const;
  int getLineStart() const;
  virtual ~TreeNode() = default;
};

class ExpressionNode : public TreeNode<ExpressionNode> {
 public:
  ExpressionKind expressionKind;
  ExpressionType expressionType;

  void print(int depth);
  ExpressionNode(ExpressionKind eKind, int line, int pos)
      : expressionKind(eKind), TreeNode(NodeKind::Expression, line, pos) {};
};

class ParamNode : public TreeNode<ParamNode> {
 public:
  TokenType type;
  std::string id;

  void print(int depth);
  ParamNode(TokenType t, const std::string& i, int line, int pos)
      : TreeNode(NodeKind::Var, line, pos), type(t), id(i) {}
};

class VarNode : public TreeNode<VarNode> {
 public:
  std::unique_ptr<ExpressionNode> expression;
  std::string id;

  void print(int depth);
  VarNode(const std::string& name, int line, int pos)
      : id(name), TreeNode(NodeKind::Var, line, pos) {};
};

class StatementNode : public TreeNode<StatementNode> {
 public:
  StatementKind statementKind;

  void print(int depth);
  StatementNode(StatementKind sKind, int line, int pos)
      : statementKind(sKind), TreeNode(NodeKind::Statement, line, pos) {};
  virtual ~StatementNode() = default;
};

class ExpressionStatementNode : public StatementNode {
 public:
  std::unique_ptr<ExpressionNode> expression;
  void print(int depth);
  ExpressionStatementNode(int line, int pos)
      : StatementNode(StatementKind::Exp, line, pos) {};
};

class IterationStatementNode : public StatementNode {
 public:
  std::unique_ptr<ExpressionNode> expression;
  std::unique_ptr<StatementNode> statement;
  void print(int depth);
  IterationStatementNode(int line, int pos)
      : StatementNode(StatementKind::While, line, pos) {};
};

class SelectionStatementNode : public StatementNode {
 public:
  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<StatementNode> statement;
  std::unique_ptr<StatementNode> elseStatement;
  void print(int depth);
  SelectionStatementNode(int line, int pos)
      : StatementNode(StatementKind::If, line, pos) {};
};

class ReturnStatementNode : public StatementNode {
 public:
  std::unique_ptr<ExpressionNode> expression;
  void print(int depth);
  ReturnStatementNode(int line, int pos)
      : StatementNode(StatementKind::Return, line, pos) {};
};

class CallNode : public ExpressionNode {
 public:
  std::string id;
  std::vector<std::unique_ptr<ExpressionNode>> argsList;
  void print(int depth);

  CallNode(const std::string& id, int line, int pos)
      : id(id), ExpressionNode(ExpressionKind::Simple, line, pos) {};
};

class FactorNode : public ExpressionNode {
 public:
  int value;
  std::unique_ptr<ExpressionNode> expression;
  std::unique_ptr<VarNode> var;
  std::unique_ptr<CallNode> call;
  void print(int depth);
  FactorNode(int line, int pos)
      : ExpressionNode(ExpressionKind::Simple, line, pos) {};
};

class TermNode : public ExpressionNode {
 public:
  std::unique_ptr<FactorNode> leftFactor;
  TokenType mulop;
  std::unique_ptr<TermNode> rightFactor;
  void print(int depth);

  TermNode(int line, int pos)
      : ExpressionNode(ExpressionKind::Simple, line, pos) {};
};

class AdditiveExpressionNode : public ExpressionNode {
 public:
  std::unique_ptr<TermNode> leftTerm;
  TokenType addop;
  std::unique_ptr<AdditiveExpressionNode> rightTerm;
  void print(int depth);
  AdditiveExpressionNode(int line, int pos)
      : ExpressionNode(ExpressionKind::Simple, line, pos) {};
};

class SimpleExpressionNode : public ExpressionNode {
 public:
  std::unique_ptr<AdditiveExpressionNode> additiveLeft;
  TokenType relop;
  std::unique_ptr<AdditiveExpressionNode> additiveRight;
  void print(int depth);
  SimpleExpressionNode(int line, int pos)
      : ExpressionNode(ExpressionKind::Simple, line, pos) {};
};

class AssignmentExpressionNode : public ExpressionNode {
 public:
  std::unique_ptr<VarNode> var;
  std::unique_ptr<ExpressionNode> simpleExpression;
  void print(int depth);

  AssignmentExpressionNode(int line, int pos)
      : ExpressionNode(ExpressionKind::Assign, line, pos) {};
};

class DeclarationNode : public TreeNode<DeclarationNode> {
 public:
  std::string id;
  std::string type;
  DeclarationKind declarationKind;
  void print(int depth);

  DeclarationNode(DeclarationKind dk, const std::string& i,
                  const std::string& t, int line, int pos)
      : TreeNode(NodeKind::Declaration, line, pos),
        declarationKind(dk),
        type(t),
        id(i) {}

  virtual ~DeclarationNode() = default;
};

class VarDeclarationNode : public DeclarationNode {
 public:
  std::optional<int> arraySize;
  void print(int depth);

  VarDeclarationNode(const std::string& t, const std::string& i, int line,
                     int pos)
      : DeclarationNode(DeclarationKind::VarD, i, t, line, pos) {}
};

class CompoundStatementNode : public StatementNode {
 public:
  std::vector<std::unique_ptr<VarDeclarationNode>> vars;
  std::vector<std::unique_ptr<StatementNode>> statements;
  void print(int depth);

  CompoundStatementNode(int line, int pos)
      : StatementNode(StatementKind::Comp, line, pos) {}
};

class FunDeclarationNode : public DeclarationNode {
 public:
  std::vector<std::unique_ptr<ParamNode>> params;
  std::unique_ptr<CompoundStatementNode> compoundStatement;

  void print(int depth);
  FunDeclarationNode(const std::string& t, const std::string& i, int line,
                     int pos)
      : DeclarationNode(DeclarationKind::FunD, i, t, line, pos) {}
};

class ProgramNode : public TreeNode<ProgramNode> {
 public:
  std::vector<std::unique_ptr<DeclarationNode>> declarationList;

  void print(int depth);
  ProgramNode(int line, int pos) : TreeNode(NodeKind::Program, line, pos) {};
};

class Parser {
 private:
  int lineno = 0;
  TokenType currToken;
  std::string currString;

  std::unique_ptr<ProgramNode> start;
  std::string program;
  int position = 0;
  int programLength = 0;
  std::string fileName;

  std::string mostRecentId;

  Lexer lexer;

  // Se definen todas las funciones de parseo que se necesitan
  DeclarationNode* parseDeclaration();
  VarDeclarationNode* parseVarDeclaration(std::string& type, std::string& id);
  FunDeclarationNode* parseFunDeclaration(std::string& type, std::string& id);
  void parseTypeSpecifier();
  std::vector<std::unique_ptr<ParamNode>> parseParams();
  ParamNode* parseParam();
  CompoundStatementNode* parseCompoundStatement();
  VarNode* parseVar(const std::string& name);
  StatementNode* parseStatement();
  ExpressionStatementNode* parseExpressionStatement();
  SelectionStatementNode* parseSelectionStatement();
  IterationStatementNode* parseIterationStatement();
  ReturnStatementNode* parseReturnStatement();
  ExpressionNode* parseExpression();
  AssignmentExpressionNode* parseAssignmentExpression();
  ExpressionNode* parseSimpleExpression();
  AdditiveExpressionNode* parseAdditiveExpression();
  TermNode* parseTerm();
  FactorNode* parseFactor();
  CallNode* parseCall(const std::string& id);
  std::vector<std::unique_ptr<ExpressionNode>> parseArgs();
  TokenType parseRelop();
  TokenType parseAddop();
  TokenType parseMulop();

  std::unique_ptr<VarNode> tryParseVar();
  // Función match para asegurarnos que el token sea el esperado
  void match(TokenType expected);

 public:
  Parser(const std::string& filename, const std::string& prog, int pos,
         int progLong);
  std::tuple<std::unique_ptr<ProgramNode>, std::optional<ParserSyntaxError>>
  parser(bool print = true);

  void print(int depth = 0);
  std::unique_ptr<ProgramNode> parseProgram();
};
