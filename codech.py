#!/usr/bin/env python3

import argparse
import itertools
import math
import multiprocessing


def main(args):
    matrix, dic_matrix = get_matrix(args.keyfile)
    if args.encode:
        return work(Worker(matrix, encode), args.source, args.dest)
    if args.decode:
        return work(Worker(dic_matrix, decode), args.source, args.dest)

    return 1


def work(worker, source, dest):
    with open(source, "rb") as stream:
        # single thread is actually better under a certain amount
        # result = worker(stream.read())
        with multiprocessing.Pool(num_workers()) as pool:
            result = flatten(pool.map(worker, stream))

    with open(dest, "wb") as out:
        out.write(bytearray(result))

    return 0


class Worker:
    def __init__(self, matrix, codec):
        self.matrix = matrix
        self.codec = codec

    def __call__(self, stream):
        return self.codec(self.matrix, stream)


def encode(matrix, stream):
    def split_char(ch):
        yield matrix[ch & 0x0F]
        yield matrix[(ch >> 4) & 0x0F]

    return [el for el in flatten(split_char(ch) for ch in stream)]


def decode(dic_matrix, stream):
    def join_chars(chars):
        return dic_matrix[chars[0]] | (dic_matrix[chars[1]] << 4)

    return [join_chars(chars) for chars in chunk(stream, 2)]


def get_matrix(filename):
    with open(filename) as stream:
        key = [int(c, 2) for c in stream.read()[5:40].split(" ")]
    matrix = [0 for _ in range(16)]
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
    return matrix, {e: i for i, e in enumerate(matrix)}


def chunk(iterable, n):
    it = iter(iterable)
    while True:
        chunk = tuple(itertools.islice(it, n))
        if not chunk:
            return
        yield chunk


def flatten(iterable):
    yield from (el for sub in iterable for el in sub)


def num_workers():
    return 2 ** int(math.log2(multiprocessing.cpu_count()))


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("keyfile")
    parser.add_argument("source")
    parser.add_argument("dest")
    group = parser.add_argument_group()
    group.add_argument("-e", "--encode", action="store_true")
    group.add_argument("-d", "--decode", action="store_true")
    return parser.parse_args()


if __name__ == "__main__":
    exit(main(parse_args()))
