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
#/   --help, -h         displays this help
#/   --file, -f         file size to benchmark
#/   --separator, -s    prints a line across the screen at the end
#/   --thread, -t       thread number
#/   --interactive, -i  waiting animation
usage() {
  grep '^#/' "$0" | cut -c4-
  exit 0
}

readonly tmp="/tmp/$(basename "$0").tmp"
readonly workdir="$(dirname "${BASH_SOURCE[0]}")"
readonly original="$(mktemp)"
readonly encoded="$(mktemp)"
readonly decoded="$(mktemp)"
readonly binary="$workdir"/codec
readonly key="$workdir"/key.txt

hr() {
  local start=$'\e(0' end=$'\e(B' line='qqqqqqqqqqqqqqqq'
  local cols=${COLUMNS:-$(tput cols)}
  while ((${#line} < cols)); do line+="$line"; done
  printf '%s%s%s\n' "$start" "${line:0:cols}" "$end"
}

declare -g thread="4"
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
      "-s"|"--separator")
        separator=true
        ;;
      "-i"|"--interactive")
        is=true
        ;;
      "-t"|"--thread")
        thread="${2:-4}"
        ;;
    esac
    shift
  done
}

readonly spin='-\|/'

spin() {
  local pid
  local msg
  local i
  pid=$1
  msg="$2"
  i=0
  while kill -0 "$pid" 2>/dev/null; do
    i=$(( (i+1) %4 ))
    printf "\\r%s [%s]" "$msg" "${spin:$i:1}"
    sleep .1
  done
  printf "\\r%s     \\n" "$msg"
}

encoding() {
  local pid
  if [[ "$is" != true ]]; then
    echo "Encoding..."
  fi
  {
    time {
      "$binary" -e "$original" "$encoded" -k "$key" -t "$thread"
    }
  } &> "$tmp" &
  pid=$!
  if [[ "$is" = true ]]; then
    spin "$pid" "Encoding..."
  fi
  wait "$pid"
  echo " › $(grep real "$tmp" | cut -f2)"
}

decoding() {
  local pid
  if [[ "$is" != true ]]; then
    echo "Decoding..."
  fi
  {
    time {
      "$binary" -d "$encoded" "$decoded" -k "$key" -t "$thread"
    }
  } &> "$tmp" &
  pid=$!
  if [[ "$is" = true ]]; then
    spin "$pid" "Decoding..."
  fi
  wait "$pid"
  echo " › $(grep real "$tmp" | cut -f2)"
}

main() {
  local pid
  local msg
  if (( size > 1023 )); then
    msg="Making a $(python3 -c "print(round($size/1024, 2))") Go file..."
  else
    msg="Making a $size Mo file..."
  fi
  if [[ "$is" != true ]]; then
    echo "$msg"
  fi
  dd if=/dev/urandom of="$original"  bs=1M  count="$size" > /dev/null 2>&1 &
  pid=$!
  if [[ "$is" = true ]]; then
    spin "$pid" "$msg"
  fi
  wait "$pid"
  echo " › Done"

  echo "Checksum original file"
  md5sum "$original"
  encoding
  rm "$original"
  decoding
  echo "Checksum decoded file"
  md5sum "$decoded"
  rm "$encoded"
  rm "$decoded"

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

rm "$tmp"
