mod args;
mod build_info;
mod codec;
mod matrix;

use args::{parse_args, Argument};
use codec::{decode, encode};
use matrix::get_matrix;

use clap::ArgMatches;
use exitcode;
use rayon::prelude::*;
use std::env;
use std::fs;
use std::fs::File;
use std::io;
use std::io::prelude::*;
use std::process;

fn main() {
    process::exit(match run(parse_args(env::args())) {
        None => exitcode::OK,
        Some(error) => {
            println!("{}", error);
            error.raw_os_error().unwrap_or(1)
        }
    })
}

fn run(args: ArgMatches<'static>) -> Option<io::Error> {
    use Argument::*;
    let matrix = get_matrix(args.value_of(KeyFile).unwrap());
    if matrix.is_err() {
        return matrix.err();
    }
    let (encode_m, decode_m) = matrix.unwrap();
    let source = args.value_of(Source).unwrap();
    let dest = args.value_of(Dest).unwrap();
    if args.is_present(Encode) {
        io_codec(|stream| encode(encode_m, stream), source, dest).err()
    } else {
        io_codec(|stream| decode(decode_m, stream), source, dest).err()
    }
}

fn io_codec<Worker>(task: Worker, source: &str, dest: &str) -> io::Result<()>
where
    Worker: Fn(&[u8]) -> Vec<u8>,
    Worker: Send + Sync,
{
    File::create(dest)?.write_all(&codec(task, fs::read(source)?, num_cpus::get()))
}

fn codec<Worker>(task: Worker, stream: Vec<u8>, num_workers: usize) -> Vec<u8>
where
    Worker: Fn(&[u8]) -> Vec<u8>,
    Worker: Send + Sync,
{
    let size = chunk_size(stream.len(), num_workers);
    chunked_parallel_map(task, stream, size)
}

fn chunked_parallel_map<T, Mapper>(mapper: Mapper, list: Vec<T>, chunk_size: usize) -> Vec<T>
where
    T: Send + Sync,
    Mapper: Fn(&[T]) -> Vec<T>,
    Mapper: Send + Sync,
{
    /// rayon's flattenning methods handles many cases of no interest here,
    /// this is better for the particular case of a few large collections
    ///
    /// `flat_map` and `flatten` are _very_ slow
    fn flatten<T>(collections: Vec<Vec<T>>) -> Vec<T> {
        let mut result = Vec::new();
        for col in collections {
            result.extend(col);
        }
        result
    }
    flatten(list.par_chunks(chunk_size).map(mapper).collect())
}

/// chunk size should always be even for the decoding phase
fn chunk_size(stream_len: usize, available_max_workers: usize) -> usize {
    const MASK: usize = std::usize::MAX - 1;
    match stream_len.checked_div(available_max_workers) {
        Some(chunk_size) => {
            if chunk_size & MASK == 0 {
                stream_len
            } else {
                chunk_size & MASK
            }
        }
        None => stream_len, // only one chunk, always even for encoded files
    }
}

#[cfg(test)]
mod main_tests {
    use super::*;
    use matrix::get_reverse_matrix;
    use matrix::Matrix;
    use rstest::rstest_parametrize;

    static MATRIX: Matrix = [
        126, 146, 164, 54, 199, 85, 99, 241, 143, 29, 43, 185, 72, 218, 236, 0,
    ];

    fn random(n: usize) -> Vec<u8> {
        const MAX_CHUNK: usize = 32;
        (0..(n / MAX_CHUNK) + 1)
            .map(|_| rand::random::<[u8; MAX_CHUNK]>().to_vec())
            .flatten()
            .collect::<Vec<u8>>()[..n]
            .to_vec()
    }

    #[rstest_parametrize(
        n,
        case(1),
        case(2),
        case(3),
        case(4),
        case(5),
        case(6),
        case(7),
        case(8),
        case(9),
        case(10),
        case(11),
        case(12),
        case(13),
        case(14),
        case(15),
        case(16),
        case(32),
        case(63),
        case(64)
    )]
    fn should_test_parallelization(n: usize) {
        let original = random(59577);
        assert_eq!(original.len(), 59577);
        let encoded = codec(|s| encode(MATRIX, s), original.to_vec(), n);
        assert!(encoded.len() % 2 == 0);
        let reverse = get_reverse_matrix(MATRIX);
        let decoded = codec(|s| decode(reverse, s), encoded, n);
        assert_eq!(original, decoded);
    }
}
