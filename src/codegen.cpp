/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el implementation file del generador de código
 * */
#include "codegen.hpp"

#include <fstream>

#include "semantic.hpp"

CodeGenerator::CodeGenerator(Semantic& semantic) : semantic(semantic) {
  std::ofstream fileToWrite = std::ofstream("main.mips");
};

void CodeGenerator::setup() { fileToWrite << ".data" << std::endl; }

void CodeGenerator::generate() { setup(); }
