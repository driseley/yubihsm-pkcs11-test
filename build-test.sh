rpm -qa | grep yubihsm-shell
yubihsm-shell -a list-objects -A rsa2048 -t asymmetric-key
rm -rf build
mkdir build
cd build
cmake3 ../
cp ../yubihsm_pkcs11.conf .
make
rm -rf yubihsm_pkcs11_debug
./find_objects
