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
        parallel_codec(|s| encode(&self.matrix, s), stream, size)
    }

    pub fn decode(&self, stream: Vec<u8>) -> Vec<Vec<u8>> {
        let size = chunk_size(stream.len(), self.max_workers);
        parallel_codec(|s| decode(&self.matrix, s), stream, size)
    }
}

fn encode(matrix: &Matrix, stream: &[u8]) -> Vec<u8> {
    // it seems using a *constant small number* of `push`
    // is faster than using `extend` on a Vec
    let mut encoded = Vec::with_capacity(stream.len() * 2);
    for &byte in stream {
        let [byte0, byte1] = matrix.encode(byte);
        encoded.push(byte0);
        encoded.push(byte1);
    }
    encoded
}

fn decode(matrix: &Matrix, stream: &[u8]) -> Vec<u8> {
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
    match stream_len.checked_div(max_workers) {
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
mod codec_tests {
    use super::*;
    use crate::matrix::Matrix;
    use rstest::rstest_parametrize;

    static KEY: &str = "10101111 11111111 10101100 10011010";

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
        let matrix = Matrix::from_key([12, 16, 254, 24]);
        let decoded = decode(&matrix, &encode(&matrix, &clear));
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
        let matrix = Matrix::from_raw(KEY).unwrap();
        let decoded = decode(&matrix, &encode(&matrix, &random_bytes));
        assert_eq!(random_bytes, decoded);
    }
}
