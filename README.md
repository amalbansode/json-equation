# json-equation
A library that reads a polynomial formatted as a JSON object. 
Pretty much based on a dream I had so we can stop hard-coding 
polynomial curves in code. Uses nlohmann::json for parsing input.

## Code Example
```c++
// Load a JSON file that contains the equation following the required schema.
ifstream infile("function_cost.json"); 

// Define a function 'f_cost' which takes in a single variable, say, quantity
Equation f_cost(infile);

// Define a value for the input variable
int quantity = 10;

// Find the cost given quantity 10. Mathematically, we'd write this as f_cost(10).
double cost = f_cost.calculate(10);
```

## Equation Example
The JSON input file contains an array of piecewise functions
that follow a schema like this. This may expand in the future
to accommodate for more features, but this simple version 
should be usable across a multitude of applications too.

```json
{
  "pieces": [
    {
      "lower_bound": 2.0,
      "lb_inclusive": true,
      "upper_bound": 8.0,
      "ub_inclusive": true,
      "numerator": {
        "powers": [0, 2, 5],
        "coefficients": [0.03, -93, 2]
      },
      "denominator": {
        "powers": [0, 1],
        "coefficients": [90, -43]
      }
    }
  ]
}
```

## Why JSON?
Because I didn't want to write an entire parsing module by
myself again. Imagine all the overhead that comes with handling 
both good and bad inputs. :(
