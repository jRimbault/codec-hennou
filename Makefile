build:
	dotnet publish -c Release -r linux-x64 /p:PublishSingleFile=true

bench: build
	@./bench.py -s 64 -s
	@./bench.py -s 512 -s
	@./bench.py -s 1024
