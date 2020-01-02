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
	var res []byte = task(content)
	// parallel split task between chunks
	// for _, chunk := range split(content, 8) {
	// 	res = append(res, task(chunk)...)
	// }
	ioutil.WriteFile(dest, res, 0644)
}

func encode(matrix Matrix, stream []byte) []byte {
	const MASK = uint8(0x0F)
	var res []byte
	for _, b := range stream {
		res = append(res, matrix[b&MASK])
		res = append(res, matrix[(b>>4)&MASK])
	}
	return res
}

func decode(reverse ReverseMatrix, stream []byte) []byte {
	var res []byte
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

func split(buf []byte, lim int) [][]byte {
	var chunk []byte
	chunks := make([][]byte, 0, len(buf)/lim+1)
	for len(buf) >= lim {
		chunk, buf = buf[:lim], buf[lim:]
		chunks = append(chunks, chunk)
	}
	if len(buf) > 0 {
		chunks = append(chunks, buf[:len(buf)])
	}
	return chunks
}
