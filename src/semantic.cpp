/*
 *  Este archivo es para la implementación de la clase semantica
 *  Copyright (C) 2025 Andrés Tarazona Solloa <andres.tara.so@gmail.com>
 * */

#include "semantic.hpp"

void Semantic::traverse(TreeNode* node, std::function<void(TreeNode*)> preOrder,
                        std::function<void(TreeNode*)> postOrder) {
  preOrder(node);
  for (auto& child :) {
    traverse(child, preOrder, postOrder);
  }
  postOrder(node);
}

void Semantic::buildSymbolTable() {
  traverse(tree, [](TreeNode* node) {}, [](TreeNode* node) {});
}

void Semantic::typeCheck() {
  traverse(tree, [](TreeNode* node) {}, [](TreeNode* node) {});
}

void insertNode() {}
