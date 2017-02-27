#!/bin/bash

NC='\033[0m' # No Color
BLACK='\033[0;30m'

GREY='\033[1;30m'
LIGHT_GREY='\033[0;37m'
RED='\033[0;31m'
LIGHT_RED='\033[1;31m'
GREEN='\033[0;32m'
LIGHT_GREEN='\033[1;32m'
ORANGE='\033[0;33m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
LIGHT_BLUE='\033[1;34m'
PURPLE='\033[0;35m'
LIGHT_PURPLE='\033[1;35m'
CYAN='\033[0;36m'
LIGHT_CYAN='\033[1;36m'
WHITE='\033[1;37m'

g_help() {
	echo -e "This is a ${LIGHT_RED}wrapper script${NC} around ${GREEN}'codec'${NC}, the main program."
	echo -e "It will measure the encode and decode time of the program,"
	echo -e "given the original file, a name for the encoded file,"
	echo -e "a name for the decoded file, and a key file."
	echo -e ""
	echo -e "It will then compare the original file and decoded file."
	echo -e ""
	echo -e "Real = human elapsed time"
	echo -e "User = user space cpu elasped time"
	echo -e "Sys  = kernel mode cpu elapsed time"
	echo -e ""
}

encoding() {
	echo -e "${YELLOW}Encoding${NC} $1 into $2..."
	time {
		codec -e $1 $2 -k $4
	}
}

decoding() {
	echo -e "${YELLOW}Decoding${NC} $2 into $3..."
	time {
		codec -d $2 $3 -k $4
	}
}

cmparing() {
	echo -e "${YELLOW}Comparing${NC} $1 and $3..."
	if cmp "$1" "$3" >/dev/null ; then
		echo -e "${LIGHT_BLUE}$1 and $3 are the same!${NC}"
	else
		echo -e "${RED}$1 and $3 are different, something went wrong.${NC}"
	fi
	echo -e "${DARK_GREY}sha1sums to be sure:"
	sha1sum "$1"
	sha1sum "$3"
}

main() {
	g_help
	echo -e "Encoding using key $4"
	cat "$4"
	echo -e ""
	echo -e ""
	encoding "$@"
	decoding "$@"
	cmparing "$@"
}

main "$@"

