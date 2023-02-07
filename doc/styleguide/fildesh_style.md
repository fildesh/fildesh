# Fildesh Script Style Guide

## Nameless stdio

It's great. Try to structure blocks of code as simple Unix pipelines.
Someone familiar with shell scripts will probably recognize that a bunch of consecutive lines starting with `|<`, `|-`, and `|>` are part of a pipeline, whereas `$(O ...)`, `$(XO ...)`, and `$(X ...)` syntax can easily be misinterpreted as being a variable expansion.

## Useless use of cat

UUOC will be optimized away as part of [issue 52](https://github.com/fildesh/fildesh/issues/52).
There's no need to avoid it if it improves readability.

```
|< cat $(XF data_from_earlier)
|- do some stuff
|> zec -o $(OF data_for_later)
```


