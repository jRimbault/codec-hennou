use crate::matrix::Matrix;
use rayon::prelude::*;

pub struct Codec {
    matrix: Matrix,
    max_workers: usize,
}

impl Codec {
    pub fn new(matrix: Matrix, max_workers: usize) -> Self {
        Codec {
            matrix,
            max_workers,
        }
    }

    pub fn encode(&self, stream: Vec<u8>) -> Vec<Vec<u8>> {
        let size = chunk_size(stream.len(), self.max_workers);
        parallel_codec(|s| encode(self.matrix, s), stream, size)
    }

    pub fn decode(&self, stream: Vec<u8>) -> Vec<Vec<u8>> {
        let size = chunk_size(stream.len(), self.max_workers);
        parallel_codec(|s| decode(self.matrix, s), stream, size)
    }
}

fn encode(matrix: Matrix, stream: &[u8]) -> Vec<u8> {
    let mut encoded = Vec::with_capacity(stream.len() * 2);
    for &byte in stream {
        let [byte0, byte1] = matrix.encode(byte);
        encoded.push(byte0);
        encoded.push(byte1);
    }
    encoded
}

fn decode(matrix: Matrix, stream: &[u8]) -> Vec<u8> {
    // this is safe to do for decoding because the encoding split each
    // byte into two bytes, hence a file encoded with this program
    // will always have an even number of bytes
    let mut decoded = Vec::with_capacity(stream.len() / 2);
    for bytes in stream.chunks(2) {
        decoded.push(matrix.decode(bytes[0], bytes[1]));
    }
    decoded
}

fn parallel_codec<Worker>(task: Worker, stream: Vec<u8>, chunk_size: usize) -> Vec<Vec<u8>>
where
    Worker: Fn(&[u8]) -> Vec<u8>,
    Worker: Send + Sync,
{
    stream.par_chunks(chunk_size).map(task).collect()
}

/// chunk size should always be even for the decoding phase
fn chunk_size(stream_len: usize, max_workers: usize) -> usize {
    const MASK: usize = std::usize::MAX - 1;
    if max_workers <= 1 {
        return stream_len;
    }
    let even_size = (stream_len / max_workers) & MASK;
    if even_size == 0 {
        stream_len
    } else {
        even_size
    }
}

#[cfg(test)]
mod codec_tests {
    use super::*;
    use rstest::rstest_parametrize;
    use std::convert::From;

    #[rstest_parametrize(
        original,
        case("abcdefghijklmnopqrstuvwxyzab"),
        case("abcdefghijklmnopqrstuvwxyza"),
        case("abcdefghijklmnopqrstuvwxyz"),
        case("abcdefghijklmnopqrstuvwxy"),
        case("abcdefghijklmnopqrstuvwx")
    )]
    fn should_encode_and_decode(original: &str) {
        let clear = original.as_bytes().to_vec();
        let matrix = From::from([12, 16, 254, 24]);
        let decoded = decode(matrix, &encode(matrix, &clear));
        assert_eq!(clear, decoded);
    }

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
        case(300),
        case(79871),
        case(524287), // prime number
    )]
    fn should_encode_decode_random(n: usize) {
        let random_bytes = random(n);
        let matrix = From::from([12, 16, 254, 24]);
        let decoded = decode(matrix, &encode(matrix, &random_bytes));
        assert_eq!(random_bytes, decoded);
    }

    #[rstest_parametrize(
        n,
        case(300),
        case(79871),
        case(524287), // prime number
    )]
    fn should_encode_decode_random_using_codec_struct(n: usize) {
        let random_bytes = random(n);
        let codec = Codec::new([12, 16, 254, 24].into(), 1);
        let encoded = codec.encode(random_bytes.clone());
        let decoded = codec.decode(encoded[0].clone());
        assert_eq!(vec![random_bytes], decoded);
    }

    #[test]
    fn should_map_and_chunk() {
        let ret = parallel_codec(|x| x.iter().map(|y| y + 1).collect(), vec![0, 1], 1);
        assert_eq!(ret, vec![vec![1], vec![2]]);
        let ret = parallel_codec(|x| x.iter().map(|y| y + 1).collect(), vec![0, 1], 2);
        assert_eq!(ret, vec![vec![1, 2]]);
    }

    #[rstest_parametrize(
        stream_len,
        max_workers,
        expected_chunk_size,
        case(0, 1, 0),
        case(8, 8, 8),
        case(8, 0, 8),
        case(2, 1, 2),
        case(7, 0, 7),
        case(7, 1, 7),
        case(7, 2, 2),
        case(7, 3, 2),
        case(21, 3, 6),
        case(100000000, 0, 100000000),
        case(100000000, 1, 100000000),
        case(100000000, 2, 50000000),
        case(100000000, 3, 33333332),
        case(std::usize::MAX, 1, std::usize::MAX)
    )]
    fn compute_chunk_size(stream_len: usize, max_workers: usize, expected_chunk_size: usize) {
        let size = chunk_size(stream_len, max_workers);
        assert_eq!(size, expected_chunk_size)
    }
}
