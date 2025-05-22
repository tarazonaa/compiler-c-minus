/*
 *  Este archivo es para la clase "visitor".
 *  Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */

#include "visitor.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <ostream>

#include "errors.hpp"
#include "parser.hpp"
#include "semantic.hpp"

Types typeCaster(const std::string& type) {
  if (type == "int") {
    return Types::INT;
  } else {
    return Types::VOID;
  }
}

void SymbolTableVisitor::visitImpl(ProgramNode* node) {
  for (auto& child : node->declarationList) {
    visit(child);
  }
}

void SymbolTableVisitor::visitImpl(VarDeclarationNode* node) {
  if (symbolTable.find(node->id)) {
    return;
  }
  if (node->arraySize) {
    symbolTable.insertNode(node->id, node->getLineno(), typeCaster(node->type),
                           *node->arraySize);
  } else {
    symbolTable.insertNode(node->id, node->getLineno(), typeCaster(node->type),
                           0);
  }
}

std::string typesToString2(Types type) {
  if (type == Types::INT) {
    return "int";
  }
  return "void";
}

void SymbolTableVisitor::visitImpl(FunDeclarationNode* node) {
  Scope* newScope = symbolTable.createScope(node->id);
  symbolTable.currScope->addChild(newScope);

  Scope* oldScope = symbolTable.currScope;
  symbolTable.currScope = newScope;
  if (node->params.size() > 0) {
    for (auto& child : node->params) {
      visit(child);
    }
  }
  visit(node->compoundStatement);

  symbolTable.currScope = oldScope;
}

void SymbolTableVisitor::visitImpl(CompoundStatementNode* node) {
  if (node->vars.size() > 0) {
    for (auto& var : node->vars) {
      visit(var);
    }
  }

  if (node->statements.size() > 0) {
    for (auto& statement : node->statements) {
      visit(statement);
    }
  }
}

void SymbolTableVisitor::visitImpl(ReturnStatementNode* node) {
  visit(node->expression);
}

void SymbolTableVisitor::visitImpl(ExpressionStatementNode* node) {
  visit(node->expression);
}

void SymbolTableVisitor::visitImpl(IterationStatementNode* node) {
  visit(node->expression);
  visit(node->statement);
}

void SymbolTableVisitor::visitImpl(SelectionStatementNode* node) {
  visit(node->condition);
  visit(node->statement);
  if (node->elseStatement) {
    visit(node->elseStatement);
  }
}

void SymbolTableVisitor::visitImpl(ExpressionNode* node) {
  if (auto expr = dynamic_cast<CallNode*>(node)) {
    visitImpl(expr);
  } else if (auto fac = dynamic_cast<FactorNode*>(node)) {
    visitImpl(fac);
  } else if (auto term = dynamic_cast<TermNode*>(node)) {
    visitImpl(term);
  } else if (auto add = dynamic_cast<AdditiveExpressionNode*>(node)) {
    visitImpl(add);
  } else if (auto simple = dynamic_cast<SimpleExpressionNode*>(node)) {
    visitImpl(simple);
  } else if (auto assign = dynamic_cast<AssignmentExpressionNode*>(node)) {
    visitImpl(assign);
  } else {
    std::cerr << "Unknown StatementNode type\n";
  }
}

void SymbolTableVisitor::visitImpl(StatementNode* node) {
  if (auto expr = dynamic_cast<ExpressionStatementNode*>(node)) {
    visitImpl(expr);
  } else if (auto comp = dynamic_cast<CompoundStatementNode*>(node)) {
    visitImpl(comp);
  } else if (auto sel = dynamic_cast<SelectionStatementNode*>(node)) {
    visitImpl(sel);
  } else if (auto iter = dynamic_cast<IterationStatementNode*>(node)) {
    visitImpl(iter);
  } else if (auto ret = dynamic_cast<ReturnStatementNode*>(node)) {
    visitImpl(ret);
  } else {
    std::cerr << "Unknown StatementNode type\n";
  }
}

void SymbolTableVisitor::visitImpl(SimpleExpressionNode* node) {
  visit(node->additiveLeft);
  if (node->additiveRight) {
    visit(node->additiveRight);
  }
}

void SymbolTableVisitor::visitImpl(AdditiveExpressionNode* node) {
  visit(node->leftTerm);
  if (node->rightTerm) {
    visit(node->rightTerm);
  }
}

void SymbolTableVisitor::visitImpl(AssignmentExpressionNode* node) {
  visit(node->var);
  visit(node->simpleExpression);
}

void SymbolTableVisitor::visitImpl(TermNode* node) {
  visit(node->leftFactor);
  if (node->rightFactor) {
    visit(node->rightFactor);
  }
}

void SymbolTableVisitor::visitImpl(FactorNode* node) {
  if (node->expression) {
    visit(node->expression);
  }
  if (node->var) {
    visit(node->var);
  }
  if (node->call) {
    visit(node->call);
  }
}

void SymbolTableVisitor::visitImpl(CallNode* node) {
  symbolTable.currScope->symbolTable.addUsage(node->id, node->getLineno());
  if (node->argsList.size() > 0) {
    for (auto& arg : node->argsList) {
      visit(arg);
    }
  }
}

void SymbolTableVisitor::visitImpl(VarNode* node) {
  symbolTable.currScope->symbolTable.addUsage(node->id, node->getLineno());
}

void SymbolTableVisitor::visitImpl(ParamNode* node) {
  symbolTable.currScope->symbolTable.addUsage(node->id, node->getLineno());
}

void SymbolTableVisitor::visitImpl(DeclarationNode* node) {
  if (auto fun = dynamic_cast<FunDeclarationNode*>(node)) {
    visitImpl(fun);
  } else if (auto var = dynamic_cast<VarDeclarationNode*>(node)) {
    visitImpl(var);
  }
}

Types expressionTypeToSemantic(ExpressionType type) {
  if (type == ExpressionType::Integer) {
    return Types::INT;
  } else {
    return Types::VOID;
  }
}

void TypeCheckerVisitor::visitImpl(ProgramNode* node) {
  for (auto& declaration : node->declarationList) {
    visit(declaration);
  }
}

void TypeCheckerVisitor::visitImpl(VarDeclarationNode* node) {
  Types expectedType = typeCaster(node->type);
  Types actualType = symbolTable.getType(node->id);

  if (actualType != expectedType) {
    std::cerr << "Type error: Variable " << node->id
              << " has an incompatible type.\n"
              << typesToString2(actualType) << typesToString2(expectedType);
  }
}

void TypeCheckerVisitor::visitImpl(FunDeclarationNode* node) {
  Types expectedReturnType = typeCaster(node->type);

  for (auto& param : node->params) {
    visit(param);
  }

  visit(node->compoundStatement);
}

void TypeCheckerVisitor::visitImpl(CompoundStatementNode* node) {
  for (auto& var : node->vars) {
    visit(var);
  }

  for (auto& stmt : node->statements) {
    visit(stmt);
  }
}

void TypeCheckerVisitor::visitImpl(ReturnStatementNode* node) {
  visit(node->expression);

  Types returnExprType =
      expressionTypeToSemantic(node->expression->expressionType);
  Types expectedReturnType = typeCaster(
      expressionTypeToSemantic(node->expression->expressionType) == Types::INT
          ? "int"
          : "void");

  if (returnExprType != expectedReturnType) {
    throw new SemanticError();
  }
}

void TypeCheckerVisitor::visitImpl(ExpressionNode* node) {
  if (auto expr = dynamic_cast<CallNode*>(node)) {
    visitImpl(expr);
  } else if (auto fac = dynamic_cast<FactorNode*>(node)) {
    visitImpl(fac);
  } else if (auto term = dynamic_cast<TermNode*>(node)) {
    visitImpl(term);
  } else if (auto add = dynamic_cast<AdditiveExpressionNode*>(node)) {
    visitImpl(add);
  } else if (auto simple = dynamic_cast<SimpleExpressionNode*>(node)) {
    visitImpl(simple);
  } else if (auto assign = dynamic_cast<AssignmentExpressionNode*>(node)) {
    visitImpl(assign);
  } else {
    std::cerr << "Unknown ExpressionNode type\n";
  }
}

void TypeCheckerVisitor::visitImpl(AssignmentExpressionNode* node) {
  visit(node->var);
  visit(node->simpleExpression);

  Types varType = symbolTable.getType(node->var->id);
  Types exprType =
      expressionTypeToSemantic(node->simpleExpression->expressionType);

  if (varType != exprType) {
  }
}

void TypeCheckerVisitor::visitImpl(TermNode* node) {
  visit(node->leftFactor);
  if (node->rightFactor) {
    visit(node->rightFactor);
  }
}

void TypeCheckerVisitor::visitImpl(FactorNode* node) {
  if (node->expression) {
    visit(node->expression);
  }
  if (node->var) {
    visit(node->var);
  }
  if (node->call) {
    visit(node->call);
  }
}

void TypeCheckerVisitor::visitImpl(CallNode* node) {
  if (!symbolTable.find(node->id) &&
      (node->id != "output" && node->id != "input")) {
  }

  for (auto& arg : node->argsList) {
    visit(arg);
  }
}

void TypeCheckerVisitor::visitImpl(VarNode* node) {}

void TypeCheckerVisitor::visitImpl(ParamNode* node) {}

void TypeCheckerVisitor::visitImpl(StatementNode* node) {
  if (auto exprStmt = dynamic_cast<ExpressionStatementNode*>(node)) {
    visitImpl(exprStmt);
  } else if (auto compStmt = dynamic_cast<CompoundStatementNode*>(node)) {
    visitImpl(compStmt);
  } else if (auto selStmt = dynamic_cast<SelectionStatementNode*>(node)) {
    visitImpl(selStmt);
  } else if (auto iterStmt = dynamic_cast<IterationStatementNode*>(node)) {
    visitImpl(iterStmt);
  } else if (auto retStmt = dynamic_cast<ReturnStatementNode*>(node)) {
    visitImpl(retStmt);
  } else {
    std::cerr << "Unknown StatementNode type\n";
  }
}

void TypeCheckerVisitor::visitImpl(DeclarationNode* node) {
  if (auto varDecl = dynamic_cast<VarDeclarationNode*>(node)) {
    visitImpl(varDecl);
  } else if (auto funDecl = dynamic_cast<FunDeclarationNode*>(node)) {
    visitImpl(funDecl);
  } else {
    std::cerr << "Unknown DeclarationNode type\n";
  }
}

void TypeCheckerVisitor::visitImpl(SimpleExpressionNode* node) {
  visit(node->additiveLeft);
  if (node->additiveRight) {
    visit(node->additiveRight);
  }

  Types leftType = expressionTypeToSemantic(node->additiveLeft->expressionType);
  Types rightType =
      node->additiveRight
          ? expressionTypeToSemantic(node->additiveRight->expressionType)
          : Types::VOID;

  if (leftType != rightType) {
    std::cerr << "Type error: Incompatible types in simple expression.\n";
  }
}

void TypeCheckerVisitor::visitImpl(AdditiveExpressionNode* node) {
  visit(node->leftTerm);
  if (node->rightTerm) {
    visit(node->rightTerm);
  }

  Types leftType = expressionTypeToSemantic(node->leftTerm->expressionType);
  Types rightType =
      node->rightTerm
          ? expressionTypeToSemantic(node->rightTerm->expressionType)
          : Types::VOID;

  if (leftType != rightType) {
    std::cerr << "Type error: Incompatible types in additive expression.\n";
  }
}

void TypeCheckerVisitor::visitImpl(IterationStatementNode* node) {
  visit(node->expression);
  Types conditionType =
      expressionTypeToSemantic(node->expression->expressionType);

  if (conditionType != Types::INT) {
    std::cerr << "Type error: Condition expression in while statement must be "
                 "of type Integer.\n";
  }

  visit(node->statement);
}

void TypeCheckerVisitor::visitImpl(SelectionStatementNode* node) {
  visit(node->condition);
  Types conditionType =
      expressionTypeToSemantic(node->condition->expressionType);

  visit(node->statement);

  if (node->elseStatement) {
    visit(node->elseStatement);
  }
}

void TypeCheckerVisitor::visitImpl(ExpressionStatementNode* node) {
  visit(node->expression);
  Types exprType = expressionTypeToSemantic(node->expression->expressionType);
}
