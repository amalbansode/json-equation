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
  json eq;

  Equation (std::istream& is) {
    is >> eq;
  }

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
  MonoPolyTerm handle_monopolyterm (const json& mpt_json) const {
    MonoPolyTerm mpt;
    const auto& powers_attr = mpt_json.find("powers");
    mpt.powers = powers_attr->get<std::vector<double> >();
    const auto& coefficients_attr = mpt_json.find("coefficients");
    mpt.coefficients = coefficients_attr->get<std::vector<double> >();
    return mpt;
  }

  // Accept a JSON object following the "piece" schema and parse this for
  // storage in the vector of equation pieces.
  double handle_pieces (const json& ps, const double var) const {
    std::vector<Piece> pieces_vec(ps.size());
    size_t piece_idx = 0;
    for (json::iterator pieces_it = ps.begin(); pieces_it != ps.end(); ++pieces_it) {
      for (json::iterator piece_attr = ps.begin(); piece_attr != ps.end(); ++piece_attr) {
        if (piece_attr.key() == "lower_bound") {
          pieces_vec[piece_idx].lower_bound = piece_attr.value();
          pieces_vec[piece_idx].lower_bound_set = true;
        } else if (piece_attr.key() == "upper_bound") {
          pieces_vec[piece_idx].upper_bound = piece_attr.value();
          pieces_vec[piece_idx].upper_bound_set = true;
        } else if (pieces_vec[piece_idx].lower_bound_set &&
                   pieces_vec[piece_idx].upper_bound_set) {
          if (pieces_vec[piece_idx].lower_bound <= var &&
              var <= pieces_vec[piece_idx].upper_bound) {
            if (piece_attr.key() == "numerator") {
              pieces_vec[piece_idx].numerator = handle_monopolyterm(piece_attr.value());
            } else if (piece_attr.key() == "denominator") {
              pieces_vec[piece_idx].denominator = handle_monopolyterm(piece_attr.value());
            }
          } else { // no within bounds, so we do not care
            break;
          }
        }
      }
    }
    ++piece_idx;
  }

  // Solve the equation for a given var value
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
