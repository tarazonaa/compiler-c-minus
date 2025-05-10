/*
 *  Este archivo es para la clase semantica.
 *  Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */

#include <functional>

#include "parser.hpp"

class Semantic {
  TreeNode* tree;

 public:
  Semantic(TreeNode* tree) : tree(tree) {}
  // Nos movemos a través del árbol con una función de preorden y otra de
  // posorden
  void traverse(TreeNode* node, std::function<void(TreeNode*)> preOrder,
                std::function<void(TreeNode*)> postOrder);
  void buildSymbolTable();
  void typeCheck();
  ~Semantic() {}
  void semantic();
};
