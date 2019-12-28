use arr_macro::arr;
use std::convert::TryInto;
use std::fs;
use std::io;

pub type Matrix = [u8; 16];
pub type ReverseMatrix = [u8; 255];

pub fn get_matrix(filename: &str) -> io::Result<(Matrix, ReverseMatrix)> {
    let key = read_key(filename)?;
    if key.len() != 4 || key.iter().any(|&c| c == 0) {
        Err(io::Error::new(io::ErrorKind::InvalidData, ""))
    } else {
        let matrix = flatten_matrix(key[..4].try_into().expect(""));
        Ok((matrix, get_reverse_matrix(matrix)))
    }
}

pub fn get_reverse_matrix(matrix: Matrix) -> ReverseMatrix {
    let mut lookup: ReverseMatrix = arr![0; 255];
    for (i, val) in matrix.iter().enumerate() {
        lookup[*val as usize] = i as u8;
    }
    lookup
}

fn read_key(filename: &str) -> io::Result<Vec<u8>> {
    Ok(key(&fs::read_to_string(filename)?[5..40]))
}

fn flatten_matrix(key: [u8; 4]) -> Matrix {
    // explicit layout to make direct lookups during
    // the encode phase and facilitate the decode phase
    let mut matrix: Matrix = [0; 16];
    matrix[15] = 0;
    matrix[1] = key[3];
    matrix[2] = key[2];
    matrix[3] = key[2] ^ key[3];
    matrix[4] = key[1];
    matrix[5] = key[1] ^ key[3];
    matrix[6] = key[1] ^ key[2];
    matrix[7] = key[1] ^ key[2] ^ key[3];
    matrix[8] = key[0];
    matrix[9] = key[0] ^ key[3];
    matrix[10] = key[0] ^ key[2];
    matrix[11] = key[0] ^ key[2] ^ key[3];
    matrix[12] = key[0] ^ key[1];
    matrix[13] = key[0] ^ key[1] ^ key[3];
    matrix[14] = key[0] ^ key[1] ^ key[2];
    matrix[0] = key[0] ^ key[1] ^ key[2] ^ key[3];
    matrix
}

fn key(chars: &str) -> Vec<u8> {
    chars
        .split_whitespace()
        .map(|num_str| u8::from_str_radix(&num_str, 2).unwrap())
        .collect()
}

#[test]
fn should_make_a_reverse_lookup_matrix() {
    let reverse = get_reverse_matrix(arr![0; 16]);
    assert_eq!(reverse.len(), std::u8::MAX as usize);
}
