build:
	dotnet publish -c Release -r linux-x64 /p:PublishSingleFile=true

bench: build
	@./bench.py -s 64 -hr
	@./bench.py -s 512 -hr
	@./bench.py -s 1024
