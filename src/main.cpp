/*
 * Este es el main para el programa de compilador. Actualmente en fase de
 * semántica. Copyright (C) 2025 Andrés Tarazona Solloa
 * <andres.tara.so@gmail.com>
 * */

// Importes de librería estándar
#include <fstream>
#include <iostream>
#include <string>

// Importes de folder include/
#include "lexer.cpp"
#include "lexer.hpp"
#include "parser.cpp"
#include "parser.hpp"

int main() {
  // Instanciamos el programa en un string, sobre el que iteraremos.
  std::string fileName = "sample.c-";
  std::ifstream archivo(fileName);
  std::string prog;

  // Creamos variable linea para iterar sobre el archivo.
  std::string line;
  while (std::getline(archivo, line)) {
    prog += line + '\n';
  }

  archivo.close();
  prog += '$';
  // Instanciamos el lexer para usar los métodos que se crearon en la clase.
  Parser parser(fileName, prog, 0, prog.length());

  auto [tree, error] = parser.parser();

  if (error) {
    std::cerr << "Parsing failed: " << error->what() << std::endl;
  } else {
    tree->print(0);
  }

  return 0;
}
