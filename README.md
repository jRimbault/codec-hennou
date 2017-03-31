# Projet "encodage"

Encodage décodage fichier 2Go en moins de 30s.

Pour tester:
```sh
cd build
dd if=/dev/urandom of=random.dat  bs=1M  count=2048 # génère fichier aléatoire de 2Go
cmake .
make
./codec --help
```

