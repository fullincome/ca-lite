#!/bin/bash -x

# Настройка необходимых пакетов
# ----------------------------------

# Пакеты будут скачены с "$url"
url="https://update.cryptopro.ru/support/nginx-gost"

revision_openssl="168667"

cat /etc/*release* | grep -Ei "(centos|red hat)"
if [ "$?" -eq 0 ] 
then
    apt="yum -y"
    pkgmsys="rpm"
    pkglist="rpm -qa"
    install="rpm -i"
    openssl_packages=(cprocsp-cpopenssl-110-base-5.0.10976-5.noarch.rpm \
    cprocsp-cpopenssl-110-64-5.0.10976-5.x86_64.rpm \
    cprocsp-cpopenssl-110-devel-5.0.10976-5.noarch.rpm \
    cprocsp-cpopenssl-110-gost-64-5.0.10976-5.x86_64.rpm)
else
    cat /etc/*release* | grep -Ei "(ubuntu|debian)"
    if [ "$?" -eq 0 ] 
    then
        apt="apt-get"
        pkgmsys="deb"
        pkglist="dpkg-query --list"
        install="dpkg -i"
	openssl_packages=(cprocsp-cpopenssl-110-base_5.0.10976-5_all.deb \
	cprocsp-cpopenssl-110-64_5.0.10976-5_amd64.deb \
	cprocsp-cpopenssl-110-devel_5.0.10976-5_all.deb \
	cprocsp-cpopenssl-110-gost-64_5.0.10976-5_amd64.deb)
    else
        printf "Not supported system (supported: Ubuntu, CentOS, Red Hat)."
        exit 0
    fi
fi

dpkg-query --list | grep -Ei "csp"
if [ "$?" -ne 0 ] 
then
    if [ -n "$1" ] 
    then
        csp=$1
    if ! [ -d "$csp" ]
        then
        if ! [ -d csp ]
        then
            mkdir csp
        fi
        tar -xzvf $csp -C csp --strip-components 1 || unzip -j $csp -d csp || exit 1
        csp="csp"
        cd ${csp} && ./install.sh && cd .. || exit 1
    fi
    else
        printf "No argument (CSP)"
        exit 0
    fi
fi


for i in ${openssl_packages[@]}; do wget --no-check-certificate -O $i ${url}/bin/"${revision_openssl}"/$i || exit 1; done

for i in ${openssl_packages[@]}; do
    cmd=$install" "$i
    eval "$cmd" || exit 1
done

# ----------------------------------
