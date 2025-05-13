/*
 *  Este archivo es para la clase semantica.
 *  Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */
#pragma once

#include <functional>

#include "parser.hpp"

class SymbolTable {
  std::unordered_map<std::string, std::vector<int>> symbolTable;

 public:
  void insertNode(const std::string& name, int lineno);
  bool find(const std::string& name) const;
  void print() const;
};

class TypeChecker {
  SymbolTable* symbolTable;

 public:
  TypeChecker(SymbolTable* symbolTable) : symbolTable(symbolTable) {}
};

class Semantic {
  TreeNode* tree;
  SymbolTable symbolTable;

 private:
  // Nos movemos a través del árbol con una función de preorden y otra de
  // posorden
  void traverse(TreeNode* node, std::function<void(TreeNode*)> preOrder,
                std::function<void(TreeNode*)> postOrder);
  void buildSymbolTable();
  void typeCheck();

 public:
  void analyze();
  explicit Semantic(TreeNode* tree);
};
