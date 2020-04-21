#![deny(warnings)]
#![forbid(unsafe_code)]

use codech::{codec::parallel::ParallelCodec, matrix::Matrix};
use std::error;
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
            exitcode::SOFTWARE
        }
    })
}

#[cfg_attr(tarpaulin, skip)]
fn run(args: Arguments) -> Result<(), Box<dyn error::Error>> {
    let key = fs::read_to_string(&args.keyfile)?;
    let codec: ParallelCodec = key[5..40].parse::<Matrix>()?.into();
    Ok(if args.encode {
        io_codec(args, |bytes| codec.encode(bytes))
    } else {
        io_codec(args, |bytes| codec.decode(bytes))
    }?)
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
