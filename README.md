# Composer
Shader file encryption/decryption tool for Halo Custom Edition usin XTEA algorithm.

## Usage

### Encrypt
```bash
D:\shaders> composer-encrypt
usage: composer-encrypt [options] ... <input-file>
options:
  -o, --output    Encrypted shader output file.
  -h, --help      Print this message.

D:\shaders> composer-encrypt shader.bin
encrypted shader file: "shader.enc"
```

### Decrypt
```bash
D:\shaders> composer-decrypt
usage: composer-decrypt [options] ... <input-file>
options:
  -o, --output    Decrypted shader output file.
  -h, --help      Print this message.

D:\shaders> composer-decrypt shader.enc
decrypted shader file: "shader.bin"
```

## Links
- [**hash-library**](https://github.com/stbrumme/hash-library) - hashing library (see [license](/licenses/hash-library))
- [**cmdline**](https://github.com/tanakh/cmdline) - command line parser library (see [license](/licenses/cmdline))
