/*
 *  Este archivo es para la implementación de la clase semantica
 *  Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */
#include "semantic.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

void SymbolTable::print() const {
  std::cout << "+----------------------+---------------------+\n";
  std::cout << "| Symbol               | Line Numbers        |\n";
  std::cout << "+----------------------+---------------------+\n";

  for (const auto& [name, lines] : symbolTable) {
    std::cout << "| " << std::left << std::setw(20) << name << " | ";

    std::stringstream lineNumbers;
    for (size_t i = 0; i < lines.size(); ++i) {
      if (i > 0) lineNumbers << ", ";
      lineNumbers << lines[i];
    }

    std::cout << std::left << std::setw(18) << lineNumbers.str() << " |\n";
  }
  std::cout << "+----------------------+---------------------+\n";
}

void SymbolTable::insertNode(const std::string& name, int lineno) {
  symbolTable[name].push_back(lineno);
}

bool SymbolTable::find(const std::string& name) const {
  return symbolTable.find(name) != symbolTable.end();
}

void Semantic::buildSymbolTable() {
  traverse(tree, nullptr, [this](TreeNode* node) {
    if (auto* varDecl = dynamic_cast<VarDeclarationNode*>(node)) {
      symbolTable.insertNode(varDecl->id, varDecl->getLineno());
    } else if (auto* funDecl = dynamic_cast<FunDeclarationNode*>(node)) {
      symbolTable.insertNode(funDecl->id, funDecl->getLineno());
    } else if (auto* varUsage = dynamic_cast<VarNode*>(node)) {
      symbolTable.insertNode(varUsage->id, varUsage->getLineno());
    }
  });
}

void Semantic::typeCheck() {
  traverse(
      tree,
      [this](TreeNode* node) {
        if (auto* assign = dynamic_cast<AssignmentExpressionNode*>(node)) {
          if (!symbolTable.find(assign->var->id)) {
          }
        }
      },
      nullptr);
}

void Semantic::traverse(TreeNode* node, std::function<void(TreeNode*)> preOrder,
                        std::function<void(TreeNode*)> postOrder) {
  if (preOrder) preOrder(node);
  if (auto* program = dynamic_cast<ProgramNode*>(node)) {
    for (auto& child : program->declarationList) {
      traverse(child.get(), preOrder, postOrder);
    }
  } else if (auto* func = dynamic_cast<FunDeclarationNode*>(node)) {
    traverse(func->compoundStatement.get(), preOrder, postOrder);
  } else if (auto* compound = dynamic_cast<CompoundStatementNode*>(node)) {
    for (auto& child : compound->vars) {
      traverse(child.get(), preOrder, postOrder);
    }
    for (auto& child : compound->statements) {
      traverse(child.get(), preOrder, postOrder);
    }
  } else if (auto* exprStmt = dynamic_cast<ExpressionStatementNode*>(node)) {
    traverse(exprStmt->expression.get(), preOrder, postOrder);
  } else if (auto* iterStmt = dynamic_cast<IterationStatementNode*>(node)) {
    traverse(iterStmt->expression.get(), preOrder, postOrder);
    traverse(iterStmt->statement.get(), preOrder, postOrder);
  } else if (auto* selectStmt = dynamic_cast<SelectionStatementNode*>(node)) {
    traverse(selectStmt->condition.get(), preOrder, postOrder);
    traverse(selectStmt->statement.get(), preOrder, postOrder);
    traverse(selectStmt->elseStatement.get(), preOrder, postOrder);
  } else if (auto* returnStmt = dynamic_cast<ReturnStatementNode*>(node)) {
    traverse(returnStmt->expression.get(), preOrder, postOrder);
  } else if (auto* assignExpr = dynamic_cast<AssignmentExpressionNode*>(node)) {
    traverse(assignExpr->var.get(), preOrder, postOrder);
    traverse(assignExpr->simpleExpression.get(), preOrder, postOrder);
  } else if (auto* addExpr = dynamic_cast<AdditiveExpressionNode*>(node)) {
    traverse(addExpr->leftTerm.get(), preOrder, postOrder);
    traverse(addExpr->rightTerm.get(), preOrder, postOrder);
  } else if (auto* term = dynamic_cast<TermNode*>(node)) {
    traverse(term->leftFactor.get(), preOrder, postOrder);
    traverse(term->rightFactor.get(), preOrder, postOrder);
  } else if (auto* factor = dynamic_cast<FactorNode*>(node)) {
    traverse(factor->expression.get(), preOrder, postOrder);
    traverse(factor->var.get(), preOrder, postOrder);
    traverse(factor->call.get(), preOrder, postOrder);
  } else if (auto* call = dynamic_cast<CallNode*>(node)) {
    for (auto& arg : call->argsList) {
      traverse(arg.get(), preOrder, postOrder);
    }
  } else if (auto* var = dynamic_cast<VarNode*>(node)) {
    traverse(var->expression.get(), preOrder, postOrder);
  }
  if (postOrder) postOrder(node);
}

void Semantic::analyze() {
  buildSymbolTable();
  typeCheck();
  symbolTable.print();
}

Semantic::Semantic(TreeNode* tree) : tree(tree) {}
