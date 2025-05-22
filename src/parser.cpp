/*
 * Este archivo es para la implementación del parser
 * Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */
#include "parser.hpp"

#include <iostream>
#include <memory>
#include <optional>
#include <vector>

template <typename Derived>
int TreeNode<Derived>::getLineno() const {
  return lineno;
}

Parser::Parser(const std::string& filename, const std::string& prog, int pos,
               int progLong)
    : fileName(filename),
      program(prog),
      programLength(progLong),
      position(pos),
      lexer(filename) {
  lexer.globales(program, position, programLength);
};

std::unique_ptr<ProgramNode> Parser::parseProgram() {
  auto node = std::make_unique<ProgramNode>(lineno, position);

  while (currToken != TokenType::ENDFILE) {
    node->declarationList.push_back(
        std::unique_ptr<DeclarationNode>(parseDeclaration()));
  }

  return node;
}

CompoundStatementNode* Parser::parseCompoundStatement() {
  auto node = std::make_unique<CompoundStatementNode>(lineno, position);
  std::vector<std::unique_ptr<VarDeclarationNode>> varsList;
  std::vector<std::unique_ptr<StatementNode>> statementList;

  match(TokenType::O_BRACE);
  while (currToken == TokenType::INT || currToken == TokenType::VOID) {
    std::string type = currString;
    match(currToken);

    std::string name = currString;
    match(TokenType::ID);

    varsList.push_back(
        std::unique_ptr<VarDeclarationNode>(parseVarDeclaration(type, name)));
  }

  while (currToken != TokenType::C_BRACE) {
    statementList.push_back(std::unique_ptr<StatementNode>(parseStatement()));
  }
  match(TokenType::C_BRACE);

  node->vars = std::move(varsList);
  node->statements = std::move(statementList);

  return node.release();
}

VarNode* Parser::parseVar(const std::string& name) {
  auto node = std::make_unique<VarNode>(name, lineno, position);
  if (currToken == TokenType::O_BRACKET) {
    match(TokenType::O_BRACKET);
    node->expression = std::unique_ptr<ExpressionNode>(parseExpression());
    match(TokenType::C_BRACKET);
  }

  return node.release();
}

StatementNode* Parser::parseStatement() {
  if (currToken == TokenType::IF) {
    return parseSelectionStatement();
  } else if (currToken == TokenType::RETURN) {
    return parseReturnStatement();
  } else if (currToken == TokenType::WHILE) {
    return parseIterationStatement();
  } else if (currToken == TokenType::O_BRACE) {
    return parseCompoundStatement();
  } else {
    return parseExpressionStatement();
  }
}

SelectionStatementNode* Parser::parseSelectionStatement() {
  auto node = std::make_unique<SelectionStatementNode>(lineno, position);

  match(TokenType::IF);
  match(TokenType::O_PAREN);
  node->condition = std::unique_ptr<ExpressionNode>(parseExpression());
  match(TokenType::C_PAREN);

  node->statement = std::unique_ptr<StatementNode>(parseStatement());

  if (currToken == TokenType::ELSE) {
    match(TokenType::ELSE);
    node->elseStatement = std::unique_ptr<StatementNode>(parseStatement());
  }

  return node.release();
}

ReturnStatementNode* Parser::parseReturnStatement() {
  auto node = std::make_unique<ReturnStatementNode>(lineno, position);
  match(TokenType::RETURN);
  if (currToken != TokenType::COMMA) {
    node->expression = std::unique_ptr<ExpressionNode>(parseExpression());
  }
  match(TokenType::SEMI);

  return node.release();
}

IterationStatementNode* Parser::parseIterationStatement() {
  auto node = std::make_unique<IterationStatementNode>(lineno, position);
  match(TokenType::WHILE);
  match(TokenType::O_PAREN);
  node->expression = std::unique_ptr<ExpressionNode>(parseExpression());
  match(TokenType::C_PAREN);
  node->statement = std::unique_ptr<StatementNode>(parseStatement());
  return node.release();
}

ExpressionStatementNode* Parser::parseExpressionStatement() {
  auto node = std::make_unique<ExpressionStatementNode>(lineno, position);
  if (currToken != TokenType::SEMI) {
    node->expression = std::unique_ptr<ExpressionNode>(parseExpression());
  }
  match(TokenType::SEMI);
  return node.release();
}

ExpressionNode* Parser::parseExpression() { return parseSimpleExpression(); }

AssignmentExpressionNode* Parser::parseAssignmentExpression() {
  auto node = std::make_unique<AssignmentExpressionNode>(lineno, position);
  node->var = std::unique_ptr<VarNode>(parseVar(mostRecentId));
  match(TokenType::ASSIGN);
  node->simpleExpression =
      std::unique_ptr<ExpressionNode>(parseSimpleExpression());
  return node.release();
}

ExpressionNode* Parser::parseSimpleExpression() {
  auto node = std::make_unique<SimpleExpressionNode>(lineno, position);
  node->additiveLeft =
      std::unique_ptr<AdditiveExpressionNode>(parseAdditiveExpression());
  if (currToken == TokenType::ASSIGN) {
    return parseAssignmentExpression();
  }
  if (currToken == TokenType::LT || currToken == TokenType::LTE ||
      currToken == TokenType::GT || currToken == TokenType::GTE ||
      currToken == TokenType::EQ || currToken == TokenType::NOT_EQ) {
    node->relop = currToken;
    match(currToken);
    node->additiveRight =
        std::unique_ptr<AdditiveExpressionNode>(parseAdditiveExpression());
  }
  return node.release();
}

AdditiveExpressionNode* Parser::parseAdditiveExpression() {
  auto node = std::make_unique<AdditiveExpressionNode>(lineno, position);
  node->leftTerm = std::unique_ptr<TermNode>(parseTerm());
  if (currToken == TokenType::ADD || currToken == TokenType::SUB) {
    node->addop = currToken;
    match(currToken);
    node->rightTerm =
        std::unique_ptr<AdditiveExpressionNode>(parseAdditiveExpression());
  }
  return node.release();
}

TermNode* Parser::parseTerm() {
  auto node = std::make_unique<TermNode>(lineno, position);
  node->leftFactor = std::unique_ptr<FactorNode>(parseFactor());
  if (currToken == TokenType::DIV || currToken == TokenType::TIMES) {
    node->mulop = currToken;
    match(currToken);
    node->rightFactor = std::unique_ptr<TermNode>(parseTerm());
  }
  return node.release();
}

FactorNode* Parser::parseFactor() {
  auto node = std::make_unique<FactorNode>(lineno, position);
  if (currToken == TokenType::O_PAREN) {
    match(TokenType::O_PAREN);
    node->expression = std::unique_ptr<ExpressionNode>(parseExpression());
    match(TokenType::C_PAREN);

  } else if (currToken == TokenType::ID) {
    std::string id = currString;
    match(TokenType::ID);

    if (currToken == TokenType::O_PAREN) {
      match(TokenType::O_PAREN);
      node->call = std::unique_ptr<CallNode>(parseCall(id));
      match(TokenType::C_PAREN);
    } else {
      node->var = std::unique_ptr<VarNode>(parseVar(id));
    }

  } else if (currToken == TokenType::NUM) {
    node->value = std::stoi(currString);
    match(TokenType::NUM);
  }
  return node.release();
}

CallNode* Parser::parseCall(const std::string& id) {
  auto node = std::make_unique<CallNode>(id, lineno, position);

  node->argsList = parseArgs();

  return node.release();
}

std::vector<std::unique_ptr<ExpressionNode>> Parser::parseArgs() {
  std::vector<std::unique_ptr<ExpressionNode>> args;

  if (currToken != TokenType::C_PAREN) {
    args.push_back(std::unique_ptr<ExpressionNode>(parseExpression()));

    while (currToken == TokenType::COMMA) {
      match(TokenType::COMMA);
      args.push_back(std::unique_ptr<ExpressionNode>(parseExpression()));
    }
  }

  return args;
}

FunDeclarationNode* Parser::parseFunDeclaration(std::string& type,
                                                std::string& id) {
  auto node = std::make_unique<FunDeclarationNode>(type, id, lineno, position);
  match(TokenType::O_PAREN);
  node->params = parseParams();
  match(TokenType::C_PAREN);

  node->compoundStatement =
      std::unique_ptr<CompoundStatementNode>(parseCompoundStatement());

  return node.release();
}

std::vector<std::unique_ptr<ParamNode>> Parser::parseParams() {
  std::vector<std::unique_ptr<ParamNode>> paramsList;
  if (currToken == TokenType::VOID) {
    match(TokenType::VOID);
    return paramsList;
  };

  paramsList.push_back(std::unique_ptr<ParamNode>(parseParam()));

  while (currToken == TokenType::COMMA) {
    match(TokenType::COMMA);
    paramsList.push_back(std::unique_ptr<ParamNode>(parseParam()));
  }

  return paramsList;
}

ParamNode* Parser::parseParam() {
  TokenType type;
  std::string name;
  if (currToken == TokenType::INT || currToken == TokenType::VOID) {
    type = currToken;
    match(currToken);
    name = currString;
    match(TokenType::ID);
  }

  auto node = std::make_unique<ParamNode>(type, name, lineno, position);
  return node.release();
}

VarDeclarationNode* Parser::parseVarDeclaration(std::string& type,
                                                std::string& id) {
  // Pasamos a parsear la declaración de una variable
  auto node = std::make_unique<VarDeclarationNode>(type, id, lineno, position);

  if (currToken == TokenType::O_BRACKET) {
    match(TokenType::O_BRACKET);
    int size = std::stoi(currString);
    match(TokenType::NUM);
    match(TokenType::C_BRACKET);
    node->arraySize = size;
  }

  match(TokenType::SEMI);

  return node.release();
}

DeclarationNode* Parser::parseDeclaration() {
  // Se parsea el primer token para que sea forzozamente void o int
  std::string type = currString;
  match(currToken);

  std::string id = currString;
  match(TokenType::ID);

  if (currToken == TokenType::O_PAREN) {
    return parseFunDeclaration(type, id);
  } else {
    return parseVarDeclaration(type, id);
  }
}

void Parser::match(TokenType expected) {
  if (currToken == expected) {
    auto [token, string, position] = lexer.getToken(false);
    this->currString = string;
    this->currToken = token;
    this->lineno = position;
  } else {
    throw ParserSyntaxError("Expected token " + tokenTypeToString(expected) +
                            ", but got " + tokenTypeToString(currToken) +
                            " on line " + std::to_string(lexer.getLineNo()));
  }
}

std::tuple<std::unique_ptr<ProgramNode>, std::optional<ParserSyntaxError>>
Parser::parser(bool imprime) {
  auto [token, string, position] = lexer.getToken(false);
  this->currString = string;
  this->currToken = token;
  this->position = position;
  this->start = parseProgram();

  return {std::move(start), std::nullopt};
}

void Parser::print(int depth) { start->print(depth + 1); }

inline void indent(int depth) { std::cout << std::string(depth, ' '); }

void ProgramNode::print(int depth) {
  indent(depth);
  std::cout << "ProgramNode" << std::endl;
  for (auto& decl : declarationList) {
    decl->print(depth + 2);
  }
}

void DeclarationNode::print(int depth) {
  indent(depth);
  std::cout << "DeclarationNode: " << id << " : " << type << std::endl;
}

void VarDeclarationNode::print(int depth) {
  indent(depth);
  std::cout << "VarDeclarationNode: " << id << " : " << type;
  if (arraySize) {
    std::cout << " [" << *arraySize << "]";
  }
  std::cout << std::endl;
}

void FunDeclarationNode::print(int depth) {
  indent(depth);
  std::cout << "FunDeclarationNode: " << id << " : " << type << std::endl;
  for (auto& param : params) {
    param->print(depth + 2);
  }
  if (compoundStatement) {
    compoundStatement->print(depth + 2);
  }
}

void StatementNode::print(int depth) {
  indent(depth);
  std::cout << "StatementNode" << std::endl;
}

void ExpressionStatementNode::print(int depth) {
  indent(depth);
  std::cout << "ExpressionStatementNode" << std::endl;
  if (expression) expression->print(depth + 2);
}

void IterationStatementNode::print(int depth) {
  indent(depth);
  std::cout << "IterationStatementNode (while)" << std::endl;
  if (expression) expression->print(depth + 2);
  if (statement) statement->print(depth + 2);
}

void SelectionStatementNode::print(int depth) {
  indent(depth);
  std::cout << "SelectionStatementNode (if)" << std::endl;
  if (condition) condition->print(depth + 2);
  if (statement) statement->print(depth + 2);
  if (elseStatement) elseStatement->print(depth + 2);
}

void ReturnStatementNode::print(int depth) {
  indent(depth);
  std::cout << "ReturnStatementNode" << std::endl;
  if (expression) expression->print(depth + 2);
}

void CompoundStatementNode::print(int depth) {
  indent(depth);
  std::cout << "CompoundStatementNode" << std::endl;
  for (auto& var : vars) {
    var->print(depth + 2);
  }
  for (auto& stmt : statements) {
    stmt->print(depth + 2);
  }
}

void ExpressionNode::print(int depth) {
  indent(depth);
  std::cout << "ExpressionNode" << std::endl;
}

void AssignmentExpressionNode::print(int depth) {
  indent(depth);
  std::cout << "AssignmentExpressionNode" << std::endl;
  if (var) var->print(depth + 2);
  if (simpleExpression) simpleExpression->print(depth + 2);
}

void SimpleExpressionNode::print(int depth) {
  indent(depth);
  std::cout << "SimpleExpressionNode" << std::endl;
  if (additiveLeft) additiveLeft->print(depth + 2);
  if (additiveRight) additiveRight->print(depth + 2);
}

void AdditiveExpressionNode::print(int depth) {
  indent(depth);
  std::cout << "AdditiveExpressionNode" << std::endl;
  if (leftTerm) leftTerm->print(depth + 2);
  if (rightTerm) rightTerm->print(depth + 2);
}

void TermNode::print(int depth) {
  indent(depth);
  std::cout << "TermNode" << std::endl;
  if (leftFactor) leftFactor->print(depth + 2);
  if (rightFactor) rightFactor->print(depth + 2);
}

void FactorNode::print(int depth) {
  indent(depth);
  std::cout << "FactorNode" << std::endl;
  if (var) var->print(depth + 2);
  if (call) call->print(depth + 2);
  if (expression)
    expression->print(depth + 2);
  else {
    indent(depth + 2);
    std::cout << "Value: " << value << std::endl;
  }
}

void CallNode::print(int depth) {
  indent(depth);
  std::cout << "CallNode: " << id << std::endl;
  for (auto& arg : argsList) {
    arg->print(depth + 2);
  }
}

void VarNode::print(int depth) {
  indent(depth);
  std::cout << "VarNode: " << id << std::endl;
  if (expression) expression->print(depth + 2);
}

void ParamNode::print(int depth) {
  indent(depth);
  std::cout << "ParamNode: " << id << " : " << tokenTypeToString(type)
            << std::endl;
}
