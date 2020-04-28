#![deny(warnings)]
#![forbid(unsafe_code)]

use codech::{codec::Codec, matrix::Matrix};
use rayon::prelude::*;
use std::fs::{self, File};
use std::io::{self, prelude::*, BufReader, BufWriter};
use std::path::PathBuf;
use std::process;
use std::time::Instant;
use structopt::{clap::ArgGroup, StructOpt};

#[derive(StructOpt, Debug)]
struct Options {
    /// Action to execute on the source file
    #[structopt(subcommand)]
    task: Task,
    /// File containing a G4C key
    ///
    /// The first line of the file should be like the following :
    ///
    /// ```
    /// G4C=[10001111 11000111 10100100 10010010]
    /// ```
    keyfile: PathBuf,
}

#[derive(StructOpt, Debug)]
enum Task {
    Encode(Params),
    Decode(Params),
}

#[derive(StructOpt, Debug)]
#[structopt(group = ArgGroup::with_name("method"))]
struct Params {
    /// File to encode or decode
    source: PathBuf,
    /// Output file
    dest: PathBuf,
    /// Output codec execution duration (seconds)
    #[structopt(short, long, group = "method")]
    timings: bool,
    /// Use when running out of memory
    #[structopt(short, long, group = "method")]
    buffered: bool,
}

#[cfg_attr(tarpaulin, skip)]
fn main() {
    process::exit(match run(Options::from_args()) {
        Ok(_) => exitcode::OK,
        Err(error) => {
            println!("{}", error);
            exitcode::SOFTWARE
        }
    })
}

#[cfg_attr(tarpaulin, skip)]
fn run(opts: Options) -> Result<(), Box<dyn std::error::Error>> {
    fn pick_method<Converter>(params: Params, codec: Converter) -> io::Result<()>
    where
        Converter: Fn(&[u8]) -> Vec<u8>,
        Converter: Send + Sync,
    {
        if params.buffered {
            buffered_codec(params, codec)
        } else {
            multithreaded_codec(params, codec)
        }
    }
    use Task::*;
    let key = fs::read_to_string(&opts.keyfile)?;
    let codec: Codec = key[5..40].parse::<Matrix>()?.into();
    Ok(match opts.task {
        Encode(params) => pick_method(params, |bytes| codec.encode(bytes)),
        Decode(params) => pick_method(params, |bytes| codec.decode(bytes)),
    }?)
}

#[cfg_attr(tarpaulin, skip)]
fn multithreaded_codec<Converter>(params: Params, codec: Converter) -> io::Result<()>
where
    Converter: Fn(&[u8]) -> Vec<u8>,
    Converter: Send + Sync,
{
    let read_start = Instant::now();
    let source = fs::read(params.source)?;
    let read_end = read_start.elapsed();
    let encoding_start = Instant::now();
    let size = chunk_size(source.len(), num_cpus::get());
    let result: Vec<_> = source.par_chunks(size).map(codec).collect();
    let encoding_end = encoding_start.elapsed();
    let write_start = Instant::now();
    let mut dest = File::create(params.dest)?;
    for part in result {
        dest.write_all(&part)?;
    }
    if params.timings {
        println!(
            "{} {} {}",
            read_end.as_secs_f32(),
            encoding_end.as_secs_f32(),
            write_start.elapsed().as_secs_f32()
        );
    }
    Ok(())
}

#[cfg_attr(tarpaulin, skip)]
fn buffered_codec<Converter>(params: Params, codec: Converter) -> io::Result<()>
where
    Converter: Fn(&[u8]) -> Vec<u8>,
    Converter: Send + Sync,
{
    let mut source = BufReader::new(File::open(params.source)?);
    let mut dest = BufWriter::new(File::create(params.dest)?);
    loop {
        let buffer = source.fill_buf()?;
        let buffer_size = buffer.len();
        if buffer_size == 0 {
            break;
        }
        dest.write_all(&codec(&buffer))?;
        source.consume(buffer_size);
    }
    Ok(())
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
