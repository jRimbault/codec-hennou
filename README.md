# Projet "encodage"


Pour tester:
```sh
cd build
dd if=/dev/urandom of=random.dat  bs=1M  count=1024 # génère fichier aléatoire de 1Go
cmake .
make
./codec --help
```
Ou, plus simplement:
```sh
make bench
```


[![asciicast](https://asciinema.org/a/85yFfN4KBtwVc6reBgQU8J2Nn.png)](https://asciinema.org/a/85yFfN4KBtwVc6reBgQU8J2Nn)
