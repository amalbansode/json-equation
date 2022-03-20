# JSON Equation
A header-only C++ library that enables the representation and computation of a system of piecewise polynomial equations (of a single variable) in JSON.

Pretty much based on a dream I had so we can stop hard-coding polynomial curves in code. 

## Dependencies

This library depends on:

- [`nlohmann::json`](https://github.com/nlohmann/json) for JSON deserialization
- [`numeric_range`](https://github.com/amalbansode/numeric-range) for sorting, validating, and indexing pieces' bounds

## Code Example
```c++
// Load a JSON file that contains the equation following the required schema.
ifstream infile("really_cool_system.json");
JSONEquation really_cool_system(infile);

// Calculate the output value with input x = 0
auto f0 = really_cool_system.calculate(0);

if (f0.has_value())
  std::cout << f0.value();
else
  std::cout << "0 not included in any pieces' ranges" << std::endl;

// Or use the shorthand operator(), equivalent to calling calculate()
auto f1 = really_cool_system(1);
```

## Equation Schema
The JSON input file contains an array of piecewise functions
that follow a schema like below. This may expand in the future
to accommodate for more features, but this simple version 
should be useful across a variety of applications too.

```json
{
  "pieces": [
    {
      "lower_bound": 0.0,
      "lb_inclusive": true,
      "upper_bound": 1.0,
      "ub_inclusive": false,
      "numerator": {
        "powers": [0, 1],
        "coefficients": [5, 10]
      },
      "denominator": {
        "powers": [0],
        "coefficients": [2]
      }
    }
  ]
}
```

## Limitations

There are some limitations (for the sake of correctness) on how pieces' bounds may be specified and handled.
Please read the [`numeric_range` documentation](https://github.com/amalbansode/numeric-range) for details.

Any such correctness violations are typically caught while constructing the `JSONEquation` object and throw an `std::runtime_error`.

If an input value is not contained in any pieces' bounds, the `calculate()` function returns `std::nullopt`.

## Why JSON?
Because I didn't want to write an entire parsing module by myself again. Imagine all the overhead that comes with handling both good and bad inputs. :(

## License and Contributing

This library is provided under the MIT License. Contributions are welcome, please open a relevant issue or PR.

