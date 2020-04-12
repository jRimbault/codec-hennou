use std::convert::{From, TryInto};
use std::str;

type EncodeMatrix = [u8; 16];
type DecodeMatrix = [u8; 256];

#[derive(Copy, Clone)]
pub struct Matrix {
    encode: EncodeMatrix,
    decode: DecodeMatrix,
}

impl Matrix {
    /// Takes a slice of bytes with a similar ASCII representation to :
    ///
    /// `b"10101111 11111111 10101100 10011010"`
    ///
    /// This slice should be 35 characters long, including whitespace
    pub fn from_raw<T: Into<Vec<u8>>>(raw: T) -> Option<Self> {
        str::from_utf8(&raw.into())
            .ok()
            .and_then(|keytext| {
                if keytext.len() == 35 {
                    Some(keytext)
                } else {
                    None
                }
            })
            .map(|text| binary_repr(text))
            .and_then(|key| if key.len() == 4 { Some(key) } else { None })
            .and_then(|key| key[..4].try_into().ok())
            .map(|key: [u8; 4]| key.into())
    }

    pub fn encode(&self, byte: u8) -> [u8; 2] {
        const MASK: u8 = 0x0f;
        const SHIFT: u8 = 4;
        [
            self.encode[(byte & MASK) as usize],
            self.encode[(byte >> SHIFT) as usize],
        ]
    }

    pub fn decode(&self, byte0: u8, byte1: u8) -> u8 {
        const SHIFT: u8 = 4;
        let (p1, p2) = (
            self.decode[byte0 as usize],
            self.decode[byte1 as usize] << SHIFT,
        );
        p1 | p2
    }
}

impl From<[u8; 4]> for Matrix {
    fn from(key: [u8; 4]) -> Matrix {
        let encode = get_encode_matrix(key);
        let decode = get_decode_matrix(&encode);
        Matrix { encode, decode }
    }
}

fn get_decode_matrix(matrix: &EncodeMatrix) -> DecodeMatrix {
    let mut lookup = [0; 256];
    for (i, val) in matrix.iter().enumerate() {
        lookup[*val as usize] = i as u8;
    }
    lookup
}

/// explicit layout to make direct lookups during
/// the encode phase and facilitate the decode phase
fn get_encode_matrix(key: [u8; 4]) -> EncodeMatrix {
    [
        key[0] ^ key[1] ^ key[2] ^ key[3],
        key[3],
        key[2],
        key[2] ^ key[3],
        key[1],
        key[1] ^ key[3],
        key[1] ^ key[2],
        key[1] ^ key[2] ^ key[3],
        key[0],
        key[0] ^ key[3],
        key[0] ^ key[2],
        key[0] ^ key[2] ^ key[3],
        key[0] ^ key[1],
        key[0] ^ key[1] ^ key[3],
        key[0] ^ key[1] ^ key[2],
        0,
    ]
}

fn binary_repr(text: &str) -> Vec<u8> {
    text.split_whitespace()
        .filter_map(|bin| u8::from_str_radix(&bin, 2).ok())
        .collect()
}

#[cfg(test)]
#[cfg_attr(tarpaulin, skip)]
mod matrix_tests {
    use super::*;
    use rstest::rstest_parametrize;

    #[test]
    fn should_get_matrix() {
        let text = "01011100 00011101 10100100 10010010";
        let matrix = Matrix::from_raw(text);
        assert!(matrix.is_some())
    }

    #[rstest_parametrize(
        text,
        case("01011100 00011101 10102100 10010010"),
        case("abcdefghijklmnopqrstuvwxyza"),
        case("01011100 00011101 10010010"),
        case(""),
        case("01011100 00011101 10102100 10010010 01011100"),
        case("01011100 0001 110110101100 10010010")
    )]
    fn should_not_get_matrix(text: &str) {
        let matrix = Matrix::from_raw(text);
        assert!(matrix.is_none())
    }
}
