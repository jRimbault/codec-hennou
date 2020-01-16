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

fn main() {
    process::exit(match run(parse_args(env::args())) {
        None => exitcode::OK,
        Some(error) => {
            println!("{}", error);
            error.raw_os_error().unwrap_or(exitcode::SOFTWARE)
        }
    })
}

fn run(args: ArgMatches<'static>) -> Option<io::Error> {
    use io::{Error, ErrorKind::InvalidData};
    let keyfile = args.value_of(Argument::KeyFile).unwrap();
    fs::read_to_string(keyfile)
        .map(|key| key[5..40].to_owned())
        .and_then(|key| Matrix::from_raw(key).ok_or(Error::new(InvalidData, "Invalid key")))
        .map(|matrix| Codec::new(matrix, num_cpus::get()))
        .map(|codec| execute(args, codec))
        .err()
}

fn execute(args: ArgMatches<'static>, codec: Codec) -> io::Result<()> {
    if args.is_present(Argument::Encode) {
        io_codec(args, |bytes| codec.encode(bytes))
    } else {
        io_codec(args, |bytes| codec.decode(bytes))
    }
}

fn io_codec<Converter>(args: ArgMatches<'static>, codec: Converter) -> io::Result<()>
where
    Converter: Fn(Vec<u8>) -> Vec<Vec<u8>>,
{
    use Argument::*;
    let source = fs::read(args.value_of(Source).unwrap())?;
    let start = std::time::Instant::now();
    let result = codec(source);
    if args.is_present(Timings) {
        println!("{}", start.elapsed().as_secs_f32());
    }
    let mut dest = File::create(args.value_of(Dest).unwrap())?;
    for part in result {
        dest.write_all(&part)?;
    }
    Ok(())
}
