use crate::build_info;

use clap::{App, Arg, ArgGroup, ArgMatches};
use std::convert;
use std::ffi::OsString;

pub enum Argument {
    KeyFile,
    Source,
    Dest,
    Decode,
    Encode,
    Timings,
}

pub fn parse_args<T, I>(itr: I) -> ArgMatches<'static>
where
    I: IntoIterator<Item = T>,
    T: Into<OsString> + Clone,
{
    use Argument::*;
    let long_version = format!(
        "{} {}, {}",
        build_info::PKG_VERSION,
        build_info::GIT_VERSION.unwrap_or("dirty"),
        build_info::RUSTC_VERSION,
    );
    App::new(build_info::PKG_NAME)
        .version(build_info::PKG_VERSION)
        .long_version(long_version.as_str())
        .author(build_info::PKG_AUTHORS)
        .arg(
            Arg::with_name(KeyFile.as_str())
                .help(KeyFile.description())
                .required(true),
        )
        .arg(
            Arg::with_name(Source.as_str())
                .help(Source.description())
                .required(true),
        )
        .arg(
            Arg::with_name(Dest.as_str())
                .help(Dest.description())
                .required(true),
        )
        .arg(
            Arg::with_name(Timings.as_str())
                .help(Timings.description())
                .short(Timings.as_str())
                .long(Timings.as_str()),
        )
        .arg(
            Arg::with_name(Decode.as_str())
                .help(Decode.description())
                .short(Decode.as_str())
                .long(Decode.as_str()),
        )
        .arg(
            Arg::with_name(Encode.as_str())
                .help(Encode.description())
                .short(Encode.as_str())
                .long(Encode.as_str()),
        )
        .group(
            ArgGroup::with_name("action")
                .args(&[Decode.as_str(), Encode.as_str()])
                .required(true),
        )
        .get_matches_from(itr)
}

impl Argument {
    pub fn as_str(&self) -> &'static str {
        use Argument::*;
        match *self {
            Encode => "encode",
            Decode => "decode",
            KeyFile => "keyfile",
            Source => "source",
            Dest => "dest",
            Timings => "timings",
        }
    }

    pub fn description(&self) -> &'static str {
        use Argument::*;
        match *self {
            Encode | Decode => "Action to execute on the source file",
            KeyFile => "File containing a G4C key",
            Source => "File to encode or decoded",
            Dest => "Output file",
            Timings => "Output codec execution duration (seconds)",
        }
    }
}

impl convert::AsRef<str> for Argument {
    fn as_ref(&self) -> &str {
        self.as_str()
    }
}

#[test]
fn should_parse_args() {
    let args = parse_args("codech key source dest -e".split_whitespace());
    assert!(args.is_present(Argument::KeyFile));
    assert!(args.is_present(Argument::Source));
    assert!(args.is_present(Argument::Dest));
    assert!(args.is_present(Argument::Encode));
    assert!(!args.is_present(Argument::Decode));
}
