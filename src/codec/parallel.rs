use crate::codec::Codec;
use rayon::prelude::*;

pub struct ParallelCodec {
    codec: Codec,
    max_workers: usize,
}

impl ParallelCodec {
    pub fn new(codec: Codec, max_workers: usize) -> Self {
        ParallelCodec { codec, max_workers }
    }

    pub fn encode(&self, stream: Vec<u8>) -> Vec<Vec<u8>> {
        parallel_codec(|bytes| self.codec.encode(bytes), stream, self.max_workers)
    }

    pub fn decode(&self, stream: Vec<u8>) -> Vec<Vec<u8>> {
        parallel_codec(|bytes| self.codec.decode(bytes), stream, self.max_workers)
    }
}

impl<T: Into<Codec>> From<T> for ParallelCodec {
    fn from(value: T) -> Self {
        ParallelCodec::new(value.into(), num_cpus::get())
    }
}

fn parallel_codec<Worker>(task: Worker, stream: Vec<u8>, max_workers: usize) -> Vec<Vec<u8>>
where
    Worker: Fn(&[u8]) -> Vec<u8>,
    Worker: Send + Sync,
{
    let size = chunk_size(stream.len(), max_workers);
    stream.par_chunks(size).map(task).collect()
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
mod tests {
    use super::*;
    use rstest::rstest;

    fn random(n: usize) -> Vec<u8> {
        const MAX_CHUNK: usize = 32;
        (0..(n / MAX_CHUNK) + 1)
            .map(|_| rand::random::<[u8; MAX_CHUNK]>().to_vec())
            .flatten()
            .collect::<Vec<u8>>()[..n]
            .to_vec()
    }

    #[test]
    fn should_encode_decode() {
        let random_bytes = random(1024);
        let codec: ParallelCodec = [24, 56, 78, 3].into();
        let encoded = codec
            .encode(random_bytes.clone())
            .into_iter()
            .flatten()
            .collect();
        let decoded: Vec<u8> = codec.decode(encoded).into_iter().flatten().collect();
        assert_eq!(random_bytes, decoded);
    }

    #[test]
    fn should_map_and_chunk() {
        let ret = parallel_codec(
            |x| x.iter().map(|y| y + 1).collect(),
            (0..2).map(u8::from).collect(),
            1,
        );
        assert_eq!(ret, vec![vec![1, 2]]);
        let ret = parallel_codec(
            |x| x.iter().map(|y| y + 1).collect(),
            (0..4).map(u8::from).collect(),
            2,
        );
        assert_eq!(ret, vec![vec![1, 2], vec![3, 4]]);
    }

    #[rstest(
        stream_len,
        max_workers,
        expected_chunk_size,
        case(0, 1, 0),
        // a naive division would result in chunks of size 1,
        // but chunk size cannot be odd during the decoding and,
        // obviously can't be 0, so chunk size will be the length
        // of original stream of bytes
        case(2, 2, 2),
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
