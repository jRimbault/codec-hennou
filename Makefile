build:
	dotnet publish -c Release -r linux-x64 --self-contained true

bench: build
	@./bench.sh -f 64 -s
	@./bench.sh -f 512 -s
	@./bench.sh -f 1024
