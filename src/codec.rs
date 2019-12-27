use crate::matrix::Matrix;

static MASK: u8 = 0x0f;

pub fn encode(matrix: Matrix, stream: &[u8]) -> Vec<u8> {
    fn split_char_in_two(matrix: Matrix, ch: u8) -> [u8; 2] {
        [
            matrix[(ch & MASK) as usize],
            matrix[(((ch >> 4) & MASK) as usize)],
        ]
    }
    stream
        .iter()
        .map(|ch| split_char_in_two(matrix, *ch).to_vec())
        .flatten()
        .collect()
}

pub fn decode(matrix: Matrix, stream: &[u8]) -> Vec<u8> {
    fn join_encoded_chars(matrix: Matrix, chars: &[u8]) -> u8 {
        // this safe to do for decoding because the encoding split each
        // byte into two bytes, hence a file encoded with this program
        // will always have an even number of bytes
        // replacing the matrix with a lookup table would improve the algo
        // if the matrix were larger
        let pos1 = matrix.iter().position(|&c| c == chars[0]).unwrap() as u8;
        let pos2 = (matrix.iter().position(|&c| c == chars[1]).unwrap() << 4) as u8;
        pos1 | pos2
    }
    stream
        .chunks(2)
        .map(|chars| join_encoded_chars(matrix, chars))
        .collect()
}

#[cfg(test)]
mod codec_tests {
    use super::*;
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
        let decoded = decode(MATRIX, &encode(MATRIX, &clear));
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
        let decoded = decode(MATRIX, &encode(MATRIX, &random_bytes));
        assert_eq!(random_bytes, decoded);
    }
}
