mod hash;

use std::io::Write;
use std::vec::Vec;
use std::io;

use serde::{Serialize,Deserialize};
use rand::seq::SliceRandom;
use rand::thread_rng;
use itertools;
use clap::Parser;

use hash::Hash;

#[derive(Parser)]
struct Args {
    id:  u32,
}

#[derive(Serialize,Deserialize)]
struct SellRequest {
    barrel_id: i32,
}


fn values_to_slice(start: &[u8], end: &[u8], values: &[u32]) -> Vec<u8> {

    let mut slice = start.to_vec();


    for value in values.iter() {
        slice.extend_from_slice(&value.to_le_bytes());
    }
    slice.extend_from_slice(end);

    slice
}

fn does_deserialize(slice: &[u8]) -> bool {
    match serde_json::from_slice::<SellRequest>(slice) {
        Ok(_) => true,
        Err(_) => false,
    }
}

fn main() {
    let args = Args::parse();
    let json_start: Vec<u8> = {
        let mut json_start: Vec<u8> = (
            "{\"barrel_id\":".to_owned() +
            &args.id.to_string() +
            ",\"kek\":\""
        ).into();
        while json_start.len() % 64 != 0 {
            json_start.push(b'a');
        }
        json_start
    };
    let json_end: Vec<u8> = "\"}".into();


    let mut possible_values: Vec<Vec<u32>> = vec![vec![0; 8]];

    let mut rng = thread_rng();

    for i in 0..32 {
        // println!("{i} {}", possible_values.len());
        let mut new_possible_values: Vec<Vec<u32>> = Vec::new();
        for values in possible_values.iter() {
            for bits in itertools::iproduct!(0..2, 0..2, 0..2, 0..2, 0..2, 0..2, 0..2, 0..2) {
                let bits = [bits.0, bits.1, bits.2, bits.3, bits.4, bits.5, bits.6, bits.7];


                let new_values: Vec<u32> = bits.into_iter().zip(values).map(|(bit, v)| v | (bit << i)).collect();
                let to_hash = values_to_slice(&json_start, &json_end, &new_values);

                let mut hash = Hash::new();
                hash.update(&to_hash);
                let hash_values = hash.digest_values();

                let and_modulo = (0u32).wrapping_sub(1) >> (31 - i);
                if hash_values.iter().all(|v| v & and_modulo == 0) {
                    new_possible_values.push(new_values);
                }
            }
        }
        new_possible_values.shuffle(&mut rng);

        possible_values = new_possible_values
            .into_iter()
            .take(1000)
            .collect();
    }

    possible_values = possible_values
        .into_iter()
        .filter(|values| does_deserialize(&values_to_slice(&json_start, &json_end, &values)))
        .collect();
    let output = values_to_slice(&json_start, &json_end, &possible_values[0]);

    let mut hash = Hash::new();
    hash.update(&output);
    let digest = hash.digest();

    assert_eq!(digest, b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    io::stdout().lock().write_all(&output).unwrap();
}
