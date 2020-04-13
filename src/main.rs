#![forbid(unsafe_code)]

mod args;
mod build_info;
mod codec;
mod matrix;

use args::{parse_args, Argument};
use clap::ArgMatches;
use codec::Codec;
use exitcode;
use matrix::Matrix;
use std::env;
use std::fs;
use std::fs::File;
use std::io;
use std::io::prelude::*;
use std::process;
use std::time::Instant;

#[cfg_attr(tarpaulin, skip)]
fn main() {
    process::exit(match run(parse_args(env::args())) {
        None => exitcode::OK,
        Some(error) => {
            println!("{}", error);
            error.raw_os_error().unwrap_or(exitcode::SOFTWARE)
        }
    })
}

#[cfg_attr(tarpaulin, skip)]
fn run(args: ArgMatches<'static>) -> Option<io::Error> {
    use io::{Error, ErrorKind::InvalidData};
    let keyfile = args.value_of(Argument::KeyFile).unwrap();
    fs::read_to_string(keyfile)
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
        .map(|codec| execute(args, codec))
        .err()
}

#[cfg_attr(tarpaulin, skip)]
fn execute(args: ArgMatches<'static>, codec: Codec) -> io::Result<()> {
    if args.is_present(Argument::Encode) {
        io_codec(args, |bytes| codec.encode(bytes))
    } else {
        io_codec(args, |bytes| codec.decode(bytes))
    }
}

#[cfg_attr(tarpaulin, skip)]
fn io_codec<Converter>(args: ArgMatches<'static>, codec: Converter) -> io::Result<()>
where
    Converter: Fn(Vec<u8>) -> Vec<Vec<u8>>,
{
    use Argument::*;
    let read_start = Instant::now();
    let source = fs::read(args.value_of(Source).unwrap())?;
    let read_end = read_start.elapsed();
    let encoding_start = Instant::now();
    let result = codec(source);
    let encoding_end = encoding_start.elapsed();
    let write_start = Instant::now();
    let mut dest = File::create(args.value_of(Dest).unwrap())?;
    for part in result {
        dest.write_all(&part)?;
    }
    if args.is_present(Timings) {
        println!(
            "{} {} {}",
            read_end.as_secs_f32(),
            encoding_end.as_secs_f32(),
            write_start.elapsed().as_secs_f32()
        );
    }
    Ok(())
}
