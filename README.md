# Projet "encodage"


Pour tester:
```sh
cd build
dd if=/dev/urandom of=random.dat  bs=1M  count=1024 # génère fichier aléatoire de 1Go
cmake .
make
./codec --help
```

[![asciicast](https://asciinema.org/a/0zukum9m10u2ty3klnlg4sdky.png)](https://asciinema.org/a/0zukum9m10u2ty3klnlg4sdky?speed=3)
