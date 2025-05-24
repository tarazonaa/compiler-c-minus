/*
 *  Copyright (c) 2025 Andres Tarazona Solloa <andres.tara.so@gmail.com>
 *  Este es el header file de colores.
 * */
#pragma once

#include <string>
namespace Color {
inline const std::string reset = "\033[0m";
inline const std::string bold = "\033[1m";
inline const std::string underline = "\033[4m";

inline const std::string black = "\033[30m";
inline const std::string red = "\033[31m";
inline const std::string green = "\033[32m";
inline const std::string yellow = "\033[33m";
inline const std::string blue = "\033[34m";
inline const std::string magenta = "\033[35m";
inline const std::string cyan = "\033[36m";
inline const std::string white = "\033[37m";

inline const std::string bold_red = "\033[1;31m";
inline const std::string bold_green = "\033[1;32m";
inline const std::string bold_yellow = "\033[1;33m";
inline const std::string bold_blue = "\033[1;34m";
}  // namespace Color
//
#pragma once
#include <string>

namespace Style {
inline std::string wrap(const std::string& code, const std::string& s) {
  return code + s + "\033[0m";
}

inline std::string bold(const std::string& s) { return wrap("\033[1m", s); }
inline std::string underline(const std::string& s) {
  return wrap("\033[4m", s);
}
inline std::string italic(const std::string& s) { return wrap("\033[3m", s); }
inline std::string faint(const std::string& s) { return wrap("\033[2m", s); }
inline std::string inverse(const std::string& s) { return wrap("\033[7m", s); }

inline std::string red(const std::string& s) { return wrap("\033[31m", s); }
inline std::string green(const std::string& s) { return wrap("\033[32m", s); }
inline std::string yellow(const std::string& s) { return wrap("\033[33m", s); }
inline std::string blue(const std::string& s) { return wrap("\033[34m", s); }
inline std::string magenta(const std::string& s) { return wrap("\033[35m", s); }
inline std::string cyan(const std::string& s) { return wrap("\033[36m", s); }
inline std::string gray(const std::string& s) { return wrap("\033[90m", s); }

inline std::string bold_red(const std::string& s) {
  return wrap("\033[1;31m", s);
}
inline std::string bold_yellow(const std::string& s) {
  return wrap("\033[1;33m", s);
}
}  // namespace Style
