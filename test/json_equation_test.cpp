#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()

#include "catch.hpp"
#include "../src/json-equation.h"

using namespace std;

TEST_CASE( "Simple Equation Computation", "[equation]" ) {
  ifstream infile("../test/equation.json");
  Equation equation(infile);

  REQUIRE(equation.calculate(0) == 0.5);
}