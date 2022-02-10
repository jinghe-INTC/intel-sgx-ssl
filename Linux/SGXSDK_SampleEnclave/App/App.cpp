/*
 * Copyright (C) 2011-2016 Intel Corporation. All rights reserved.
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


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
    	printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(void)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    
    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        return -1;
    }

    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}

void* thread_test_func(void* p)
{
	new_thread_func(global_eid);
	return NULL;
}

int ucreate_thread()
{
	pthread_t thread;
	int res = pthread_create(&thread, NULL, thread_test_func, NULL);
	return res;
}

/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);
    int retval;

    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        printf("Enter a character before exit ...\n");
        getchar();
        return -1; 
    }

    if ( SGX_SUCCESS != rsa_key_gen(global_eid))
    {
	    printf("rsa_key_gen failed\n");
	    goto exit;
    }
    printf("test rsa_key_gen completed\n");


    if ( SGX_SUCCESS != ec_key_gen(global_eid))
    {
	    printf("ec_key_gen failed\n");
	    goto exit;
    }
    printf("test ec_key_gen completed\n");


    retval = -1;
    if ( SGX_SUCCESS != rsa_test(global_eid, &retval)) 
    {
	    printf("test rsa_test ecall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test rsa_test returned error %d\n", retval);
            goto exit;
    } else printf("test rsa_test completed\n");

    retval = -1;
    if ( SGX_SUCCESS != bn_test(global_eid, &retval)) 
    {
	    printf("test bn_test ecall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test bn_test returned error %d\n", retval);
            goto exit;
    } else printf("test bn_test completed\n");

    retval = -1;
    if ( SGX_SUCCESS != ecdh_test(global_eid, &retval)) 
    {
	    printf("test ecdh_test ecall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test ecdh_test returned error %d\n", retval);
            goto exit;
    } else printf("test ecdh_test completed\n");

    retval = -1;
    if ( SGX_SUCCESS != dh_test(global_eid, &retval)) 
    {
	    printf("test dh_test ecall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test dh_test returned error %d\n", retval);
            goto exit;
    } else printf("test dh_test completed\n");

    retval = -1;
    if ( SGX_SUCCESS != ecdsa_test(global_eid, &retval))
    {
	    printf("test ecdsa_test ecall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test ecdsa_test returned error %d\n", retval);
            goto exit;
    } else printf("test ecdsa_test completed\n");

    retval = -1;
    if ( SGX_SUCCESS != sha1_test(global_eid, &retval)) 
    {
	    printf("test sha1_test ecall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test sha1_test returned error %d\n", retval);
            goto exit;
    } else printf("test sha1_test completed\n");

    retval = -1;
    if ( SGX_SUCCESS != sha256_test(global_eid, &retval)) 
    {
	    printf("test sha256_test ecall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test sha256_test returned error %d\n", retval);
            goto exit;
    } else printf("test sha256_test completed\n");

    retval = -1;
    if ( SGX_SUCCESS != threads_test(global_eid, &retval))
    {
	    printf("test threads_test ecall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test threads_test returned error %d\n", retval);
            goto exit;
    } else printf("test threads_test completed\n");
	
   //GM SM2 - sign and verify
    retval = -1;
    if ( SGX_SUCCESS != ecall_sm2(global_eid, &retval)) 
    {
	    printf("test ecall_sm2 ecdhall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test ecall_sm2 returned error %d\n", retval);
            goto exit;
    } else printf("test ecall_sm2 completed\n");

    //GM SM3 - compute digest of message
    retval = -1;
    if ( SGX_SUCCESS != ecall_sm3(global_eid, &retval)) 
    {
	    printf("test ecall_sm3 ecdhall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test ecall_sm3 returned error %d\n", retval);
            goto exit;
    } else printf("test ecall_sm3 completed\n");

    //GM SM4 - cbc encrypt and decrypt
    retval = -1;
    if ( SGX_SUCCESS != ecall_sm4_cbc(global_eid, &retval)) 
    {
	    printf("test ecall_sm4_cbc ecdhall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test ecall_sm4_cbc returned error %d\n", retval);
            goto exit;
    } else printf("test ecall_sm4_cbc completed\n");

    //GM SM4 - ctr encrypt and decrypt
    retval = -1;
    if ( SGX_SUCCESS != ecall_sm4_ctr(global_eid, &retval)) 
    {
	    printf("test ecall_sm4_ctr ecdhall failed\n");
	    goto exit;
    }
    if ( 0 != retval)
    {
            printf("test ecall_sm4_ctr returned error %d\n", retval);
            goto exit;
    } else printf("test ecall_sm4_ctr completed\n");

    printf("Info: SampleEnclave successfully returned.\n");
exit:
    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);
    printf("Enter a character before exit ...\n");
    getchar();
    return 0;
}

