#ifndef JSON_EQUATION_JSON_EQUATION_H
#define JSON_EQUATION_JSON_EQUATION_H

#include "json.hpp"

#include <cmath>
#include <vector>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <algorithm>

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
  Equation () : equation_obj() { }

  Equation (std::istream& is) {
    json equation_json;
    is >> equation_json;
    build_equation(equation_json);
  }

  Equation (Equation& other) noexcept : Equation() {
    swap(other, *this);
  }

  ~Equation () { }

  Equation& operator=(Equation other) {
    Equation temp(other);
    swap(temp, *this);

    return *this;
  }

  friend void swap(Equation& first, Equation& second) {
    std::swap(first.equation_obj, second.equation_obj);
  }

  // Solve the equation for a given var value
  double calculate (const double var) const {
    const auto& eqn = get_piece_for_var(var);
    double numerator_val = 0;
    double denominator_val = 0;

    for (size_t i = 0; i < eqn.numerator.powers.size(); ++i) {
      numerator_val += eqn.numerator.coefficients[i] * pow(var, eqn.numerator.powers[i]);
    }

    for (size_t i = 0; i < eqn.denominator.powers.size(); ++i) {
      denominator_val += eqn.denominator.coefficients[i] * pow(var, eqn.denominator.powers[i]);
    }

    return numerator_val / denominator_val;
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
    equation_obj.resize(ps.size());
    for (size_t i = 0; i < ps.size(); ++i) {
      auto& piece_obj = equation_obj[i];
      const auto& piece_json = ps[i];

      if (piece_json.find("lower_bound") != piece_json.end())
        piece_obj.lower_bound = piece_json.find("lower_bound").value();
      else
        throw std::runtime_error("[Error] Piece does not specify lower_bound.\n");

      if (piece_json.find("lb_inclusive") != piece_json.end())
        piece_obj.lower_bound_inclusive = piece_json.find("lb_inclusive").value();
      else
        throw std::runtime_error("[Error] Piece does not specify lb_inclusive.\n");

      if (piece_json.find("upper_bound") != piece_json.end())
        piece_obj.upper_bound = piece_json.find("upper_bound").value();
      else
        throw std::runtime_error("[Error] Piece does not specify upper_bound.\n");

      if (piece_json.find("ub_inclusive") != piece_json.end())
        piece_obj.upper_bound_inclusive = piece_json.find("ub_inclusive").value();
      else
        throw std::runtime_error("[Error] Piece does not specify ub_inclusive.\n");

      if (piece_json.find("numerator") != piece_json.end())
        piece_obj.numerator = read_monopolyterm(piece_json.find("numerator").value());
      else {
        std::cout
            << "[Warning] Piece does not specify numerator, setting to 1.\n";
        piece_obj.numerator = {{0}, {1}};
      }

      if (piece_json.find("denominator") != piece_json.end())
        piece_obj.denominator = read_monopolyterm(piece_json.find("denominator").value());
      else {
        std::cout
            << "[Warning] Piece does not specify denominator, setting to 1.\n";
        piece_obj.denominator = {{0}, {1}};
      }

    }
  }

  void build_equation (const json& equation_json) {
    if (equation_json.find("pieces") != equation_json.end())
      build_piece(equation_json.find("pieces").value());
    else {
      throw std::runtime_error("[Error] JSON Equation does not contain pieces.\n");
    }
  }

  const Piece& get_piece_for_var (const double var) const {
    bool lb_sat = false;
    bool ub_sat = false;
    for (const auto& piece : equation_obj) {
      lb_sat = false;
      ub_sat = false;

      if ((piece.lower_bound_inclusive && piece.lower_bound <= var) || piece.lower_bound < var)
        lb_sat = true;
      else
        continue;

      if ((piece.upper_bound_inclusive && var <= piece.upper_bound) || var < piece.upper_bound)
        ub_sat = true;
      else
        continue;

      if (lb_sat && ub_sat)
        return piece;
    }

    throw std::runtime_error("[Error] No appropriate bound range found for the variable with value " + std::to_string(var) + ".\n");
  }

  std::vector<Piece> equation_obj;
};

#endif //JSON_EQUATION_JSON_EQUATION_H
