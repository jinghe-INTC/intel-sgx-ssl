------------------------
Purpose of SampleEnclave
------------------------
The project demonstrates the usages of Intel(R) Software Guard 
Extensions (Intel(R) SGX) SDK:
- Initializing and destroying an enclave
- Creating ECALLs or OCALLs
- Calling SGXSSL cryptographical function inside the enclave

------------------------------------
How to Build/Execute the Sample Code
------------------------------------
1. Install Intel(R) SGX SDK for Linux* OS, which is built with SGX SSL instead of IPP, by running "make sdk USE_OPT_LIBS=0" or "make sdk USE_OPT_LIBS=2"
2. Make sure your environment is set:
    $ source ${sgx-sdk-install-path}/environment
3. Build and install OpenSSL, for the native header files located at /usr/local/include/openssl/ .
4. Build the project with the prepared Makefile:
    a. Hardware Mode, Debug build:
        $ make
    b. Hardware Mode, Pre-release build:
        $ make SGX_PRERELEASE=1 SGX_DEBUG=0
    c. Hardware Mode, Release build:
        $ make SGX_DEBUG=0
    d. Simulation Mode, Debug build:
        $ make SGX_MODE=SIM
    e. Simulation Mode, Pre-release build:
        $ make SGX_MODE=SIM SGX_PRERELEASE=1 SGX_DEBUG=0
    f. Simulation Mode, Release build:
        $ make SGX_MODE=SIM SGX_DEBUG=0
5. Execute the binary directly:
    $ ./app
6. Remember to "make clean" before switching build mode
