A bomb compiler that targets the 8086 assembly language.
Mostly here to learn about compilers / parsers / lexers / code generation.

Currently only supports the integer (16-bit) datatype.
Support for arrays is hopefully coming soon.

# Compiling
Run `make` to compile the program and create the executable file `bomb-gen`.

Then pipe your source file into `bomb-gen`. Example:
```
./bomb-gen < testing.bmb
```

# Bomb Program to Compute Fibonacci
```  
fib(8)

def fib(n)
    if n == 1 or n == 2
        1
    else
        fib(n - 1) + fib(n - 2)
    end
end
```
