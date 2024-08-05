# Sxpb Style Guide

## Syntax

### Value
```
; Fields begin and end with parentheses.
(my_empty_dict)
; Booleans always begin with a plus.
(my_bool +false)
; Integers can also begin with a minus or digit.
(my_int -5)
; Floating point numbers can also begin with a period.
(my_float .6e+1)
; Strings begin with any other non-whitespace character
; like in YAML but less ambiguous.
; Unquoted segments are joined with a space.
(my_string a b " c")
; Field names can be quoted like in JSON.
; For consistency, all strings and field names inside
; a quoted field's scope must be quoted as well.
("my_quoted_string" "a b c")
```

See [https://grencez.dev/2024/sxpb-string-grammar-20240717#bare](https://grencez.dev/2024/sxpb-string-grammar-20240717#bare) for a precise explanation of when double quotes can be omitted from strings.
The grammar can also be found at [eg/sxpb_string/grammar.sxpb](../../eg/sxpb_string/grammar.sxpb) in Sxpb format.
For a small example of valid edge cases, see [eg/ansible/motd.sxpb](../../eg/ansible/motd.sxpb).

### Repetition
```
; Arrays begin with empty nested parentheses.
(my_ints (()) 1 2 3)
(my_strings (()) a b c)
(my_dicts (())
 (() (i 1) (f 2.0) (s three))
 ()  ; Empty.
 (() (i 4) (f 5.0) (s six))
)
(my_empty_array (()))
```

### Nesting
```
; Dictionaries have uniquely-named fields.
(my_map () (x a) (y b) (z c))
; Messages are dictionaries that conform to a schema.
; Think of them like objects conforming to a class.
(my_msg (x a) (y b) (z c))
; You can always write a message as a dict.
(my_empty_msg)

; Arrays are lists of elements.
(my_array (()) a b c)
; Sets are unordered arrays.
; You can always write a set as an array (and vice-versa).
(my_set () a b c)
```

### Extra
```
(fruit_map ()
 (breakfast banana)
 (snack apple)
 (dessert cherry)
)
(my_array (()) apple banana cherry)
(my_manyof (()) (breakfast apple) (breakfast banana) (breakfast cherry))
```
