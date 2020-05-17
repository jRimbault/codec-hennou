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
    let chunk_size = cache_size::l3_cache_size().unwrap_or(12 * 1024 * 1024);
    let read_start = Instant::now();
    let source = fs::read(params.source)?;
    let read_end = read_start.elapsed();
    let encoding_start = Instant::now();
    let result: Vec<Vec<u8>> = source.par_chunks(chunk_size).map(codec).collect();
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
