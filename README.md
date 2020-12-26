# json-equation
A library that reads a polynomial formatted as a JSON object. 
Pretty much based on a dream I had so we can stop hard-coding 
polynomial curves in code. Uses nlohmann::json for parsing input.

## Why JSON?
Because I didn't want to write an entire parsing module by
myself again. Imagine all the overhead that comes with handling 
both good and bad inputs. :(
