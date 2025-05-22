/*
 *  Este archivo es para la clase semantica.
 *  Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */
#pragma once

#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "errors.hpp"
#include "parser.hpp"

class ProgramNode;

enum Types { INT, VOID };

class Symbols {
 public:
  struct SymbolInfo {
    int declaredAt;
    std::vector<int> lines;
    std::string scope;
    Types type;
    bool isArray;
    int size;
  };
  std::unordered_map<std::string, SymbolInfo> symbolTable;

  void insertNode(const std::string& name, int lineno);
  void addUsage(const std::string& name, int lineno);
  bool find(const std::string& name) const;
  void print() const;

  Symbols& operator+=(const std::pair<std::string, int>& decl) {
    addUsage(decl.first, decl.second);
    return *this;
  };

  void operator()(const std::pair<std::string, int>& decl) {
    insertNode(decl.first, decl.second);
  };
};

class Scope {
 public:
  std::string name;
  Symbols symbolTable;
  std::vector<Scope*> children;

  Scope(const std::string& name);
  void addChild(Scope* child) { children.push_back(child); }
  void insertNode(const std::string& name, int lineno, Types type);
  void addUsage(const std::string& name, int lineno);
  void printScope();
};

class TypeChecker {
  Symbols* symbolTable;

 public:
  TypeChecker(Symbols* symbolTable) : symbolTable(symbolTable) {}
};

class SymbolTable {
 public:
  std::vector<std::unique_ptr<Scope>> scopes;
  Scope* globalScope;
  Scope* currScope;
  int scopeAllocCount = 0;

  SymbolTable();
  void insertNode(const std::string& name, int lineno, Types type);
  void insertNode(const std::string& name, int lineno, Types type,
                  int arraySize);
  void addUsage(const std::string& name, int lineno);
  bool find(const std::string& name) const;
  void print() const;

  Types getType(const std::string& name);
  Scope* createScope(const std::string& name) {
    scopes.push_back(std::make_unique<Scope>(name));
    Scope* s = scopes.back().get();
    return s;
  }
};

class Semantic {
  std::unique_ptr<ProgramNode> tree;
  SymbolTable symbolTable;

 private:
  // Nos movemos a través del árbol con una función de preorden y otra de
  // posorden
  void buildSymbolTable(bool imprime);
  void typeCheck(bool imprime);

 public:
  void analyze(bool imprime = true);
  explicit Semantic(std::unique_ptr<ProgramNode> tree);
};
