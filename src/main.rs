#![forbid(unsafe_code)]

mod codec;
mod matrix;

use codec::Codec;
use exitcode;
use matrix::Matrix;
use std::fs;
use std::fs::File;
use std::io;
use std::io::prelude::*;
use std::path::PathBuf;
use std::process;
use std::time::Instant;
use structopt::{clap::ArgGroup, StructOpt};

#[derive(Clone, StructOpt, Debug)]
#[structopt(group = ArgGroup::with_name("action").required(true))]
struct Arguments {
    /// File containing a G4C key
    ///
    /// The first line of the file should be like the following :
    ///
    /// ```
    /// G4C=[10001111 11000111 10100100 10010010]
    /// ```
    keyfile: PathBuf,
    /// File to encode or decode
    source: PathBuf,
    /// Output file
    dest: PathBuf,
    /// Action to execute on the source file
    #[structopt(short, long, group = "action")]
    encode: bool,
    /// Action to execute on the source file
    #[structopt(short, long, group = "action")]
    decode: bool,
    /// Output codec execution duration (seconds)
    #[structopt(short, long)]
    timings: bool,
}

#[cfg_attr(tarpaulin, skip)]
fn main() {
    process::exit(match run(Arguments::from_args()) {
        Ok(_) => exitcode::OK,
        Err(error) => {
            println!("{}", error);
            error.raw_os_error().unwrap_or(exitcode::SOFTWARE)
        }
    })
}

#[cfg_attr(tarpaulin, skip)]
fn run(args: Arguments) -> io::Result<()> {
    use io::{Error, ErrorKind::InvalidData};
    fs::read_to_string(&args.keyfile)
        .map(|key| key.trim().to_owned())
        .and_then(|key| {
            if key.len() < 40 {
                Err(Error::new(InvalidData, "Key too short"))
            } else {
                Ok(key[5..40].to_owned())
            }
        })
        .and_then(|key| Matrix::from_raw(key).ok_or_else(|| Error::new(InvalidData, "Invalid key")))
        .map(|matrix| Codec::new(matrix, num_cpus::get()))
        .and_then(|codec| {
            if args.encode {
                io_codec(args, |bytes| codec.encode(bytes))
            } else {
                io_codec(args, |bytes| codec.decode(bytes))
            }
        })
}

#[cfg_attr(tarpaulin, skip)]
fn io_codec<Converter>(args: Arguments, codec: Converter) -> io::Result<()>
where
    Converter: Fn(Vec<u8>) -> Vec<Vec<u8>>,
{
    let read_start = Instant::now();
    let source = fs::read(args.source)?;
    let read_end = read_start.elapsed();
    let encoding_start = Instant::now();
    let result = codec(source);
    let encoding_end = encoding_start.elapsed();
    let write_start = Instant::now();
    let mut dest = File::create(args.dest)?;
    for part in result {
        dest.write_all(&part)?;
    }
    if args.timings {
        println!(
            "{} {} {}",
            read_end.as_secs_f32(),
            encoding_end.as_secs_f32(),
            write_start.elapsed().as_secs_f32()
        );
    }
    Ok(())
}
