Reproducer for yubihsm2 pkcs11 find objects bug

# yubihsm setup

Reset the device and load the following 2 asymetric keys:
```
[user@localhost yubihsm-pkcs11-test]$ yubihsm-shell
Using default connector URL: http://localhost:12345
yubihsm> connect
Session keepalive set up to run every 15 seconds
yubihsm> session open 1
Enter password:
Created session 0
yubihsm>  generate asymmetric 0 200 "Dummy PKI Root CA(200)" 1 exportable-under-wrap:sign-attestation-certificate:sign-pkcs:sign-pss rsa2048
Generated Asymmetric key 0x00c8
yubihsm> generate asymmetric 0 65000 "Dummy PKI Root CA(65000)" 1 exportable-under-wrap:sign-attestation-certificate:sign-pkcs:sign-pss rsa2048
Generated Asymmetric key 0xfde8
yubihsm>

```

# Building and test

Requires cmake3 and the yubihsm-shell RPM (testing on Centos 7) - versions 2.3.2 and 2.4.0

Run [build-test.sh](build-test.sh)

# 2.3.2 Output

```
[user@localhost yubihsm-pkcs11-test]$ ./build-test.sh
yubihsm-shell-2.3.2-1.el7.x86_64
Using default connector URL: http://127.0.0.1:12345
Session keepalive set up to run every 15 seconds
Enter password:
Created session 0
Found 2 object(s)
id: 0x00c8, type: asymmetric-key, sequence: 0
id: 0xfde8, type: asymmetric-key, sequence: 0
-- The C compiler identification is GNU 4.8.5
-- The CXX compiler identification is GNU 4.8.5
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc - works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ - works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /apps/yubihsm/yubihsm-pkcs11-test/build
Scanning dependencies of target find_objects
[ 50%] Building C object CMakeFiles/find_objects.dir/find_objects.c.o
[100%] Linking C executable find_objects
[100%] Built target find_objects
START: pkcs11 test app
Got function list
Intialised
Found 1 slots
Session opened
Logged in
Searching for object with id(byte) 200
Found 1 objects
Searching for object with id(short) 200
Found 1 objects
Searching for object with id(long) 200
Found 2 objects
Searching for object with id(short) 65000
Found 1 objects
Searching for object with id(long) 65000
Found 2 objects
Session closed
Finalized
END: pkcs11 test app
```

# 2.4.0 Output

```
[user@localhost yubihsm-pkcs11-test]$ ./build-test.sh
yubihsm-shell-2.4.0-1.el7.x86_64
Using default connector URL: http://localhost:12345
Session keepalive set up to run every 15 seconds
Enter password:
Created session 0
Found 2 object(s)
id: 0x00c8, type: asymmetric-key, algo: rsa2048, sequence: 0 label: Dummy PKI Root CA(200)
id: 0xfde8, type: asymmetric-key, algo: rsa2048, sequence: 0 label: Dummy PKI Root CA(65000)
-- The C compiler identification is GNU 4.8.5
-- The CXX compiler identification is GNU 4.8.5
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc - works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ - works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: /apps/yubihsm/yubihsm-pkcs11-test/build
Scanning dependencies of target find_objects
[ 50%] Building C object CMakeFiles/find_objects.dir/find_objects.c.o
[100%] Linking C executable find_objects
[100%] Built target find_objects
START: pkcs11 test app
Got function list
Intialised
Found 1 slots
Session opened
Logged in
Searching for object with id(byte) 200
Found 0 objects
Searching for object with id(short) 200
Found 1 objects
Searching for object with id(long) 200
Found 0 objects
Searching for object with id(short) 65000
Found 1 objects
Searching for object with id(long) 65000
Found 0 objects
Session closed
Finalized
END: pkcs11 test app
```

# Test results

* If the value of the id can be represented in single byte (ie < 256) and is defined as uint8_t - 2.3.2 finds the public key, 2.4.0 does not (FAIL)
* If the value of the id can be represented in single byte (ie < 256) and is defined as uint16_t - 2.3.2 finds the public key, 2.4.0 also finds the key (PASS)
* If the value of the id can be represented in single byte (ie < 256) and is defined as uint32_t - 2.3.2 finds both keys and 2.4.0 finds neither (FAIL?)
* If the value of the id must be represented in two bytes (ie >= 256 && < 65536) and is defined as uint16_t - 2.3.2 finds the public key, 2.4.0 also finds the key (PASS)
* If the value of the id must be represented in two byte (ie >= 256 && < 65536) and is defined as uint32_t - 2.3.2 finds both keys and 2.4.0 finds neither (FAIL?)