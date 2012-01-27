#!/bin/sh

bin=$(dirname "$0")

sec=$1
if [ "$sec" = "" ]
then
    echo "Provide a section number!" >&2
    exit 1
fi

printf "Creating 'allnames' and 'names' in sec%s/\n" "$sec"
mkdir -p "sec$sec"

cp /classes/cs1121/grades/cs1121.master ./
ssh wopr.csl getent passwd > passwd

printf '.*\nsec%s/allnames' "$sec" | "$bin/format-names.lace" >/dev/null
printf '%d\nsec%s/names' "$sec" "$sec" | "$bin/format-names.lace" >/dev/null

