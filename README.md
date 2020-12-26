# json-equation
A library that reads a polynomial formatted as a JSON object. 
Pretty much based on a dream I had so we can stop hard-coding 
polynomial curves in code. Uses nlohmann::json for parsing input.

## Example
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

## Why JSON?
Because I didn't want to write an entire parsing module by
myself again. Imagine all the overhead that comes with handling 
both good and bad inputs. :(
