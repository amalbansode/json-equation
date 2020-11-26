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

  struct MonoPolyTerm {
    std::vector<double> powers;
    std::vector<double> coefficients;
  };

  struct Piece {
    MonoPolyTerm numerator;
    MonoPolyTerm denominator;
    double lower_bound = 0;
    double upper_bound = 0;
    bool lower_bound_set = false;
    bool upper_bound_set = false;
  };

  MonoPolyTerm handle_monopolyterm (const json& mpt_json) const {
    MonoPolyTerm mpt;
    const auto& powers_attr = mpt_json.find("powers");
    mpt.powers = powers_attr->get<std::vector<double> >();
    const auto& coefficients_attr = mpt_json.find("coefficients");
    mpt.coefficients = coefficients_attr->get<std::vector<double> >();
    return mpt;
  }

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
