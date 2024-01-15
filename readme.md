# Lexscript
Strongly and static typed language based on python.
Evolved from lexscript.py for increased efficiency and speed.

# Example of Code
Code:
```julia
# Variables
string hi = "Hello World"
print(hi)

bool boolean = 1 > 2
print("Boolean variable value:", boolean, conclude=" ")

var num = 2 * 2 if boolean else -1 ^ 2
print("\nNum variable value: ", num)

# WHILE STATEMENT
var test = 1
while test < 4 then test = test + 1 end
print("Var test after while loop: ", test)

# FOR STATEMENT
var example = 0
for i to 4 then example = example + i end
print("Example value: ", example)

print("Drawing a 3x3 square:")
for i to 3 then
    for j to 3 then
        print("*", conclude=" ")
    end
    print("", conclude="\n")
end

# FUNCTIONS
function sum(a: var, b: var) -> a + b
print("Result of sum function: ", sum(example, test))

function mul(a: var, b: var) does
    var result = a * b
    return result
end

print("Result of mul function: ", mul(example, test))

function fibonacci_recursive(num : var) does
    if num > 1 then num = fibonacci_recursive(num - 1) + fibonacci_recursive(num - 2) end

    return num
end

print("Recursive fibonacci function: ", fibonacci_recursive(12))
```
Output:
```
Hello World
Boolean variable value: False 
Num variable value: 1
Var test after while loop: 4
Example value: 10
Drawing a 3x3 square:
* * * 
* * * 
* * * 
Result of sum function: 14
Result of mul function: 40
Recursive fibonacci function: 144

Execution time: 0.003275 seconds
```

# How to run a script
```bash
./lexscript <file>
```
example:
```bash
./lexscript tryme.lx
```

# How to install language support on Visual Studio Code
1. Open command menu on Visual Studio: `Ctrl+Shift+P` on Linux and Windows or `Cmd+Shift+P` on MacOS
2. Write Extensions: Install from VSIX
3. Select lexscript-language-sopport.vsix

# Variables
- FLOAT | INT -> var num = `value`
- BOOLEAN -> bool true_or_false = `value`
- STRING -> string hi = `"Hello World"`

# Bult-in Functions
- print()         -> Print all the arguments in a line
- clear()         -> clean the terminal
- exit()          -> close the program
- close()         -> close the program