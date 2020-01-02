package codech

import (
	"io/ioutil"
)

// Run program
func Run(keyfile string, action string, source string, dest string) {
	matrix, reverse := getMatrix(keyfile)
	if action == "-e" || action == "--encode" {
		work(func(stream []byte) []byte {
			return encode(matrix, stream)
		}, source, dest)
	} else {
		work(func(stream []byte) []byte {
			return decode(reverse, stream)
		}, source, dest)
	}
}

func work(task func([]byte) []byte, source string, dest string) {
	content, err := ioutil.ReadFile(source)
	if err != nil {
		panic(err)
	}
	ioutil.WriteFile(dest, task(content), 0644)
}

func encode(matrix Matrix, stream []byte) []byte {
	const MASK = uint8(0x0F)
	res := make([]byte, 0)
	for _, b := range stream {
		res = append(res, matrix[b&MASK])
		res = append(res, matrix[(b>>4)&MASK])
	}
	return res
}

func decode(reverse ReverseMatrix, stream []byte) []byte {
	res := make([]byte, 0)
	for i := 0; i < len(stream); i += 2 {
		a, b := stream[i], stream[i+1]
		pos1, pos2 := reverse[a], reverse[b]<<4
		res = append(res, pos1|pos2)
	}
	return res
}

// getChunkSize should always return an even number
// allowing the stream to be
// chunked into n parts where n would be the number of workers
func getChunkSize(length int) int {
	// runtime.NumCPU()
	return 2
}
