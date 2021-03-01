#include "StdAfx.h"
#include "algo_hmac.h"
#include "openssl\hmac.h"
#include <string.h>
#include <iostream>

int HmacEncode(const char * algo, 
		const char * key, unsigned int key_length, 
		const char * input, unsigned int input_length, 
		unsigned char * &output, unsigned int &output_length) {
	const EVP_MD * engine = NULL;
	if(strcmp("sha512", algo) == 0) {
		engine = EVP_sha512();
	}
	else if(strcmp("sha256", algo) == 0) {
		engine = EVP_sha256();
	}
	else if(strcmp("sha1", algo) == 0) {
		engine = EVP_sha1();
	}
	else if(strcmp("md5", algo) == 0) {
		engine = EVP_md5();
	}
	else if(strcmp("sha224", algo) == 0) {
		engine = EVP_sha224();
	}
	else if(strcmp("sha384", algo) == 0) {
		engine = EVP_sha384();
	}
//	else if(strcmp("sha", algo) == 0) {
//		engine = EVP_sha();
//	}
//	else if(strcmp("md2", algo) == 0) {
//		engine = EVP_md2();
//	}
	else {
		cout << "Algorithm " << algo << " is not supported by this program!" << endl;
		return -1;
	}

	output = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
	
	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(key), engine, NULL);
	HMAC_Update(&ctx, (unsigned char*)input, strlen(input));	// input is OK; &input is WRONG !!!

	HMAC_Final(&ctx, output, &output_length);
	HMAC_CTX_cleanup(&ctx);	

	return 0;
}

