#!/usr/bin/env bash
#/ Script Name: bench.sh
#/ Author: jRimbault
#/ Date:   2017-06-30
#/
#/ Description:
#/   Benchmarking script for the codec project.
#/   It will make a file of random bytes and then try to use
#/   the codec to time the encode and the decode of this file.
#/   Then it will compare the signature of the 3 resulting files.
#/
#/ Run Information:
#/   This script is called by the Makefile target `bench`
#/
#/ Usage: bench.sh -f <size in Mo>
#/
#/ Options:
#/   --help, -h displays this help
#/   --file, -f file size to benchmark
#/   --sep, -s  prints a line across the screen at the end
usage() {
  grep '^#/' "$0" | cut -c4-
  exit 0
}

readonly tmp="/tmp/$(basename "$0").tmp"
readonly workdir="$(dirname "${BASH_SOURCE[0]}")"
readonly original="$workdir"/build/original
readonly encoded="$workdir"/build/encoded
readonly decoded="$workdir"/build/decoded
readonly binary="$workdir"/codec
readonly key="$workdir"/build/key.txt

hr() {
  local start=$'\e(0' end=$'\e(B' line='qqqqqqqqqqqqqqqq'
  local cols=${COLUMNS:-$(tput cols)}
  while ((${#line} < cols)); do line+="$line"; done
  printf '%s%s%s\n' "$start" "${line:0:cols}" "$end"
}

parse_args() {
  size=64
  while [[ $# -gt 0 ]]; do
    case "$1" in
      "-h"|"--help")
        usage
        ;;
      "-f"|"--file")
        size="$2"
        ;;
      "-s"|"--sep")
        separator=true
        ;;
    esac
    shift
  done
}

encoding() {
  echo "Encoding..."
  {
    time {
      "$binary" -e "$original" "$encoded" -k "$key"
    }
  } &> "$tmp"
  echo " › $(grep real "$tmp" | cut -f2)"
}

decoding() {
  echo "Decoding..."
  {
    time {
      "$binary" -d "$encoded" "$decoded" -k "$key"
    }
  } &> "$tmp"
  echo " › $(grep real "$tmp" | cut -f2)"
}

main() {
  if (( size > 1023 )); then
    echo "Making a $(python -c "print(round($size/1024, 2))") Go file..."
  else
    echo "Making a $size Mo file..."
  fi
  dd if=/dev/urandom of="$original"  bs=1M  count="$size" > /dev/null 2>&1
  echo " › Done"

  encoding
  decoding

  echo "Comparing md5sum signature:"
  md5sum "$original" "$encoded" "$decoded"
  echo "Comparing file size:"
  du -b "$original" "$decoded"
  if [[ "$separator" = true ]]; then
    hr
  fi
}

# if binary hasn't been compiled yet
if [[ ! -f "$binary" ]]; then
  echo "$binary not found"
  echo " › execute \`make\`"
  exit 1
fi
# executes only when executed directly not sourced
if [[ "${BASH_SOURCE[0]}" = "$0" ]]; then
  parse_args "$@"
  main
fi
