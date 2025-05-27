/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el implementation file del generador de código
 * */
#include "codegen.hpp"

#include <_time.h>

#include <fstream>
#include <memory>
#include <string>

#include "parser.hpp"
#include "semantic.hpp"

CodeGenerator::CodeGenerator(Semantic& semantic) : semantic(semantic) {};

void CodeGenerator::setup() {
  fileToWrite.open("main.mips");
  fileToWrite << ".data" << std::endl;
}

void CodeGenerator::generate() {
  setup();

  generateForNode(semantic.getTree());

  fileToWrite.close();
}

template <typename Node>
void CodeGenerator::generateForNode(std::unique_ptr<Node> tree) {
  visitImpl(tree.get());
};

void CodeGenerator::visitImpl(ProgramNode* node) {
  for (auto& child : node->declarationList) {
    generateForNode(std::move(child));
  }
}

std::string nodeToMIPS(VarDeclarationNode* node) {
  std::string finalString;
  if (node->type == "int") {
    finalString.append(".space ");
  }

  if (node->arraySize) {
    finalString.append(std::to_string(4 * node->arraySize.value()));
  } else {
    finalString.append("4");
  }

  return finalString;
}

void CodeGenerator::visitImpl(VarDeclarationNode* node) {
  fileToWrite << node->id << ": " << nodeToMIPS(node) << std::endl;
}

void CodeGenerator::visitImpl(FunDeclarationNode* node) {}

void CodeGenerator::visitImpl(CompoundStatementNode* node) {
  if (node->vars.size() > 0) {
    for (auto& var : node->vars) {
      generateForNode(std::move(var));
    }
  }

  if (node->statements.size() > 0) {
    for (auto& statement : node->statements) {
      generateForNode(std::move(statement));
    }
  }
}

void CodeGenerator::visitImpl(ReturnStatementNode* node) {
  generateForNode(std::move(node->expression));
}

void CodeGenerator::visitImpl(ExpressionStatementNode* node) {
  generateForNode(std::move(node->expression));
}

void CodeGenerator::visitImpl(IterationStatementNode* node) {
  generateForNode(std::move(node->expression));
  generateForNode(std::move(node->statement));
}

void CodeGenerator::visitImpl(SelectionStatementNode* node) {
  generateForNode(std::move(node->condition));
  generateForNode(std::move(node->statement));
  if (node->elseStatement) {
    generateForNode(std::move(node->elseStatement));
  }
}

void CodeGenerator::visitImpl(ExpressionNode* node) {
  if (auto expr = dynamic_cast<CallNode*>(node)) {
    visitImpl(std::move(expr));
  } else if (auto fac = dynamic_cast<FactorNode*>(node)) {
    visitImpl(std::move(fac));
  } else if (auto term = dynamic_cast<TermNode*>(node)) {
    visitImpl(std::move(term));
  } else if (auto add = dynamic_cast<AdditiveExpressionNode*>(node)) {
    visitImpl(std::move(add));
  } else if (auto simple = dynamic_cast<SimpleExpressionNode*>(node)) {
    visitImpl(std::move(simple));
  } else if (auto assign = dynamic_cast<AssignmentExpressionNode*>(node)) {
    visitImpl(std::move(assign));
  } else {
  }
}

void CodeGenerator::visitImpl(StatementNode* node) {
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
  }
}

void CodeGenerator::visitImpl(SimpleExpressionNode* node) {
  generateForNode(std::move(node->additiveLeft));
  if (node->additiveRight) {
    generateForNode(std::move(node->additiveRight));
  }
}

void CodeGenerator::visitImpl(AdditiveExpressionNode* node) {
  generateForNode(std::move(node->leftTerm));
  if (node->rightTerm) {
    generateForNode(std::move(node->rightTerm));
  }
}

void CodeGenerator::visitImpl(AssignmentExpressionNode* node) {
  generateForNode(std::move(node->var));
  generateForNode(std::move(node->simpleExpression));
}

void CodeGenerator::visitImpl(TermNode* node) {
  generateForNode(std::move(node->leftFactor));
  if (node->rightFactor) {
    generateForNode(std::move(node->rightFactor));
  }
}

void CodeGenerator::visitImpl(FactorNode* node) {
  if (node->expression) {
    generateForNode(std::move(node->expression));
  }
  if (node->var) {
    generateForNode(std::move(node->var));
  }
  if (node->call) {
    generateForNode(std::move(node->call));
  }
}

void CodeGenerator::visitImpl(CallNode* node) {
  if (node->argsList.size() > 0) {
    for (auto& arg : node->argsList) {
      generateForNode(std::move(arg));
    }
  }
}

void CodeGenerator::visitImpl(VarNode* node) {}

void CodeGenerator::visitImpl(ParamNode* node) {}

void CodeGenerator::visitImpl(DeclarationNode* node) {
  if (auto fun = dynamic_cast<FunDeclarationNode*>(node)) {
    visitImpl(fun);
  } else if (auto var = dynamic_cast<VarDeclarationNode*>(node)) {
    visitImpl(var);
  }
}
