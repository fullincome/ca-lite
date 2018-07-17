# ca-lite 
based on openssl and cryptopro CSP.

## Supported OS:
Ubuntu-16.04

## For use ca: 
1. **Download** last version **ca-lite** (or build from source):  
[Release (linux-x64)](https://github.com/fullincome/ca-lite/releases/download/ca-lite-1.01/ca-lite-1.01-release_linux-amd64.tar.gz)  
[Debug (linux-x64)](https://github.com/fullincome/ca-lite/releases/download/ca-lite-1.01/ca-lite-1.01-debug_linux-amd64.tar.gz)  
2. Run **./install_dependencies.sh**
3. If need test keys - **gen_key.sh**

Easy way to build ca: **./configure && make**.

##Warning

To obtain the permission to generate keys GOST R 34.10-2001 use by root (sudo) **allow_keys2001.sh** before installation.
