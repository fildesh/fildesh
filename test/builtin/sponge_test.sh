#!/bin/sh

sponge="$1"

sponge_inherited_delay() {
  {
    {
      echo "b" |
      tee /dev/fd/3 |
      {
        sleep 1
        sed -e 's/b/a/' >&4
      }
    } 3>&1 | {
      # Sponge eats up the tee'd text and waits for the above commands to exit
      # because the `sleep` block hasn't closed its stdout.
      "$sponge" /dev/fd/4
    }
  } 4>&1
}

expect=$(printf 'a\nb\n')
result=$(sponge_inherited_delay)
if [ "$expect" != "$result" ]; then
  printf 'Expected (after newline):\n%s\nGot (after newline):\n%s\n' "$expect" "$result" >&2
  exit 1
fi
exit 0
