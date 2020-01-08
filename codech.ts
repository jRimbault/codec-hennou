#!/usr/bin/env node

import fs = require('fs')
import process = require('process')

type Args = ReturnType<typeof parseArgs>
type Codec = (buffer: Buffer) => Buffer
type Matrix = ReturnType<typeof getMatrix>

async function main(args: Args): Promise<number> {
  const matrix = getMatrix(args.keyfile)
  if (args.encode) {
    return work(
      buffer => encode(matrix.encode, buffer),
      args.source,
      args.dest
    )
  }
  if (args.decode) {
    return work(
      buffer => decode(matrix.decode, buffer),
      args.source,
      args.dest
    )
  }
  return 1
}

function work(codec: Codec, source: string, dest: string): Promise<number> {
  return new Promise(resolve => {
    fs.readFile(source, (err, buffer) => {
      if (err !== null) {
        console.error(err)
        return resolve(err.errno)
      }
      fs.writeFile(dest, codec(buffer), () => resolve(0))
    })
  })
}

function encode(matrix: Matrix['encode'], original: Buffer): Buffer {
  const MASK = 0x0F
  const encoded = Buffer.from(new Uint8Array(original.byteLength * 2))
  for (let i = 0, j = 0; i < original.byteLength; i += 1, j += 2) {
    const o = original.readUInt8(i)
    encoded.writeUInt8(matrix[o & MASK], j)
    encoded.writeUInt8(matrix[(o >> 4) & MASK], j + 1)
  }
  return encoded
}

function decode(matrix: Matrix['decode'], encoded: Buffer): Buffer {
  const decoded = Buffer.from(new Uint8Array(encoded.byteLength / 2))
  for (let i = 0, j = 0; i < encoded.byteLength; i += 2, j += 1) {
    const a = encoded.readUInt8(i)
    const b = encoded.readUInt8(i + 1)
    const p1 = matrix[a]
    const p2 = matrix[b] << 4
    decoded.writeUInt8(p1 | p2, j)
  }
  return decoded
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
  matrix[15] = 0
  const reverse: number[] = Array.from({ length: 256 })
  reverse.fill(0)
  for (let i = 0; i < matrix.length; i += 1) {
    reverse[matrix[i]] = i
  }
  return {
    encode: matrix as readonly number[],
    decode: reverse as readonly number[],
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
