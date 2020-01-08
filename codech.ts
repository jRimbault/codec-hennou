#!/usr/bin/env node

import fs = require('fs')
import process = require('process')

type Codec = (stream: Buffer) => Uint8Array

async function main(args: ReturnType<typeof parseArgs>): Promise<number> {
  const matrix = getMatrix(args.keyfile)
  if (args.encode) {
    return work(
      stream => encode(matrix.encode, stream),
      args.source,
      args.dest
    )
  }
  if (args.decode) {
    return work(
      stream => decode(matrix.decode, stream),
      args.source,
      args.dest
    )
  }
  return 1
}

function work(codec: Codec, source: string, dest: string): Promise<number> {
  return new Promise(resolve => {
    fs.readFile(source, (_, buffer) => {
      fs.writeFile(dest, codec(buffer), () => resolve(0))
    })
  })
}

function encode(matrix: number[], stream: Buffer): Uint8Array {
  const MASK = 0x0F
  const array = new Uint8Array(stream.byteLength * 2);
  for (let i = 0; i < stream.length; i += 1) {
    const o = stream.readUInt8(i)
    array.set([
      matrix[o & MASK],
      matrix[(o >> 4) & MASK],
    ], i * 2)
  }
  return array
}

function decode(matrix: Record<number, number>, stream: Buffer): Uint8Array {
  const array = new Uint8Array(stream.byteLength / 2);
  for (let i = 0; i < stream.length / 2; i += 1) {
    const a = stream.readUInt8(i * 2)
    const b = stream.readUInt8(i * 2 + 1)
    const p1 = matrix[a]
    const p2 = matrix[b] << 4
    array.set([p1 | p2], i)
  }
  return array
}

function getMatrix(filename: string) {
  const key = fs.readFileSync(filename)
    .slice(5, 40)
    .toString()
    .split(' ')
    .map(b => parseInt(b, 2))
  const matrix: number[] = Array.from({ length: 16 })
  matrix[0] = key[0] ^ key[1] ^ key[2] ^ key[3]
  matrix[1] = key[3]
  matrix[2] = key[2]
  matrix[3] = key[2] ^ key[3]
  matrix[4] = key[1]
  matrix[5] = key[1] ^ key[3]
  matrix[6] = key[1] ^ key[2]
  matrix[7] = key[1] ^ key[2] ^ key[3]
  matrix[8] = key[0]
  matrix[9] = key[0] ^ key[3]
  matrix[10] = key[0] ^ key[2]
  matrix[11] = key[0] ^ key[2] ^ key[3]
  matrix[12] = key[0] ^ key[1]
  matrix[13] = key[0] ^ key[1] ^ key[3]
  matrix[14] = key[0] ^ key[1] ^ key[2]
  return {
    encode: matrix,
    decode: Object.fromEntries((matrix).map((e, i) => [e, i])) as Record<number, number>
  } as const
}

function parseArgs(argv: typeof process.argv) {
  return {
    keyfile: argv[2],
    action: argv[3],
    encode: argv[3] === '-e' || argv[3] === '--encode',
    decode: argv[3] === '-d' || argv[3] === '--decode',
    source: argv[4],
    dest: argv[5],
  } as const
}

main(parseArgs(process.argv)).then(n => process.exit(n))
