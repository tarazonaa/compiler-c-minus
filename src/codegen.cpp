/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el implementation file del generador de código
 * */
#include "codegen.hpp"

#include <fstream>
#include <memory>
#include <string>

#include "colors.hpp"
#include "parser.hpp"
#include "semantic.hpp"

int labelCounter = 0;

#include <fstream>
#include <iostream>

void printGeneratedCode(const std::string& filename) {
  std::ifstream input(filename);
  std::string line;
  if (!input.is_open()) {
    std::cerr << Style::bold_red("Error: ") << "Could not open file "
              << filename << "\n";
    return;
  }

  std::cout << Style::bold("Generated MIPS Code:\n\n");
  while (std::getline(input, line)) {
    if (line.empty()) {
      std::cout << "\n";
    } else if (line[0] == '.') {
      std::cout << Style::cyan(line) << "\n";
    } else if (line.back() == ':') {
      std::cout << Style::yellow(line) << "\n";
    } else {
      std::cout << Style::green("  " + line) << "\n";
    }
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

CodeGenerator::CodeGenerator(Semantic& semantic) : semantic(semantic) {}

void CodeGenerator::setup() {
  fileToWrite.open("main.mips");

  fileToWrite << ".data" << std::endl;
  isInGlobals = true;

  for (const auto& decl : semantic.getTree()->declarationList) {
    if (auto var = dynamic_cast<VarDeclarationNode*>(decl.get())) {
      if (emittedGlobals.count(var->id)) continue;
      emittedGlobals.insert(var->id);
      fileToWrite << var->id << ": " << nodeToMIPS(var) << std::endl;
    }
  }

  isInGlobals = false;
  fileToWrite << "\n.text\n.globl main\nmain:\n";
  fileToWrite << "  jal main_entry\n";
}

void CodeGenerator::generate() {
  setup();
  generateForNode(semantic.getTree().get());
  fileToWrite.close();
  printGeneratedCode("main.mips");
}

template <typename Node>
void CodeGenerator::generateForNode(Node* tree) {
  if (!tree) return;
  visitImpl(tree);
}

void CodeGenerator::visitImpl(ProgramNode* node) {
  for (auto& child : node->declarationList) {
    generateForNode(child.get());
  }
}

void CodeGenerator::visitImpl(VarDeclarationNode* node) {
  if (isInGlobals) {
    if (emittedGlobals.count(node->id)) return;
    emittedGlobals.insert(node->id);
    fileToWrite << node->id << ": " << nodeToMIPS(node) << std::endl;
  } else {
    localOffsets[node->id] = currentStackOffset;
    fileToWrite << "  # Allocate local var '" << node->id << "' at "
                << currentStackOffset << "($fp)\n";
    currentStackOffset -= 4;
  }
}

void CodeGenerator::visitImpl(FunDeclarationNode* node) {
  fileToWrite << "\n" << node->id << "_entry:" << std::endl;

  localOffsets.clear();
  currentStackOffset = -4;

  fileToWrite << "  move $fp, $sp\n";
  fileToWrite << "  sw $fp, 0($sp)\n";
  fileToWrite << "  addiu $sp, $sp, -4\n";
  fileToWrite << "  sw $ra, 0($sp)\n";
  fileToWrite << "  addiu $sp, $sp, -4\n";

  offsets.clear();
  int paramOffset = 8;
  for (const auto& param : node->params) {
    offsets[param->id] = paramOffset;
    paramOffset += 4;
  }

  generateForNode(node->compoundStatement.get());

  int totalArgsBytes = 4 * node->params.size();

  if (node->id == "main") {
    fileToWrite << "  lw $ra, 4($sp)\n";
    fileToWrite << "  lw $fp, 0($sp)\n";
    fileToWrite << "  addiu $sp, $sp, 8\n";
    fileToWrite << "  li $v0, 10\n";
    fileToWrite << "  syscall\n";
  } else {
    fileToWrite << "  lw $ra, 4($sp)\n";
    fileToWrite << "  lw $fp, 0($sp)\n";
    fileToWrite << "  addiu $sp, $sp, 8\n";
    fileToWrite << "  jr $ra\n";
  }
}

void CodeGenerator::visitImpl(CompoundStatementNode* node) {
  for (auto& var : node->vars) generateForNode(var.get());
  for (auto& stmt : node->statements) generateForNode(stmt.get());
}

void CodeGenerator::visitImpl(ReturnStatementNode* node) {
  generateForNode(node->expression.get());
  fileToWrite << "  move $v0, $t0\n";
}

void CodeGenerator::visitImpl(ExpressionStatementNode* node) {
  generateForNode(node->expression.get());
}

void CodeGenerator::visitImpl(IterationStatementNode* node) {
  std::string loop = "loop" + std::to_string(labelCounter);
  std::string end = "endloop" + std::to_string(labelCounter++);
  fileToWrite << loop << ":\n";
  generateForNode(node);
  fileToWrite << "  beq $t0, $zero, " << end << "\n";
  generateForNode(node);
  fileToWrite << "  j " << loop << "\n";
  fileToWrite << end << ":\n";
}

void CodeGenerator::visitImpl(SelectionStatementNode* node) {
  std::string trueLbl = "true" + std::to_string(labelCounter);
  std::string endLbl = "endif" + std::to_string(labelCounter++);
  generateForNode(node->condition.get());
  fileToWrite << "  bne $t0, $zero, " << trueLbl << "\n";
  if (node->elseStatement) generateForNode(node->elseStatement.get());
  fileToWrite << "  j " << endLbl << "\n";
  fileToWrite << trueLbl << ":\n";
  generateForNode(node->statement.get());
  fileToWrite << endLbl << ":\n";
}

void CodeGenerator::visitImpl(ExpressionNode* node) {
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
  }
}

void CodeGenerator::visitImpl(SimpleExpressionNode* node) {
  if (node->additiveRight) {
    generateForNode(node->additiveLeft.get());
    fileToWrite << "  move $t1, $t0\n";
    generateForNode(node->additiveRight.get());
    fileToWrite << "  add $t0, $t1, $t0\n";
  } else {
    generateForNode(node->additiveLeft.get());
  }
}

void CodeGenerator::visitImpl(AdditiveExpressionNode* node) {
  if (node->rightTerm) {
    generateForNode(node->leftTerm.get());
    fileToWrite << "  move $t1, $t0\n";
    generateForNode(node->rightTerm.get());
    fileToWrite << "  add $t0, $t1, $t0\n";
  } else {
    generateForNode(node->leftTerm.get());
  }
}

void CodeGenerator::visitImpl(AssignmentExpressionNode* node) {
  generateForNode(node->simpleExpression.get());

  if (offsets.count(node->var->id)) {
    fileToWrite << "  sw $t0, " << offsets[node->var->id] << "($fp)\n";
  } else if (localOffsets.count(node->var->id)) {
    fileToWrite << "  sw $t0, " << localOffsets[node->var->id] << "($fp)\n";
  } else {
    fileToWrite << "  la $t1, " << node->var->id << "\n";
    fileToWrite << "  sw $t0, 0($t1)\n";
  }
}

void CodeGenerator::visitImpl(TermNode* node) {
  if (node->rightFactor) {
    generateForNode(node->leftFactor.get());
    fileToWrite << "  move $t1, $t0\n";
    generateForNode(node->rightFactor.get());
    fileToWrite << "  mul $t0, $t1, $t0\n";
  } else {
    generateForNode(node->leftFactor.get());
  }
}

void CodeGenerator::visitImpl(FactorNode* node) {
  if (node->expression) {
    generateForNode(node->expression.get());
  } else if (node->var) {
    generateForNode(node->var.get());
  } else if (node->call) {
    generateForNode(node->call.get());
  }
}

void CodeGenerator::visitImpl(CallNode* node) {
  std::string& funcName = node->id;
  if (funcName == "input") {
    fileToWrite << "  li $v0, 5\n";
    fileToWrite << "  syscall\n";
    fileToWrite << "  move $t0, $v0\n";
    return;
  }

  if (funcName == "output") {
    generateForNode(node->argsList[0].get());
    fileToWrite << "  move $a0, $t0\n";
    fileToWrite << "  li $v0, 1\n";
    fileToWrite << "  syscall\n";
    return;
  }
  for (int i = node->argsList.size() - 1; i >= 0; --i) {
    generateForNode(node->argsList[i].get());
    fileToWrite << "  sw $t0, 0($sp)\n";
    fileToWrite << "  addiu $sp, $sp, -4\n";
  }
  fileToWrite << "  jal " << node->id << "_entry\n";
  fileToWrite << "  move $t0, $v0\n";
}

void CodeGenerator::visitImpl(VarNode* node) {
  if (offsets.count(node->id)) {
    int offset = offsets[node->id];
    fileToWrite << "  lw $t0, " << offset << "($fp)\n";
  } else if (localOffsets.count(node->id)) {
    int offset = localOffsets[node->id];
    fileToWrite << "  lw $t0, " << offset << "($fp)\n";
  } else {
    fileToWrite << "  la $t1, " << node->id << "\n";
    fileToWrite << "  lw $t0, 0($t1)\n";
  }
}

void CodeGenerator::visitImpl(ParamNode* node) {
  fileToWrite << "  # ParamNode code generation not implemented\n";
}

void CodeGenerator::visitImpl(DeclarationNode* node) {
  if (auto fun = dynamic_cast<FunDeclarationNode*>(node)) {
    visitImpl(fun);
  } else if (auto var = dynamic_cast<VarDeclarationNode*>(node)) {
    visitImpl(var);
  }
}
