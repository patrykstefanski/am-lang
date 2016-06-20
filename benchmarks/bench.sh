#!/bin/sh

SRCDIR=$(dirname $0)
TMPDIR=$(mktemp -d)

run_am() {
    echo "--------"
    echo "$1 - am-lang"
    time am-lang "$SRCDIR/$1.am"
}

run_c() {
    echo "--------"
    echo "$1 - C"
    gcc -O3 -Wall -Wextra "$SRCDIR/$1.c" -o "$TMPDIR/$1"
    time "$TMPDIR/$1"
}

run_java() {
    echo "--------"
    echo "$1 - Java"
    javac -d $TMPDIR "$SRCDIR/$1.java"
    time java -cp "$TMPDIR" $1
}

run_php() {
    echo "--------"
    echo "$1 - PHP"
    time php -f "$SRCDIR/$1.php"
}

run_python() {
    echo "--------"
    echo "$1 - Python"
    time python "$SRCDIR/$1.py"
}

for t in ackermann fibonacci prime; do
    run_am $t
    run_c $t
    run_java $t
    run_php $t
    run_python $t
done;
