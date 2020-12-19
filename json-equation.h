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

/*
 * This header-only library reads in a templated mathematical function defined
 * in a JSON file, and is able to solve the function given an input value for a
 * variable member.
 * This is currently designed to support polynomials using a single
 * variable for the sake of simplicity. I do have some ideas for how we can
 * support multiple variables, trig functions, and other complex operations
 * but this is unnecessary at the moment.
 * Amal Bansode, 2020. MIT License.
 */

class Equation {
public:
  Equation (std::istream& is) {
    is >> equation_json;
    build_equation();
  }

  // Solve the equation for a given var value
  double calculate (const double var) const {
    (void) var;
    return 0;
  }

private:
  struct MonoPolyTerm {
    std::vector<double> powers;
    std::vector<double> coefficients;
  };

  struct Piece {
    // parallel double vectors with corresponding variable power and coefficient
    MonoPolyTerm numerator;
    MonoPolyTerm denominator;
    // All equations are piece-wise at the moment for clarity's sake
    // (i.e. be as explicit as possible)
    double lower_bound = 0;
    double upper_bound = 0;
    // Bounds are inclusive by default
    bool lower_bound_inclusive = true;
    bool upper_bound_inclusive = true;
    // Bounds *have* to be defined as real numbers at the moment since I'm not
    // sure how to numerically represent infinities in JSON.
    bool lower_bound_set = false;
    bool upper_bound_set = false;
  };

  // Accept a JSON object following the "polyterm" schema and parse this for
  // inclusion in piece term.
  static MonoPolyTerm read_monopolyterm (const json& mpt_json) {
    MonoPolyTerm mpt;
    mpt.powers = mpt_json.at("powers").get<std::vector<double> >();
    mpt.coefficients = mpt_json.at("coefficients").get<std::vector<double> >();

    return mpt;
  }

  // Accept a JSON object following the "piece" schema and parse this for
  // storage in the vector of equation pieces.
  double build_piece (const json& ps) {
    (void) ps;
    return 0;
  }

  void build_equation () {
    bool pieces_found = false;
    for (json::iterator it = eq.begin(); it != eq.end(); ++it) {
      if (it.key() == "pieces") {
        pieces_found = true;
        build_piece(it.value());
      }
    }
    if (!pieces_found)
      throw std::runtime_error("[Error] JSON Equation does not contain pieces.");
  }

  std::vector<Piece> equation_obj;
  json equation_json;
};

#endif //JSON_EQUATION_JSON_EQUATION_H
