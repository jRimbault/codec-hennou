use crate::build_info;

use clap::{App, Arg, ArgGroup, ArgMatches};
use std::convert;
use std::ffi::OsString;
use std::fmt;

pub enum Argument {
    Action,
    KeyFile,
    Source,
    Dest,
    Decode,
    Encode,
}

pub fn parse_args<T, I>(itr: I) -> ArgMatches<'static>
where
    I: IntoIterator<Item = T>,
    T: Into<OsString> + Clone,
{
    let long_version = format!(
        "{} {}, {}",
        build_info::PKG_VERSION,
        build_info::GIT_VERSION.expect(""),
        build_info::RUSTC_VERSION
    );
    App::new(build_info::PKG_NAME)
        .version(build_info::PKG_VERSION)
        .long_version(long_version.as_str())
        .author(build_info::PKG_AUTHORS)
        .arg(
            Arg::with_name(Argument::KeyFile.as_str())
                .help(Argument::KeyFile.description())
                .required(true),
        )
        .arg(
            Arg::with_name(Argument::Source.as_str())
                .help(Argument::Source.description())
                .required(true),
        )
        .arg(
            Arg::with_name(Argument::Dest.as_str())
                .help(Argument::Dest.description())
                .required(true),
        )
        .arg(
            Arg::with_name(Argument::Decode.as_str())
                .help(Argument::Action.description())
                .short(Argument::Decode.as_str())
                .long(Argument::Decode.as_str()),
        )
        .arg(
            Arg::with_name(Argument::Encode.as_str())
                .help(Argument::Action.description())
                .short(Argument::Encode.as_str())
                .long(Argument::Encode.as_str()),
        )
        .group(
            ArgGroup::with_name(Argument::Action.as_str())
                .args(&[Argument::Decode.as_str(), Argument::Encode.as_str()])
                .required(true),
        )
        .get_matches_from(itr)
}

impl Argument {
    pub fn as_str(&self) -> &'static str {
        match *self {
            Argument::Action => "action",
            Argument::Encode => "encode",
            Argument::Decode => "decode",
            Argument::KeyFile => "keyfile",
            Argument::Source => "source",
            Argument::Dest => "dest",
        }
    }

    pub fn description(&self) -> &'static str {
        match *self {
            Argument::Action => "Action to execute on the source file",
            Argument::KeyFile => "File containing a G4C key",
            Argument::Source => "File to encode or decoded",
            Argument::Dest => "Output file",
            _ => "",
        }
    }
}

impl fmt::Display for Argument {
    fn fmt(&self, fmt: &mut fmt::Formatter) -> fmt::Result {
        fmt.write_str(self.as_str())
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
