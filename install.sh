#!/bin/bash

# Утановка openssl-1.1.0 вместе с gostengy
./scripts/install_dependencies.sh || exit 1

# Разрешение на генерацию ключей по GOST R 34.10.2001
./scripts/allow_keys2001.sh || exit 1

# Сохраняем конфиг для УЦ
cp ./openssl_calite.cnf /var/opt/cprocsp/cp-openssl-1.1.0/openssl_calite.cnf || exit 1
