#!/bin/sh

bin=$(readlink -f $(dirname "$0"))
dir=$PWD

configf="labconf.lace"
if [ ! -f "$configf" ]
then
    echo "Get into a directory with labconf.lace!" >&1
    exit 1
fi

sec=`grep '$(H: \+section)' "$configf" | grep -o '[[:digit:]]\+'`
lab=`grep '$(H: \+lab)' "$configf" | grep -o '[[:digit:]]\+'`

# Accept lab.
"$bin/accept.fildesh"
# Generate pairs file and some other stuff.
"$bin/pairgen.fildesh"
# Go to the appropriate directory.
"$bin/pairgen.fildesh"

cd "$HOME/cs1121.grading/Lab$lab/sec$sec"

# Find out who was paired up.
"$bin/pairup.fildesh" > "$dir/lab$lab"
if [ ! -e "lab$lab" ]
then
    ln -s "$dir/lab$lab" ./
fi

cat <<EOF
File ready for grades: lab$lab
Tips:
- Grades will be destroyed if you run this command.
- Check $HOME/cs1121.grading/Lab$lab/sec$sec
- Verify the pairup-matches file has equivalent names on left and right.
- Check the wrong-section file for bad submittions.
EOF


