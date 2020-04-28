pub mod error;

use error::MatrixError;
use std::str::FromStr;

/// I could make the EncodeLookup a [[u8; 2]; 256] pre-filled with
/// everything, but after testing that, the gain is actually
/// not very noticeable.
/// Until I find a better way to do it, I'll abstain.
/// The DecodeLookup cannot be fully pre-computed, but it could
/// be composed of 2 [u8; 256], one filled with pre-shifted values,
/// the gain here is even more marginal.
/// That makes sense since the operation is already O(n)...

#[derive(Copy, Clone)]
struct EncodeLookup([u8; 16]);
#[derive(Copy, Clone)]
struct DecodeLookup([u8; 256]);

#[derive(Copy, Clone)]
pub struct Matrix {
    encode: EncodeLookup,
    decode: DecodeLookup,
}

impl From<[u8; 4]> for Matrix {
    fn from(key: [u8; 4]) -> Matrix {
        let encode = key.into();
        Matrix {
            encode,
            decode: encode.into(),
        }
    }
}

impl FromStr for Matrix {
    type Err = MatrixError;

    fn from_str(value: &str) -> Result<Self, Self::Err> {
        let key = value
            .split_whitespace()
            .take(4)
            .map(|bin| u8::from_str_radix(&bin, 2))
            .collect::<Result<Vec<u8>, _>>()?;
        if key.len() < 4 {
            Err(MatrixError::KeyIsTooShort)
        } else {
            Ok([key[0], key[1], key[2], key[3]].into())
        }
    }
}

impl Matrix {
    pub fn encode(&self, byte: u8) -> [u8; 2] {
        const MASK: u8 = 0x0f;
        const SHIFT: u8 = 4;
        [
            self.encode.0[(byte & MASK) as usize],
            self.encode.0[(byte >> SHIFT) as usize],
        ]
    }

    pub fn decode(&self, byte0: u8, byte1: u8) -> u8 {
        const SHIFT: u8 = 4;
        let (p1, p2) = (
            self.decode.0[byte0 as usize],
            self.decode.0[byte1 as usize] << SHIFT,
        );
        p1 | p2
    }
}

impl From<[u8; 4]> for EncodeLookup {
    /// specific layout to make direct lookups during the encoding
    fn from(key: [u8; 4]) -> EncodeLookup {
        EncodeLookup([
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
        ])
    }
}

impl From<EncodeLookup> for DecodeLookup {
    /// specific layout to make direct lookups during the decoding
    fn from(encode_lookup: EncodeLookup) -> DecodeLookup {
        let mut lookup = [0; 256];
        for (i, val) in encode_lookup.0.iter().enumerate() {
            lookup[*val as usize] = i as u8;
        }
        DecodeLookup(lookup)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use rstest::rstest;

    #[test]
    fn should_get_matrix() {
        let text = "01011100 00011101 10100100 10010010";
        let matrix = text.parse::<Matrix>();
        assert!(matrix.is_ok());
    }

    #[rstest(
        text,
        case("01011100 00011101 10102100 10010010"),
        case("abcdefghijklmnopqrstuvwxyza"),
        case("01011100 00011101 10010010"),
        case(""),
        case("01011100 00011101 10102100 10010010 01011100"),
        case("01011100 0001 110110101100 10010010")
    )]
    fn should_not_get_matrix(text: &str) {
        let matrix = text.parse::<Matrix>();
        assert!(matrix.is_err());
    }
}
