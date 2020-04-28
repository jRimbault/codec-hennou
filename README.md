# Projet "encodage"

![CI](https://github.com/jRimbault/codech/workflows/CI/badge.svg?branch=master)
[![codecov](https://codecov.io/gh/jRimbault/codech/branch/master/graph/badge.svg)](https://codecov.io/gh/jRimbault/codech)

Pour tester:
```sh
dd if=/dev/urandom of=random.dat  bs=1M  count=1024 # génère fichier aléatoire de 1Go
make
./target/release/codech --help
```
Ou, plus simplement:
```sh
make bench
```

[![asciicast](https://asciinema.org/a/bxk1smRrOWv9kcjtnso12eqAV.svg)](https://asciinema.org/a/bxk1smRrOWv9kcjtnso12eqAV)

Benchmarks éxécutés sur Dell XPS 15 9570 (2018)

- CPU i9-8950HK
- stockage M.2 2280
