/*
 * Copyright (C) 2011-2021 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "sgx_tsgxssl_t.h"
#include "tcommon.h"

#define FAKE_PIPE_READ_FD	0xFAFAFAFALL
#define FAKE_PIPE_WRITE_FD	0xFBFBFBFBLL

#define ENCLAVE_PAGE_SIZE	0x1000	// 4096 B

extern "C" {

long sgxssl_sysconf(int name)
{
	FSTART;

	// Used by mem_sec.c
	if (name == _SC_PAGESIZE) {
		return ENCLAVE_PAGE_SIZE;
	}

	SGX_UNREACHABLE_CODE(SET_ERRNO);
	FEND;

	return -1;
}

//Process ID is used as RNG entropy, SGXSSL use sgx_get_rand() hence this function is redundant.
//
int sgxssl_getpid() {

    SGX_UNREACHABLE_CODE(SET_ERRNO);
    return 0;
}

//Don't use system calls inside enclave.
//
int sgxssl_OPENSSL_issetugid()
{
    return 1;
}

//ENOSYS indicates that Function not implemented.
//
long sgxssl_syscall (long number, ...) {
    (void)(number);
    
    errno = ENOSYS;
    return -1;
}

int sgxssl_stat(const char *path, struct stat *buf)
{

    SGX_UNREACHABLE_CODE(SET_ERRNO);
    return -1;
}

struct dirent *sgxssl_readdir(void *dirp)
{
    FSTART;

    SGX_UNREACHABLE_CODE(SET_ERRNO);
    FEND;
    return NULL;

}

int sgxssl_closedir(void *dirp)
{
    FSTART;

    SGX_UNREACHABLE_CODE(SET_ERRNO);
    FEND;
    return -1;

}

void *sgxssl_opendir(const char *name)
{
    FSTART;

    SGX_UNREACHABLE_CODE(SET_ERRNO);
    FEND;
    return NULL;
}


} // extern "C"
