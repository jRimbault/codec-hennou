# Projet "encodage"


Pour tester:
```sh
cd build
dd if=/dev/urandom of=random.dat  bs=1M  count=1024 # génère fichier aléatoire de 1Go
cmake .
make
./codec --help
```

<script type="text/javascript" src="https://asciinema.org/a/7gpuugdqxzplow2vk4n8vgml5.js" id="asciicast-7gpuugdqxzplow2vk4n8vgml5" async></script>

