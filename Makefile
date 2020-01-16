build: target/release/codech

target/release/codech:
	cargo build --release
	strip target/release/codech

bench: target/release/codech
	@./bench.sh -f 64 -s "${ARGS}"
	@./bench.sh -f 512 -s "${ARGS}"
	@./bench.sh -f 1024 "${ARGS}"
