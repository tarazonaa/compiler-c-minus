#pragma once

#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "lexer.hpp"

enum NodeKind { Statement, Expression, Program, Var, Declaration };
enum StatementKind { Exp, Comp, If, While, Return };
enum ExpressionKind { Assign, Simple };
enum ExpressionType { Void, Integer, Boolean };
enum DeclarationKind { VarD, FunD };

class TreeNode {
 protected:
  int lineno;
  int linestart;
  NodeKind nodekind;

 public:
  TreeNode(NodeKind k) : nodekind(k) {};
  virtual void print(int depth) = 0;
  virtual ~TreeNode() = default;
};

class ExpressionNode : public TreeNode {
 public:
  ExpressionKind expressionKind;
  ExpressionType expressionType;

  void print(int depth);
  ExpressionNode(ExpressionKind eKind)
      : expressionKind(eKind), TreeNode(NodeKind::Expression) {};
};

class ParamNode : public TreeNode {
 public:
  TokenType type;
  std::string id;

  void print(int depth);
  ParamNode(TokenType t, const std::string& i)
      : TreeNode(NodeKind::Var), type(t), id(i) {}
};

class VarNode : public TreeNode {
 public:
  std::unique_ptr<ExpressionNode> expression;
  std::string id;

  void print(int depth);
  VarNode(const std::string& name) : id(name), TreeNode(NodeKind::Var) {};
};

class StatementNode : public TreeNode {
 public:
  StatementKind statementKind;

  void print(int depth);
  StatementNode(StatementKind sKind)
      : statementKind(sKind), TreeNode(NodeKind::Statement) {};
  virtual ~StatementNode() = default;
};

class ExpressionStatementNode : public StatementNode {
 public:
  std::unique_ptr<ExpressionNode> expression;
  void print(int depth);
  ExpressionStatementNode() : StatementNode(StatementKind::Exp) {};
};

class IterationStatementNode : public StatementNode {
 public:
  std::unique_ptr<ExpressionNode> expression;
  std::unique_ptr<StatementNode> statement;
  void print(int depth);
  IterationStatementNode() : StatementNode(StatementKind::While) {};
};

class SelectionStatementNode : public StatementNode {
 public:
  std::unique_ptr<ExpressionNode> condition;
  std::unique_ptr<StatementNode> statement;
  std::unique_ptr<StatementNode> elseStatement;
  void print(int depth);
  SelectionStatementNode() : StatementNode(StatementKind::If) {};
};

class ReturnStatementNode : public StatementNode {
 public:
  std::unique_ptr<ExpressionNode> expression;
  void print(int depth);
  ReturnStatementNode() : StatementNode(StatementKind::Return) {};
};

class CallNode : public ExpressionNode {
 public:
  std::string id;
  std::vector<std::unique_ptr<ExpressionNode>> argsList;
  void print(int depth);

  CallNode(const std::string& id)
      : id(id), ExpressionNode(ExpressionKind::Simple) {};
};

class FactorNode : public ExpressionNode {
 public:
  int value;
  std::unique_ptr<ExpressionNode> expression;
  std::unique_ptr<VarNode> var;
  std::unique_ptr<CallNode> call;
  void print(int depth);
  FactorNode() : ExpressionNode(ExpressionKind::Simple) {};
};

class TermNode : public ExpressionNode {
 public:
  std::unique_ptr<FactorNode> leftFactor;
  TokenType mulop;
  std::unique_ptr<TermNode> rightFactor;
  void print(int depth);

  TermNode() : ExpressionNode(ExpressionKind::Simple) {};
};

class AdditiveExpressionNode : public ExpressionNode {
 public:
  std::unique_ptr<TermNode> leftTerm;
  TokenType addop;
  std::unique_ptr<AdditiveExpressionNode> rightTerm;
  void print(int depth);
  AdditiveExpressionNode() : ExpressionNode(ExpressionKind::Simple) {};
};

class SimpleExpressionNode : public ExpressionNode {
 public:
  std::unique_ptr<AdditiveExpressionNode> additiveLeft;
  TokenType relop;
  std::unique_ptr<AdditiveExpressionNode> additiveRight;
  void print(int depth);
  SimpleExpressionNode() : ExpressionNode(ExpressionKind::Simple) {};
};

class AssignmentExpressionNode : public ExpressionNode {
 public:
  std::unique_ptr<VarNode> var;
  std::unique_ptr<ExpressionNode> simpleExpression;
  void print(int depth);

  AssignmentExpressionNode() : ExpressionNode(ExpressionKind::Assign) {};
};

class DeclarationNode : public TreeNode {
 public:
  std::string id;
  std::string type;
  DeclarationKind declarationKind;
  void print(int depth);

  DeclarationNode(DeclarationKind dk, const std::string& i,
                  const std::string& t)
      : TreeNode(NodeKind::Declaration), declarationKind(dk), type(t), id(i) {}

  virtual ~DeclarationNode() = default;
};

class VarDeclarationNode : public DeclarationNode {
 public:
  std::optional<int> arraySize;
  void print(int depth);

  VarDeclarationNode(const std::string& i, const std::string& t)
      : DeclarationNode(DeclarationKind::VarD, i, t) {}
};

class CompoundStatementNode : public StatementNode {
 public:
  std::vector<std::unique_ptr<VarDeclarationNode>> vars;
  std::vector<std::unique_ptr<StatementNode>> statements;
  void print(int depth);

  CompoundStatementNode() : StatementNode(StatementKind::Comp) {}
};

class FunDeclarationNode : public DeclarationNode {
 public:
  std::vector<std::unique_ptr<ParamNode>> params;
  std::unique_ptr<CompoundStatementNode> compoundStatement;

  void print(int depth);
  FunDeclarationNode(const std::string& i, const std::string& t)
      : DeclarationNode(DeclarationKind::FunD, i, t) {}
};

class ProgramNode : public TreeNode {
 public:
  std::vector<std::unique_ptr<DeclarationNode>> declarationList;

  void print(int depth);
  ProgramNode() : TreeNode(NodeKind::Program) {};
};

class ParserSyntaxError : public std::exception {
 private:
  std::string message;

 public:
  ParserSyntaxError(const std::string& msg) : message(msg) {}

  const char* what() const noexcept override { return message.c_str(); }
};

class Parser {
 private:
  int lineno = 0;
  TokenType currToken;
  std::string currString;

  TreeNode* start;
  std::string program;
  int position = 0;
  int programLength = 0;
  std::string fileName;

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
  std::tuple<TreeNode*, std::optional<ParserSyntaxError>> parser(
      bool print = true);

  void print(int depth = 0);
  TreeNode* parseProgram();
};
