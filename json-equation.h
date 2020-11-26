#ifndef JSON_EQUATION_JSON_EQUATION_H
#define JSON_EQUATION_JSON_EQUATION_H

#include "json.hpp"

#include <cmath>
#include <vector>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>

using nlohmann::json;

class Equation {
public:
  json eq;

  Equation (std::istream& is) {
    is >> eq;
  }

  double handle_pieces (const json& ps, const double var) const {

  }

  double calculate (const double var) const {
    for (json::iterator it = eq.begin(); it != eq.end(); ++it) {
      if (it.key() == "pieces") {
        handle_pieces(it.value(), var);
      }
    }
    return 0;
  }
};

#endif //JSON_EQUATION_JSON_EQUATION_H
