use crate::matrix::{Matrix, ReverseMatrix};

static MASK: u8 = 0x0f;

pub fn encode(matrix: Matrix, stream: &[u8]) -> Vec<u8> {
    let mut res: Vec<u8> = Vec::with_capacity(stream.len() * 2);
    for b in stream {
        res.push(matrix[(b & MASK) as usize]);
        res.push(matrix[((b >> 4) & MASK) as usize]);
    }
    res
}

pub fn decode(matrix: ReverseMatrix, stream: &[u8]) -> Vec<u8> {
    // this is safe to do for decoding because the encoding split each
    // byte into two bytes, hence a file encoded with this program
    // will always have an even number of bytes
    let mut res: Vec<u8> = Vec::with_capacity(stream.len() / 2);
    for i in (0..stream.len()).step_by(2) {
        let (a, b) = (stream[i], stream[i + 1]);
        let (p1, p2) = (matrix[a as usize], matrix[b as usize] << 4);
        res.push(p1 | p2);
    }
    res
}

#[cfg(test)]
mod codec_tests {
    use super::*;
    use crate::matrix::get_reverse_matrix;
    use rstest::rstest_parametrize;

    static MATRIX: Matrix = [
        126, 146, 164, 54, 199, 85, 99, 241, 143, 29, 43, 185, 72, 218, 236, 0,
    ];

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
        let decoded = decode(get_reverse_matrix(MATRIX), &encode(MATRIX, &clear));
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
        let decoded = decode(get_reverse_matrix(MATRIX), &encode(MATRIX, &random_bytes));
        assert_eq!(random_bytes, decoded);
    }
}
