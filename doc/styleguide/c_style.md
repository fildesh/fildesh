# C Programming Style Guide

Use the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) as a starting point.
This guide is formatted similarly.

## C Version
C code must conform to the syntax of ANSI C89 and ISO C90, meaning that it must compile with GCC's `-ansi` flag without warnings.
Newer features can be provided behind `#ifdef`s.

Other assumptions:
- `char` is assumed to be an 8-bit byte.
  - Use `CHAR_BIT` in place of the literal number 8 when possible.

## Scoping

### Local Variables
```c
unsigned f(unsigned n)
{
  /* All variables must be declared at the start of a block.*/
  unsigned i;
  /* Initialize only when the value will be used.*/
  unsigned x = 0;
  for (i = 0; i < n; ++i) {
    x += i;
  }
  return x;
}
```

## Comments

### Comment Style
Use the `/* Inline comment style.*/` exclusively.

## Formatting

### Conditionals

```c
if (x == y) {
  /* Opening brace goes on same line of short if statement.*/
}
else if (x < y) {
  /* Closing brace gets its own line.*/
  /* Else should be aligned with the previous if.*/
}
else if (x+1 == y &&
         (really_long_condition_part_1(x,y) ||
          really_long_condition_part_2(x,y)))
{
  /* Opening brace can be on its own line  else.*/
}
else {
  /* Last thing.*/
}
```

```c
/* All if-statements use braces.
 * 1-liners are okay, especially for error conditions that won't be tested
 * (and therefore won't count towards code coverage).
 */
if (x == y) {return 3;}
```

### Pointer and Reference Expressions

```c
/* Place the * next to the type name in varible declarations,
 * because the pointer is part of the variable's type.
 */
FildeshO* out = open_FildeshOF("file.txt");

/* Place const qualifiers before the type name,
 * because the const is guarding the data rather than the address.
 */
const char* arg = argv[1];

/* Sometimes neither of the above is possible. Just do your best.*/
const char* const* const args = (const char**)(void*)argv;
```

