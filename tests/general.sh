#!/bin/bash

# General definitions testing BASH scripts can use.

ASSERT_FAILS=0

RED="\033[1;31m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
NC="\033[0m"

print_h1() {
    echo
    echo -e "${YELLOW}=====" $1 "=====${NC}"
}

print_h2() {    
    echo
    echo -e "${YELLOW}-----" $1 "-----${NC}"
}

print_info() {
    echo "      $1"
}

print_ok() {
    echo -e -n "${GREEN}OK${NC}    "
}

print_fail() {
    echo -e -n "${RED}FAIL${NC}  "
}

assert_equal() { # val1, val2, test_description
    res=0

    if [ $1 = $2 ]; then
       print_ok
       res=0
    else
       print_fail
       ((ASSERT_FAILS+=1))
       res=1
    fi

    echo $3 "($1 == $2)"

    return $res
}

assert_command() { # command_str
    res=0

    eval $1 > /dev/null 2> /dev/null

    if [ $? -eq 0 ]; then
       print_ok
       res=0
    else
       print_fail
       ((ASSERT_FAILS+=1))
       res=1
    fi

    echo "asserting command ($1)"

    return $res
}

print_summary() {
    res=0

    echo
    echo -e "${YELLOW}~~~~~ test summary ~~~~~${NC}"

    echo -n "asserts failed: "

    if [ $ASSERT_FAILS = 0 ]; then
        echo -e "${GREEN}$ASSERT_FAILS${NC}"
        res=0
    else
        echo -e "${RED}$ASSERT_FAILS${NC}"
        res=1
    fi

    echo

    return $res
}
