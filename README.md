# Projet "encodage"


Pour tester:
```sh
cd build
dd if=/dev/urandom of=random.dat  bs=1M  count=1024 # génère fichier aléatoire de 1Go
cmake .
make
./codec --help
```

[![asciicast](https://asciinema.org/a/7gpuugdqxzplow2vk4n8vgml5.png)](https://asciinema.org/a/7gpuugdqxzplow2vk4n8vgml5)
