/*
 *  Este archivo es para la implementación de la clase semantica
 *  Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */

#include "semantic.hpp"

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>

#include "parser.hpp"
#include "visitor.hpp"

std::string typesToString(Types type) {
  if (type == Types::INT) {
    return "int";
  }
  return "void";
}

/*
 *  Functiones para las tablas de simbolos simples
 * */
void Symbols::print() const {
  std::cout
      << "+------------------+-------------------+---------------------+\n";
  std::cout
      << "| Symbol           |   Types           | Line Numbers        |\n";
  std::cout
      << "+------------------+-------------------+---------------------+\n";

  for (const auto& [name, info] : symbolTable) {
    std::cout << "| " << std::left << std::setw(15) << name << " | ";
    std::cout << std::left << std::setw(15)
              << "type: " << typesToString(info.type) << std::setw(2) << "|";

    std::stringstream lineNumbers;
    for (size_t i = 0; i < info.lines.size(); ++i) {
      if (i > 0) lineNumbers << ", ";
      lineNumbers << info.lines[i];
    }

    std::cout << std::left << std::setw(18) << lineNumbers.str() << " |\n";
  }
  std::cout << "+----------------------+---------------------+\n";
}

void Symbols::insertNode(const std::string& name, int lineno) {
  if (!name.empty() && lineno > 0) {
    symbolTable[name].lines.push_back(lineno);
  }
}

bool Symbols::find(const std::string& name) const {
  return symbolTable.find(name) != symbolTable.end();
}

void Symbols::addUsage(const std::string& name, int lineno) {
  symbolTable[name].lines.push_back(lineno);
}

/*
 *  Funciones que tienen que ver con el semántico
 * */
void Semantic::buildSymbolTable(bool imprime) {
  SymbolTableVisitor visitor(symbolTable);
  visitor.visit(tree);
}
void Semantic::typeCheck(bool imprime) {
  TypeCheckerVisitor visitor(symbolTable);
  visitor.visit(tree);
}

void Semantic::analyze(bool imprime) {
  buildSymbolTable(imprime);
  symbolTable.print();
  typeCheck(imprime);
  std::cout << "Se ha logrado el typechecking correctamente." << std::endl;
}

Semantic::Semantic(std::unique_ptr<ProgramNode> tree) : tree(std::move(tree)) {
  symbolTable = SymbolTable();
}

SymbolTable::SymbolTable() {
  scopes.push_back(std::make_unique<Scope>("__global"));
  globalScope = scopes.back().get();
  currScope = globalScope;
}

void SymbolTable::insertNode(const std::string& name, int lineno, Types type) {
  if (find(name)) {
    currScope->insertNode(name, lineno, type);
  }
  currScope->insertNode(name, lineno, type);
}

void SymbolTable::insertNode(const std::string& name, int lineno, Types type,
                             int size = 0) {
  currScope->insertNode(name, lineno, type);
  if (size > 0) {
    currScope->symbolTable.symbolTable[name].isArray = true;
    currScope->symbolTable.symbolTable[name].size = size;
  }
}

void SymbolTable::print() const {
  for (auto& scope : scopes) {
    scope->printScope();
  }
}

bool SymbolTable::find(const std::string& name) const {
  return currScope->symbolTable.find(name);
}

/*
 *  Funciones que tienen que ver con los scopes
 * */
Scope::Scope(const std::string& name) : name(name) {
  std::vector<Scope*> children;
}

void Scope::printScope() {
  std::cout << "Imrpimiendo scope: " << name << std::endl;
  symbolTable.print();
}

void Scope::insertNode(const std::string& name, int lineno, Types type) {
  symbolTable(std::make_pair(name, lineno));
  symbolTable.symbolTable[name].type = type;
}

void Scope::addUsage(const std::string& name, int lineno) {
  symbolTable += {name, lineno};
}

Types SymbolTable::getType(const std::string& name) {
  Types type = currScope->symbolTable.symbolTable[name].type;
  if (type == Types::INT) {
    return type;
  } else {
    std::cerr << "Cannot index type of " << name << std::endl;
  }
  return Types::VOID;
}
