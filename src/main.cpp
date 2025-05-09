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

  try {
    auto [tree, error] = parser.parser();

    if (error) {
      std::cerr << "Parsing failed: " << error->what() << std::endl;
    } else {
      tree->print(0);
    }

  } catch (const ParserSyntaxError& e) {
    std::cerr << "Parser Error: " << e.what() << std::endl;
  }

  return 0;
}
