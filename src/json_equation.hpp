/*
 * json_equation
 *
 * Copyright (c) 2020 Amal Bansode <https://www.amalbansode.com>.
 * Provided under the MIT License
 *
 * A header-only C++ library that enables the representation and computation
 * of a system of piecewise polynomial equations (of a single variable) in JSON.
 * The JSON must follow a valid schema as defined in the library's
 * documentation.
 * This library depends on:
 * - nlohmann::json <https://github.com/nlohmann/json> for JSON deserialization.
 * - numeric_range <https://github.com/amalbansode/numeric-range> for sorting,
 *   validating, and indexing by pieces' bounds.
 */

#ifndef JSON_EQUATION_HPP
#define JSON_EQUATION_HPP

#include <cmath>

/*
 * Necessary dependencies for JSON deserialization and handling piece bounds,
 * respectively. Change these paths per your project structure if needed.
 */
#include "../include/json.hpp"
#include "../include/numeric_range.hpp"

namespace json_equation {

/**
 * Monomial represents the power that a single variable may be raised to
 * and the coefficient this result is multiplied with. That is, given the
 * variable x, this represents c * (x)^p.
 */
struct Monomial
{
  double power = 0;
  double coefficient = 1;
};

/**
 * PolynomialEquation represents an m-degree polynomial as the numerator
 * and an n-degree polynomial as the denominator. Both the numerator
 * and denominator are expressions involving the same single variable.
 */
class PolynomialEquation
{
public:
  /**
   * The numerator and denominator are simply lists of monomials (of the same
   * single variable) that are individually evaluated and then arithmetically
   * added to obtain a result when given an input value.
   */
  std::vector<Monomial> numerator;
  std::vector<Monomial> denominator;

  /**
   * The numerator and denominator are default-constructed to 1.
   */
  PolynomialEquation() : numerator({{0,1}}), denominator({{0,1}}) {}

  /**
   * Calculate the result of this polynomial expression given the input value x
   * @param x Input to the expression
   * @return Result of computing f(x)
   */
  double calculate (const double x) const
  {
    double numerator_val = 0.0;
    double denominator_val = 0.0;

    for (const auto & i : numerator)
      numerator_val += i.coefficient * std::pow(x, i.power);

    for (const auto & i : denominator)
      denominator_val += i.coefficient * std::pow(x, i.power);

    if (numerator_val == 0 && denominator_val == 0)
      return 0;
    else if (numerator_val != 0 && denominator_val == 0)
      return std::numeric_limits<double>::infinity();

    return numerator_val / denominator_val;
  }

  /**
   * Calculate the result of this polynomial expression given the input value x.
   * Shorthand operator provided for convenience.
   * @param x Input to the expression
   * @return Result of computing f(x)
   */
  inline double operator() (const double x) const
  {
    return calculate(x);
  }
};

/**
 * JSONEquation represents a system of piecewise polynomial equations
 * constructed using a JSON input.
 */
class JSONEquation
{
public:
  /**
   * The "secret sauce" of this representation is a numeric range mapped
   * to a polynomial equation.
   */
  std::map<numeric_range::NumericRange<double>, PolynomialEquation,
  numeric_range::NumericRangeComparator<double> > pieces;

  JSONEquation () = default;

  /**
   * Construct JSONEquation from an istream containing JSON data.
   * @param is istream corresponding to JSON needed to build a JSONEquation
   * object. This is expected to follow the schema laid out in documentation.
   */
  explicit JSONEquation (std::istream& is) : JSONEquation()
  {
    nlohmann::json json_obj;
    is >> json_obj;
    build_equation(json_obj);
  }

  /**
   * Construct JSONEquation from an nlohmann::json object containing JSON data.
   * @param json_in JSON needed to build a JSONEquation object. This is
   * expected to follow the schema laid out in documentation.
   */
  explicit JSONEquation (const nlohmann::json& json_in) : JSONEquation()
  {
    build_equation(json_in);
  }

  JSONEquation (JSONEquation& other) : JSONEquation()
  {
    pieces = other.pieces;
  }

  JSONEquation& operator= (JSONEquation other)
  {
    JSONEquation temp(other);
    swap(temp, *this);
    return *this;
  }

  /**
   * Swap two systems (i.e. their pieces) in memory
   * @param first
   * @param second
   */
  friend void swap (JSONEquation& first, JSONEquation& second)
  {
    std::swap(first.pieces, second.pieces);
  }

  ~JSONEquation () = default;

  /**
   * Calculate the output of the polynomial system given input x. If x is not
   * included in the range for any piece, std::nullopt is returned instead and
   * it is up to the caller to determine the course of action.
   * @param x Input to the system of equations
   * @return nullopt if x not included in any pieces' range.Else, double val
   */
  std::optional<double> calculate (const double x)
  {
    const auto found_piece = pieces.find(numeric_range::NumericRange<double>{x});
    if (found_piece != pieces.end())
      return found_piece->second.calculate(x);
    else
      return std::nullopt;
  }

  /**
   * Calculate the output of the polynomial system given input x. If x is not
   * included in the range for any piece, std::nullopt is returned instead and
   * it is up to the caller to determine the course of action.
   * Shorthand operator provided for convenience.
   * @param x Input to the system of equations
   * @return nullopt if x not included in any pieces' range.Else, double val
   */
  std::optional<double> operator() (const double x)
  {
    const auto found_piece = pieces.find(numeric_range::NumericRange<double>{x});
    if (found_piece != pieces.end())
      return found_piece->second.calculate(x);
    else
      return std::nullopt;
  }

private:
  /**
   * Build the system of equations from JSON input. Input is expected to follow
   * the schema laid out by the library. Missing attributes are handled as
   * specified in documentation.
   * @param eq_in JSON representing a system of piecewise polynomial equations
   */
  void build_equation (const nlohmann::json& eq_in)
  {
    pieces.clear();
    const auto pieces_in = eq_in.find("pieces");
    if (pieces_in != eq_in.end())
    {
      const auto pieces_list_in = pieces_in.value();
      for (size_t i = 0; i < pieces_list_in.size(); ++i)
      {
        try
        {
          build_and_add_piece(pieces_list_in[i], i);
        }
        catch (const std::exception& e)
        {
          throw std::runtime_error("Error building JSONEquation: " + std::string(e.what()));
        }
      }
    }
    else
    {
      throw std::runtime_error("JSON object does not contain \"pieces\" key needed for building JSONEquation.");
    }
  }

  /**
   * Perform error-checking on a given piece and add it to the current system
   * if it is valid.
   * @param piece_in JSON corresponding to "piece" in a piecewise equation
   * @param idx Index of this piece in the pieces list used for error messages
   */
  void build_and_add_piece (const nlohmann::json& piece_in, const size_t idx)
  {
    double lb = 0;
    double ub = 0;
    bool lb_inclusive = false;
    bool ub_inclusive = false;

    /*
     * Construct the error string prefix beforehand for convenience
     */
    const std::string idx_str = std::to_string(idx);
    const std::string error_prefix = "Piece at index " + idx_str + " ";

    /*
     * The Lower Bound and Upper Bound attributes must be specified in JSON
     */
    if (piece_in.find("lower_bound") != piece_in.end())
      lb = piece_in.find("lower_bound").value();
    else
      throw std::runtime_error(error_prefix + "does not specify lower_bound.");

    if (piece_in.find("upper_bound") != piece_in.end())
      ub = piece_in.find("upper_bound").value();
    else
      throw std::runtime_error(error_prefix + "does not specify upper_bound.");

    /*
     * Get the inclusive/exclusive attribute for lower and upper bounds.
     * If unspecified, the default is "true".
     */
    if (piece_in.find("lb_inclusive") != piece_in.end())
      lb_inclusive = piece_in.find("lb_inclusive").value();
    else
      lb_inclusive = true;

    if (piece_in.find("ub_inclusive") != piece_in.end())
      ub_inclusive = piece_in.find("ub_inclusive").value();
    else
      ub_inclusive = true;

    numeric_range::NumericRange<double> bounds{lb, lb_inclusive, ub, ub_inclusive};

    /*
     * Get the numerator and denominator attributes.
     * If numerator AND denominator absent, both are set to "0".
     * If numerator XOR denominator absent, the default is "1" for the absent element.
     */
    bool numerator_present = piece_in.find("numerator") != piece_in.end();
    bool denominator_present = piece_in.find("denominator") != piece_in.end();

    PolynomialEquation function;

    /*
     * If neither numerator nor denominator present, set function to return 0
     */
    if (!numerator_present && !denominator_present)
    {
      function.numerator = {{0, 0}};
      function.denominator = {{0, 0}};
    }

    if (numerator_present)
    {
      function.numerator.clear();

      /*
       * In JSON, these are "parallel arrays". However, in code and memory
       * corresponding elements are serialized to PolyTerm objects
       * inside the PolynomialEquation object
       */
      const auto numerator_powers_in = piece_in.at("numerator").at("powers").get<std::vector<double> >();
      const auto numerator_coeffs_in = piece_in.at("numerator").at("coefficients").get<std::vector<double> >();

      if (numerator_powers_in.size() != numerator_coeffs_in.size())
        throw std::runtime_error(error_prefix + "numerator cannot have len(powers) != len(coefficients)");

      for (size_t i = 0; i < numerator_powers_in.size(); ++i)
        function.numerator.push_back({numerator_powers_in[i], numerator_coeffs_in[i]});
    }

    if (denominator_present)
    {
      function.denominator.clear();

      /*
       * In JSON, these are "parallel arrays". However, in code and memory
       * corresponding elements are serialized to PolyTerm objects
       * inside the PolynomialEquation object
       */
      const auto denominator_powers_in = piece_in.at("denominator").at("powers").get<std::vector<double> >();
      const auto denominator_coeffs_in = piece_in.at("denominator").at("coefficients").get<std::vector<double> >();

      if (denominator_powers_in.size() != denominator_coeffs_in.size())
        throw std::runtime_error(error_prefix + "denominator cannot have len(powers) != len(coefficients)");

      for (size_t i = 0; i < denominator_powers_in.size(); ++i)
        function.denominator.push_back({denominator_powers_in[i], denominator_coeffs_in[i]});
    }

    /*
     * Try inserting this piece into the map. Throw on error.
     */
    try
    {
      pieces.insert({bounds, function});
    }
    catch (const std::exception& e)
    {
      throw std::runtime_error(error_prefix + "could not be added to piecewise equation map. Map insert operation threw error: " + e.what());
    }
  } /* void build_and_add_piece */
};

} /* namespace json_equation */

#endif //JSON_EQUATION_HPP
