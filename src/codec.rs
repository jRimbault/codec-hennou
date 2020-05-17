use crate::matrix::Matrix;

pub struct Codec {
    matrix: Matrix,
}

impl Codec {
    pub fn new(matrix: Matrix) -> Self {
        Codec { matrix }
    }

    pub fn encode(&self, stream: &[u8]) -> Vec<u8> {
        // it seems using a *constant small number* of `push`es
        // is faster than using `extend` on a Vec
        // implementing a custom iterator to use flat_map and next 2 times
        // is slower, and this allows pre-allocating exactly what will be needed
        let mut encoded = Vec::with_capacity(stream.len() * 2);
        for bytes in stream.iter().map(|&byte| self.matrix.encode(byte)) {
            encoded.push(bytes[0]);
            encoded.push(bytes[1]);
        }
        encoded
    }

    pub fn decode(&self, stream: &[u8]) -> Vec<u8> {
        // this is safe to do for decoding because the encoding split each
        // byte into two bytes, hence a file encoded with this program
        // will always have an even number of bytes
        stream
            .chunks_exact(2)
            .map(|bytes| self.matrix.decode(bytes[0], bytes[1]))
            .collect()
    }
}

impl<T: Into<Matrix>> From<T> for Codec {
    fn from(key: T) -> Self {
        Codec::new(key.into())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use rstest::rstest;

    #[rstest(
        original,
        case("abcdefghijklmnopqrstuvwxyzab"),
        case("abcdefghijklmnopqrstuvwxyza"),
        case("abcdefghijklmnopqrstuvwxyz"),
        case("abcdefghijklmnopqrstuvwxy"),
        case("abcdefghijklmnopqrstuvwx")
    )]
    fn should_encode_and_decode(original: &str) {
        let clear = original.as_bytes().to_vec();
        let codec: Codec = [12, 16, 254, 24].into();
        let decoded = codec.decode(&codec.encode(&clear));
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

    #[rstest(
        n,
        case(300),
        case(79871),
        case(524287), // prime number
    )]
    fn should_encode_decode_random(n: usize) {
        let random_bytes = random(n);
        let codec: Codec = [12, 16, 254, 24].into();
        let decoded = codec.decode(&codec.encode(&random_bytes));
        assert_eq!(random_bytes, decoded);
    }

    #[rstest(
        n,
        case(300),
        case(79871),
        case(524287), // prime number
    )]
    fn should_encode_decode_random_using_codec_struct(n: usize) {
        let random_bytes = random(n);
        let codec: Codec = [12, 16, 254, 24].into();
        let encoded = codec.encode(&random_bytes);
        let decoded = codec.decode(&encoded);
        assert_eq!(random_bytes, decoded);
    }

    #[test]
    fn should_build_codec_directly_from_key() {
        let _: Codec = [1, 2, 3, 4].into();
    }
}
