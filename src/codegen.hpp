/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el header file del generador de código
 * */
#pragma once
#include <fstream>

#include "parser.hpp"
#include "semantic.hpp"
class CodeGenerator {
  Semantic& semantic;
  std::ofstream fileToWrite;

 public:
  CodeGenerator(Semantic& semantic);

  void generate();
  void setup();
  void writeToFile(std::string& content);
};
