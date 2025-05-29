/*
 *  Este archivo es para la clase semantica.
 *  Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */
#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "errors.hpp"
#include "parser.hpp"

class ProgramNode;

enum Types { INT, VOID, BUILTIN };

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
  void print(const std::string& name) const;

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
  Scope* parent = nullptr;

  Scope(const std::string& name);
  void addChild(Scope* child) { children.push_back(child); }
  void insertNode(const std::string& name, int lineno, Types type);
  void addUsage(const std::string& name, int lineno);
  void printScope();
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

  // Scopes
  Scope* getScope(const std::string& name) {
    for (const auto& scope : scopes) {
      if (scope->name == name) {
        return scope.get();
      }
    }
    throw SemanticError("Scope not found" + name);
  }
  Scope* createScope(const std::string& name) {
    scopes.push_back(std::make_unique<Scope>(name));
    Scope* s = scopes.back().get();
    s->parent = currScope;
    return s;
  }
};

class Semantic {
  std::unique_ptr<ProgramNode> tree;
  SymbolTable symbolTable;
  int lineno = 0;
  int position = 0;
  int lineStart = 0;
  std::string fileName;
  std::vector<std::string> lines;

 private:
  // Nos movemos a través del árbol con una función de preorden y otra de
  // posorden
  void buildSymbolTable(bool imprime);
  void typeCheck(bool imprime);

 public:
  void analyze(bool imprime = true);
  Semantic(std::unique_ptr<ProgramNode> tree, const std::string& fileName,
           std::vector<std::string> lines);
  void setLineno(int lineno);
  void setPosition(int pos);
  void setLineStart(int lineStart);
  const std::string& getFileName() const { return fileName; }
  int getLineno() const { return lineno; }
  int getPosition() const { return position; }
  int getLineStart() const { return position; }
  const std::string& getCurrLine() const { return lines[lineno - 1]; }
  std::unique_ptr<ProgramNode>& getTree() { return tree; };
};
