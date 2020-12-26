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
  void build_piece (const json& ps) {
    equation_obj.reserve(ps.size());
    for (size_t i = 0; i < equation_obj.size(); ++i) {
      auto& piece_obj = equation_obj[i];
      const auto& piece_json = ps[i];

      if (piece_json.find("lower_bound") != piece_json.end())
        piece_obj.lower_bound = piece_json.find("lower_bound").value();
      else
        throw std::runtime_error("[Error] Piece does not specify lower_bound.");

      if (piece_json.find("lb_inclusive") != piece_json.end())
        piece_obj.lower_bound_inclusive = piece_json.find("lb_inclusive").value();
      else
        throw std::runtime_error("[Error] Piece does not specify lb_inclusive.");

      if (piece_json.find("upper_bound") != piece_json.end())
        piece_obj.upper_bound = piece_json.find("upper_bound").value();
      else
        throw std::runtime_error("[Error] Piece does not specify upper_bound.");

      if (piece_json.find("ub_inclusive") != piece_json.end())
        piece_obj.upper_bound_inclusive = piece_json.find("ub_inclusive").value();
      else
        throw std::runtime_error("[Error] Piece does not specify ub_inclusive.");

      if (piece_json.find("numerator") != piece_json.end())
        piece_obj.numerator = read_monopolyterm(piece_json.find("numerator").value());
      else {
        std::cout
            << "[Warning] Piece does not specify numerator, setting to 1.";
        piece_obj.numerator = {{0}, {1}};
      }

      if (piece_json.find("denominator") != piece_json.end())
        piece_obj.denominator = read_monopolyterm(piece_json.find("denominator").value());
      else {
        std::cout
            << "[Warning] Piece does not specify denominator, setting to 1.";
        piece_obj.denominator = {{0}, {1}};
      }

    }
  }

  void build_equation () {
    bool pieces_found = false;
    if (equation_json.find("pieces") != equation_json.end())
      build_piece(equation_json.find("pieces").value());
    else {
      throw std::runtime_error("[Error] JSON Equation does not contain pieces.");
    }
  }

  std::vector<Piece> equation_obj;
  json equation_json;
};

#endif //JSON_EQUATION_JSON_EQUATION_H
