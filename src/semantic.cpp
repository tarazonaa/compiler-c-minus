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
#include <vector>

#include "colors.hpp"
#include "errors.hpp"
#include "parser.hpp"
#include "visitor.hpp"

std::string typesToString(Types type) {
  if (type == Types::INT) {
    return "int";
  } else if (type == Types::BUILTIN) {
    return "builtin";
  }
  return "void";
}

/*
 *  Functiones para las tablas de simbolos simples
 * */
std::string padAndStyle(
    const std::string& content, int width,
    const std::function<std::string(const std::string&)>& styleFn) {
  std::ostringstream out;
  out << std::left << std::setw(width) << content;
  return styleFn(out.str());
}

std::vector<std::string> wrapLines(const std::string& str, size_t width) {
  std::vector<std::string> result;
  std::istringstream stream(str);
  std::string token;
  std::string line;
  while (std::getline(stream, token, ',')) {
    token.erase(0, token.find_first_not_of(" "));  // trim leading space
    std::string addition = (line.empty() ? "" : ", ") + token;
    if (line.length() + addition.length() > width) {
      result.push_back(line);
      line = token;
    } else {
      line += addition;
    }
  }
  if (!line.empty()) result.push_back(line);
  return result;
}

void Symbols::print() const {
  using std::left;
  using std::setw;

  const std::string sep =
      "├────────────────┼────────────┼──────────┼─────────┼────────────────┤";
  const std::string top =
      "╭────────────────────────── Symbol Table ───────────────────────────╮";
  const std::string bot =
      "╰────────────────┴────────────┴──────────┴─────────┴────────────────╯";

  std::cout << Style::bold(top) << "\n";
  std::cout << "│ " << padAndStyle("Name", 14, Style::bold) << " │ "
            << padAndStyle("Type", 10, Style::bold) << " │ "
            << padAndStyle("Scope", 8, Style::bold) << " │ "
            << padAndStyle("Array", 7, Style::bold) << " │ "
            << padAndStyle("Lines", 14, Style::bold) << " │\n";
  std::cout << Style::gray(sep) << "\n";

  for (const auto& [name, info] : symbolTable) {
    std::stringstream lines;
    for (size_t i = 0; i < info.lines.size(); ++i) {
      lines << info.lines[i];
      if (i < info.lines.size() - 1) lines << ", ";
    }

    std::vector<std::string> wrappedLines = wrapLines(lines.str(), 14);
    size_t numLines = std::max<size_t>(1, wrappedLines.size());

    for (size_t i = 0; i < numLines; ++i) {
      std::cout << "│ "
                << (i == 0 ? padAndStyle(name.empty() ? "<anon>" : name, 14,
                                         Style::cyan)
                           : std::string(14, ' '))
                << " │ "
                << (i == 0
                        ? padAndStyle(typesToString(info.type), 10, Style::blue)
                        : std::string(10, ' '))
                << " │ "
                << (i == 0 ? padAndStyle(info.scope.empty() ? "-" : info.scope,
                                         8, Style::yellow)
                           : std::string(8, ' '))
                << " │ "
                << (i == 0
                        ? padAndStyle(info.isArray ? "true" : "false", 7,
                                      info.isArray ? Style::green : Style::red)
                        : std::string(7, ' '))
                << " │ "
                << padAndStyle(wrappedLines.empty() ? "-" : wrappedLines[i], 14,
                               Style::gray)
                << " │\n";
    }
  }

  std::cout << Style::bold(bot) << "\n";
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
void Semantic::setLineno(int line) { lineno = line; }
void Semantic::setPosition(int pos) { position = pos; }
void Semantic::setLineStart(int ls) { lineStart = ls; }

void Semantic::buildSymbolTable(bool imprime) {
  SymbolTableVisitor visitor(symbolTable, *this);
  visitor.visit(tree);
}
void Semantic::typeCheck(bool imprime) {
  TypeCheckerVisitor visitor(symbolTable, *this);
  visitor.visit(tree);
}

void Semantic::analyze(bool imprime) {
  try {
    buildSymbolTable(imprime);
    symbolTable.print();
    typeCheck(imprime);
    std::cout << "Se ha logrado el typechecking correctamente." << std::endl;
  } catch (const SemanticError& e) {
    std::cout << e.format() << std::endl;
  }
}

Semantic::Semantic(std::unique_ptr<ProgramNode> tree,
                   const std::string& fileName, std::vector<std::string> lines)
    : tree(std::move(tree)), fileName(fileName), lines(lines) {
  symbolTable = SymbolTable();
}

SymbolTable::SymbolTable() {
  scopes.push_back(std::make_unique<Scope>("__global"));
  globalScope = scopes.back().get();
  currScope = globalScope;
  currScope->insertNode("output", 0, Types::BUILTIN);
  currScope->insertNode("input", 0, Types::BUILTIN);
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
  Scope* scope = currScope;
  while (scope != nullptr) {
    if (scope->symbolTable.find(name)) {
      return true;
    }
    scope = scope->parent;
  }
  return false;
}

/*
 *  Funciones que tienen que ver con los scopes
 * */
Scope::Scope(const std::string& name) : name(name) {
  std::vector<Scope*> children;
}

void Scope::printScope() {
  std::cout << Style::cyan("Imprimiendo scope: ") << Style::italic(name)
            << std::endl;
  if (parent != nullptr) {
    std::cout << Style::yellow("Scope padre: ") << Style::italic(parent->name)
              << std::endl;
  }
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
  Scope* scope = currScope;
  while (scope != nullptr) {
    if (scope->symbolTable.find(name)) {
      return scope->symbolTable.symbolTable.at(name).type;
    }
    scope = scope->parent;
  }
  throw SemanticError("Undeclared variable: " + name);
}
