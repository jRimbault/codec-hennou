use std::convert::TryInto;
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
            .map(|keytext| {
                keytext
                    .split_whitespace()
                    .filter_map(|bin| u8::from_str_radix(&bin, 2).ok())
                    .collect::<Vec<u8>>()
            })
            .and_then(|key| if key.len() == 4 { Some(key) } else { None })
            .and_then(|key| key[..4].try_into().ok())
            .map(Matrix::from_key)
    }

    pub fn from_key(key: [u8; 4]) -> Self {
        let encode = get_encode_matrix(&key);
        let decode = get_decode_matrix(&encode);
        Matrix { encode, decode }
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

fn get_decode_matrix(matrix: &EncodeMatrix) -> DecodeMatrix {
    let mut lookup = [0; 256];
    for (i, val) in matrix.iter().enumerate() {
        lookup[*val as usize] = i as u8;
    }
    lookup
}

/// explicit layout to make direct lookups during
/// the encode phase and facilitate the decode phase
fn get_encode_matrix(key: &[u8; 4]) -> EncodeMatrix {
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
