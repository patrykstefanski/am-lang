#!/bin/sh

SRCDIR=$(dirname $0)
TMPDIR=$(mktemp -d)

CC=clang
TIME=/usr/bin/time # Use the time binary, not a shell builtin

usage() {
    echo "$0 [LANG] [TESTCASE] [NUM]"
    echo
    echo "Available languages: am-lang, c, java, java-int (for interpreted java), lua, luajit, php, python"
    echo "Available test cases: ackermann, fibonacci, prime"
    echo "Example:"
    echo "Run the ackermann test for am-lang 5 times"
    echo "$0 am-lang ackermann 5"
    exit 1
}

if [[ ! " am-lang c java java-int lua luajit php python " =~ " $1 " ]]; then
    usage
fi

if [[ ! " ackermann fibonacci prime " =~ " $2 " ]]; then
    usage
fi

if [[ -z "$3" ]]; then
    usage
fi

exe() {
    echo "$@"; "$@";
}

case $1 in
    "am-lang")
        CMD="am-lang $2.am"
        ;;
    "c")
        exe $CC -O3 -Wall -Wextra "$SRCDIR/$2.c" -o "$TMPDIR/$2"
        CMD="$TMPDIR/$2"
        ;;
    "java")
        exe javac -d $TMPDIR "$SRCDIR/$2.java"
        CMD="java -cp $TMPDIR $2"
        ;;
    "java-int")
        exe javac -d $TMPDIR "$SRCDIR/$2.java"
        CMD="java -Xint -cp $TMPDIR $2"
        ;;
    "lua")
        CMD="lua $SRCDIR/$2.lua"
        ;;
    "luajit")
        CMD="luajit $SRCDIR/$2.lua"
        ;;
    "php")
        CMD="php -f $SRCDIR/$2.php"
        ;;
    "python")
        CMD="python $SRCDIR/$2.py"
        ;;
esac

total=0
for i in $(seq 1 $3); do
    t=$($TIME -p -f "%e" 2>&1 $CMD 1>/dev/null)
    total=$(echo $total $t | awk '{ printf "%f", $1 + $2 }')
    avg=$(echo $total $i | awk '{ printf "%.02f", $1 / $2 }')
    if [[ -z "$min" ]] || [[ -z $(echo $t $min | awk '{ if ($1 > $2) print "dummy" }') ]]; then
        min=$t
    fi
    echo "$i/$3 cur=${t}s avg=${avg}s min=${min}s"
done;

rm -fr $TMPDIR
