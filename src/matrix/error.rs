use std::error::Error;
use std::fmt;
use std::num::ParseIntError;

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum MatrixErrorKind {
    KeyIsTooShort,
    ParseError(ParseIntError),
}

#[derive(Debug, Clone)]
pub struct MatrixError {
    kind: MatrixErrorKind,
}

impl MatrixError {
    pub fn new(kind: MatrixErrorKind) -> Self {
        MatrixError { kind }
    }

    fn description(&self) -> String {
        use MatrixErrorKind::*;
        match &self.kind {
            KeyIsTooShort => "key is too short".to_owned(),
            ParseError(error) => error.to_string(),
        }
    }
}

impl fmt::Display for MatrixError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        self.description().fmt(f)
    }
}

impl Error for MatrixError {
    fn source(&self) -> Option<&(dyn Error + 'static)> {
        match &self.kind {
            MatrixErrorKind::KeyIsTooShort => None,
            MatrixErrorKind::ParseError(error) => Some(error),
        }
    }
}

impl From<ParseIntError> for MatrixError {
    fn from(error: ParseIntError) -> Self {
        MatrixError::new(MatrixErrorKind::ParseError(error))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::matrix::Matrix;
    use rstest::rstest;

    #[test]
    fn should_error_if_key_too_short() {
        let matrix = "01011100 00011101 10010010".parse::<Matrix>();
        assert!(matrix.is_err());
        let error = matrix.err().unwrap();
        assert_eq!(error.kind, MatrixErrorKind::KeyIsTooShort);
    }

    #[rstest(
        text,
        case("01011100 00011101 10102100 10010010"),
        case("abcdefghijklmnopqrstuvwxyza"),
        case("01011100 00011101 10102100 10010010 01011100"),
        case("01011100 0001 110110101100 10010010")
    )]
    fn should_fail_to_parse(text: &str) {
        let matrix = text.parse::<Matrix>();
        assert!(matrix.is_err());
    }

    #[test]
    fn it_should_produce_error_messages() {
        let key_too_short = MatrixError::new(MatrixErrorKind::KeyIsTooShort);
        assert_eq!(key_too_short.description(), "key is too short".to_owned());
        let parse_error = u8::from_str_radix("", 2).err().unwrap();
        let empty_string = MatrixError::new(MatrixErrorKind::ParseError(parse_error));
        assert_eq!(
            empty_string.description(),
            "cannot parse integer from empty string".to_owned()
        );
        assert_eq!(format!("{}", key_too_short), "key is too short");
        assert!(key_too_short.source().is_none());
        assert!(empty_string.source().is_some());
    }
}
