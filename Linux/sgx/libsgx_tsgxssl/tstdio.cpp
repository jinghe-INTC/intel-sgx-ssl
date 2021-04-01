/*
 * Copyright (C) 2011-2017 Intel Corporation. All rights reserved.
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
#include "tcommon.h"
#include "sgx_tsgxssl_t.h"
#include "tSgxSSL_api.h"

extern PRINT_TO_STDOUT_STDERR_CB s_print_cb;

struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };

char *s_stdin_ptr = "0";
char *s_stdout_ptr = "1";
char *s_stderr_ptr = "2";

int print_with_cb(void* fp, const char* fmt, __va_list vl)
{
        int res = -1;

        SGX_ASSERT(fp != NULL && sgx_is_within_enclave(fp, 1) && s_print_cb != NULL);

        int stream = -1;
        if (((struct _iobuf *)fp)->_ptr == s_stdout_ptr) {
                stream = STREAM_STDOUT;
        }
        else if (((struct _iobuf *)fp)->_ptr == s_stderr_ptr) {
                stream = STREAM_STDERR;
        }
        else {
                // This function is called only when fp is one of the internally implemented stdout/stderr.
                SGX_ASSERT(FALSE);
                return res;
        }

        res = s_print_cb((Stream_t)stream, fmt, vl);
        return res;
}

bool is_fake_stdout_stderr(void* fp)
{
        if (fp != NULL &&
                sgx_is_within_enclave(fp, 1) &&
                (       ((struct _iobuf *)fp)->_ptr == s_stdout_ptr
                        || ((struct _iobuf *)fp)->_ptr == s_stderr_ptr) ) {
                return TRUE;
        }

        return FALSE;
}

extern "C" {

#ifndef SUPPORT_FILES_APIS

/////////////////////
// void* functions //
/////////////////////

void* __attribute__((weak)) sgxssl_fopen(const char* filename, const char* mode)
{
        FSTART;

        // A null pointer return value indicates an error.
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;

        return NULL;
}

int __attribute__((weak)) sgxssl_fclose(void* fp)
{
        FSTART;

        //  Return EOF (-1) to indicate an error
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;

        return EOF;
}


int __attribute__((weak)) sgxssl_ferror(void* fp)
{
        FSTART;

        int ret = 0;

        if (is_fake_stdout_stderr(fp) &&
                s_print_cb != NULL) {

                FEND;

                // If no error has occurred on stream, ferror returns 0.
                return ret;
        }

        // On error returns a nonzero value
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;

        return -1;
}

int __attribute__((weak)) sgxssl_feof(void* fp)
{
        FSTART;

        // The feof function returns a nonzero value if a read operation has attempted to read past the end of the file;
        // it returns 0 otherwise. ( On error returns 0 as well)
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;
        return 0;
}

int __attribute__((weak)) sgxssl_fflush(void* fp)
{
        FSTART;

        if (is_fake_stdout_stderr(fp) &&
                s_print_cb != NULL) {

                FEND;
                // fflush returns 0 if the buffer was successfully flushed.
                return 0;
        }

        // A return value of EOF (-1) indicates an error.
        SGX_UNSUPPORTED_FUNCTION(SET_NO_ERRNO);

        FEND;
        return -1;
}

long __attribute__((weak)) sgxssl_ftell(void* fp)
{
        FSTART;

        long ret = -1;

        //  If execution is allowed to continue, these functions return <96>1L
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;
        return ret;
}

int __attribute__((weak)) sgxssl_fseek(void* fp, long offset, int origin)
{
        FSTART;

        int ret = -1;

        // On error, returns a nonzero value
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;
        return ret;
}


int __attribute__((weak)) sgxssl_fprintf(void* fp, const char* fmt, ...)
{
        FSTART;

        if (is_fake_stdout_stderr(fp) &&
                s_print_cb != NULL) {
                va_list vl;
                va_start(vl, fmt);
                int res = print_with_cb(fp, fmt, vl);
                va_end(vl);

                FEND;
                return res;
        }

        int ret = -1;

        // Returns a negative value when an output error occurs.
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;
        return ret;
}


int __attribute__((weak)) sgxssl_vfprintf(void* fp, const char* fmt, va_list vl)
{
        FSTART;

        if (is_fake_stdout_stderr(fp) &&
                s_print_cb != NULL) {
                int res = print_with_cb(fp, fmt, vl);

                FEND;
                return res;
        }

        int ret = -1;
        //  Returns a negative value if an output error occurs and no characters were written
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;
        return ret;
}

size_t __attribute__((weak)) sgxssl_fread(void* dest, size_t element_size, size_t cnt, void* fp)
{
        FSTART;

        // returns 0 on error
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;
        return 0;
}


size_t __attribute__((weak)) sgxssl_fwrite(const void* src, size_t element_size, size_t cnt, void* fp)
{
        FSTART;

        if (is_fake_stdout_stderr(fp) &&
                s_print_cb != NULL) {
                int res = sgxssl_fprintf(fp, "%.*s", element_size*cnt, src);

                FEND;
                return res;
        }

        // Returns 0 on error
        // On Failure errno will be set. No impact on LastError.
        SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

        FEND;
        return 0;
}

char* __attribute__((weak)) sgxssl_fgets(char* dest, int max_cnt, void* fp)
{
	FSTART;

	// NULL is returned to indicate an error
	// On Failure errno will be set. No impact on LastError.
	SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

	FEND;
	return NULL;
}


int __attribute__((weak)) sgxssl_fputs(const char* src, void* fp)
{
	FSTART;

	if (is_fake_stdout_stderr(fp) &&
		s_print_cb != NULL) {
		int res = sgxssl_fprintf(fp, "%s", src);
		FEND;
		return res;
	}

	// On error, returns EOF (-1)
	// On Failure errno will be set. No impact on LastError.
	SGX_UNSUPPORTED_FUNCTION(SET_ERRNO);

	FEND;
	return EOF;
}

#endif // SUPPORT_FILES_APIS is not defined

int sgx_print(const char *format, ...)
{
	if (s_print_cb != NULL) {
		va_list vl;
		va_start(vl, format);
		int res = s_print_cb(STREAM_STDOUT, format, vl);
		va_end(vl);

		return res;
	}
	
	return 0;
}

}
