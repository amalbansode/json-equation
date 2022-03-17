#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()

#include "catch.hpp"
#include "../include/json.hpp"
#include "../src/json_equation.hpp"

using namespace std;
using namespace nlohmann;
using namespace json_equation;

TEST_CASE("Single Piece Construction & Computation", "[json_equation]") {
  ifstream infile("../test/single_piece.json");
  JSONEquation equation(infile);

  // Values within the bounds of a piece should be calculated successfully
  auto x0 = equation.calculate(0);
  REQUIRE(x0.has_value());
  REQUIRE(x0.value() == 5.0);

  auto x0p5 = equation.calculate(0.5);
  REQUIRE(x0p5.has_value());
  REQUIRE(x0p5.value() == 10.0);

  // Values outside the bounds of all pieces should not have a value
  auto x5 = equation.calculate(5);
  REQUIRE(!x5.has_value());
}

TEST_CASE("Overload operator()", "[json_equation]") {
  ifstream infile("../test/single_piece.json");
  JSONEquation equation(infile);

  auto x0 = equation(0);
  REQUIRE(x0.has_value());
  REQUIRE(x0.value() == 5.0);
}

TEST_CASE("Multiple Pieces Construction & Computation", "[json_equation]") {
  ifstream infile("../test/multiple_pieces.json");
  JSONEquation equation(infile);

  auto x0 = equation.calculate(0);
  REQUIRE(x0.has_value());
  REQUIRE(x0.value() == 2.0);

  auto x2 = equation.calculate(2.0);
  REQUIRE(x2.has_value());
  REQUIRE(x2.value() == 10.0);

  auto x2p5 = equation.calculate(2.5);
  REQUIRE(!x2p5.has_value());

  auto x4 = equation.calculate(4.0);
  REQUIRE(x4.has_value());
  REQUIRE(x4.value() == 32.0);

  auto x5 = equation.calculate(5.0);
  REQUIRE(x5.has_value());
  REQUIRE(x5.value() == 42.0);
}

TEST_CASE("Missing Numerator or Denominator Set Correctly", "[json_equation]") {
  ifstream infile("../test/missing_numerator_denominator.json");
  JSONEquation equation(infile);

  // If numerator XOR denominator missing, set missing to 1.
  auto x0 = equation.calculate(0);
  REQUIRE(x0.has_value());
  REQUIRE(x0.value() == 5.0);

  auto x1 = equation.calculate(1.0);
  REQUIRE(x1.has_value());
  REQUIRE(x1.value() == -0.5);

  // If numerator AND denominator missing, set to 0.
  auto x2 = equation.calculate(2.0);
  REQUIRE(x2.has_value());
  REQUIRE(x2.value() == 0.0);
}

TEST_CASE("Missing Inclusive Attribute is set to True by Default", "[json_equation]") {
  ifstream infile("../test/missing_inclusive_attr.json");
  JSONEquation equation(infile);

  auto x0 = equation.calculate(0);
  REQUIRE(x0.has_value());
  REQUIRE(x0.value() == 5.0);

  auto x1 = equation.calculate(1);
  REQUIRE(x1.has_value());
  REQUIRE(x1.value() == 15.0);
}

TEST_CASE("Missing Lower Bound is an Error", "[json_equation]") {
  ifstream infile("../test/missing_lb.json");
  REQUIRE_THROWS_AS(JSONEquation(infile), std::runtime_error);
}

TEST_CASE("Missing Upper Bound is an Error", "[json_equation]") {
  ifstream infile("../test/missing_ub.json");
  REQUIRE_THROWS_AS(JSONEquation(infile), std::runtime_error);
}

TEST_CASE("len(powers) != len(coefficients) is an Error", "[json_equation]") {
  ifstream infile("../test/mismatch_power_coeff_len.json");
  REQUIRE_THROWS_AS(JSONEquation(infile), std::runtime_error);
}

TEST_CASE("No \"pieces\" Key in JSON is an Error", "[json_equation]") {
  ifstream infile("../test/missing_pieces_key.json");
  REQUIRE_THROWS_AS(JSONEquation(infile), std::runtime_error);
}

TEST_CASE("Overlapping Bounds Across Pieces is an Error", "[json_equation]") {
  ifstream infile("../test/overlapping_pieces.json");
  REQUIRE_THROWS_AS(JSONEquation(infile), std::runtime_error);
}

TEST_CASE("JSONEquation Swap Operation", "[json_equation]") {
  ifstream single("../test/single_piece.json");
  JSONEquation equation1(single);

  ifstream multiple("../test/multiple_pieces.json");
  json multiple_json;
  multiple >> multiple_json;
  JSONEquation equation2(multiple_json);

  auto preswap_e1x0 = equation1(0);
  REQUIRE(preswap_e1x0.has_value());
  REQUIRE(preswap_e1x0.value() == 5.0);

  auto preswap_e2x0 = equation2(0);
  REQUIRE(preswap_e2x0.has_value());
  REQUIRE(preswap_e2x0.value() == 2.0);

  // Swap!
  swap(equation1, equation2);

  auto postswap_e1x0 = equation1(0);
  REQUIRE(postswap_e1x0.has_value());
  REQUIRE(postswap_e1x0.value() == 2.0);

  auto postswap_e2x0 = equation2(0);
  REQUIRE(postswap_e2x0.has_value());
  REQUIRE(postswap_e2x0.value() == 5.0);
}

