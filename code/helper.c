#include <stdio.h>

#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

RSA* LoadRSAKeyFromFile(const char* lpcszFilePath) {
    int success = 0;
    RSA* lpRSAKey = NULL;
    BIO* bio_file = NULL;

    bio_file = BIO_new(BIO_s_file());
    if (bio_file == NULL)
        goto ON_LoadRSAKeyFromFile_ERROR;

    if (!BIO_read_filename(bio_file, lpcszFilePath))
        goto ON_LoadRSAKeyFromFile_ERROR;

    lpRSAKey = PEM_read_bio_RSAPrivateKey(bio_file, NULL, NULL, NULL);
    if (lpRSAKey == NULL)
        goto ON_LoadRSAKeyFromFile_ERROR;

    success = 1;
    ON_LoadRSAKeyFromFile_ERROR:
    if (bio_file)
        BIO_free_all(bio_file);
    if (!success && lpRSAKey) {
        RSA_free(lpRSAKey);
        lpRSAKey = NULL;
    }
    return lpRSAKey;
}

void PrintRSAKeyInPEM(RSA* lpRSAKey, int Private) {
    if (lpRSAKey == NULL)
        return;

    BIO* bio_buf = NULL;
    char* pem_data = NULL;

    bio_buf = BIO_new(BIO_s_mem());
    if (bio_buf == NULL)
        return;

    if (Private) {
        if (!PEM_write_bio_RSAPrivateKey(bio_buf, lpRSAKey, NULL, NULL, 0, NULL, NULL)) {
            BIO_free_all(bio_buf);
            return;
        }
    } else {
        if (!PEM_write_bio_RSA_PUBKEY(bio_buf, lpRSAKey)) {
            BIO_free_all(bio_buf);
            return;
        }
    }

    if (!BIO_get_mem_data(bio_buf, &pem_data)) {
        BIO_free_all(bio_buf);
        return;
    }

    printf("%s", pem_data);
    BIO_free_all(bio_buf);
}


