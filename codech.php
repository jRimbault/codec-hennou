<?php

function main($args) {
    echo Matrix::from_file("key.txt") . PHP_EOL;
}

/**
 * @param Matrix $matrix
 * @param int[] $bytes
 * @return int[]
 */
function encode(Matrix $matrix, array $bytes) {
    return $bytes;
}

/**
 * @param Matrix $matrix
 * @param int[] $bytes
 * @return int[]
 */
function decode(Matrix $matrix, array $bytes) {
    return $bytes;
}

class Matrix {
    /**
     * @var int[]
    */
    public array $encode;
    /**
     * @var int[]
     */
    public array $decode;

    private function __construct($encode, $decode) {
        $this->encode = $encode;
        $this->decode = $decode;
    }

    public static function from_file(string $filename): Matrix {
        $content = explode(' ', substr(file_get_contents($filename), 5, 35));

        $nums = [];
        foreach ($content as $num_str) {
            $nums[] = intval($num_str, 2);
        }
        $encode = Matrix::flatten($nums);
        $decode = Matrix::reverse($encode);
        return new Matrix($encode, $decode);
    }

    private static function flatten(array $key): array {
        return [
            $key[0] ^ $key[1] ^ $key[2] ^ $key[3],
            $key[3],
            $key[2],
            $key[2] ^ $key[3],
            $key[1],
            $key[1] ^ $key[3],
            $key[1] ^ $key[2],
            $key[1] ^ $key[2] ^ $key[3],
            $key[0],
            $key[0] ^ $key[3],
            $key[0] ^ $key[2],
            $key[0] ^ $key[2] ^ $key[3],
            $key[0] ^ $key[1],
            $key[0] ^ $key[1] ^ $key[3],
            $key[0] ^ $key[1] ^ $key[2],
        ];
    }

    private static function reverse(array $matrix): array {
        $reverse = [];
        for ($i = 0; $i < count($matrix); $i += 1) {
            $reverse[$matrix[$i]] = $i;
        }
        return $reverse;
    }

    public function __toString() {
        return json_encode([$this->encode, $this->decode]);
    }
}

main("hello world");
