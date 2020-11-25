#ifndef JSON_EQUATION_JSON_EQUATION_H
#define JSON_EQUATION_JSON_EQUATION_H

#include "json.hpp"

#include <cmath>
#include <vector>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>

class Equation {
public:
  nlohmann::json eq;

  Equation (std::istream& is) : eq(is) { }

  double calculate (const std::vector<double>& vars) const {

    return 0;
  }
};

#endif //JSON_EQUATION_JSON_EQUATION_H
