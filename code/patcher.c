#include <stdio.h>
#include <string.h>

#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/mman.h>

const char OfficialPublicKey[] = {
    "-----BEGIN PUBLIC KEY-----\r\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwknN5/81qIoM+k1WEc/5\r\n"
    "A+u3YzWeDNE5i5rpayiNWv4TvxX9udMz3uKF4tmDJfjuSCVUHQWAjJefwSv4ZBQ7\r\n"
    "egXHtGzh/HVY/Geei7A1TwOroHxxgUKVkXRoPGYIVNBhrqX0CsCgYCZv7Ij6CSy4\r\n"
    "Iamw0Gb/BnZ4Uw1AkOs6DfbGaVspeQGg7oTjSj+TKOWdK1dJ7ZoGyoVQLqxpaq41\r\n"
    "r893XqSK5mXlbE3pFSjS6Rn8unyt57EuIcFy+x2VqjOf3KiwiGCeIaQtAtF5Pqii\r\n"
    "KoEMoGgcDedO4C6h/LKy1oXJR7QeHQijUAn7JXE/i0I57BXyKylR6/YIV6hV+Ovm\r\n"
    "bQIDAQAB\r\n"
    "-----END PUBLIC KEY-----"
};

extern RSA* LoadRSAKeyFromFile(const char* lpcszFilePath);
extern void PrintRSAKeyInPEM(RSA* lpRSAKey, int Private);

RSA* GenerateRSAKey(int bits) {
    int success = 0;
    RSA* lpRSAKey = NULL;
    BIGNUM* e = NULL;

    lpRSAKey = RSA_new();
    if (lpRSAKey == NULL)
        goto ON_GenerateRSAKey_ERROR;

    e = BN_new();
    if (e == NULL)
        goto ON_GenerateRSAKey_ERROR;

    if (!BN_set_word(e, 65537))
        goto ON_GenerateRSAKey_ERROR;

    if (!RSA_generate_key_ex(lpRSAKey, bits, e, NULL))
        goto ON_GenerateRSAKey_ERROR;

    success = 1;

ON_GenerateRSAKey_ERROR:
    if (e)
        BN_free(e);
    if (!success && lpRSAKey) {
        RSA_free(lpRSAKey);
        lpRSAKey = NULL;
    }
    return lpRSAKey;
}

int ReadRSAKeyInPEM(RSA* lpRSAKey, char* buf) {
    int status = 0;
    BIO* bio_buf = NULL;
    long pem_len = 0;
    char* pem_data = NULL;

    if (lpRSAKey == NULL || buf == NULL)
        return 0;

    bio_buf = BIO_new(BIO_s_mem());
    if (bio_buf == NULL)
        goto ON_ReadRSAKeyInPEM_ERROR;

    if (!PEM_write_bio_RSA_PUBKEY(bio_buf, lpRSAKey))
        goto ON_ReadRSAKeyInPEM_ERROR;

    pem_len = BIO_get_mem_data(bio_buf, &pem_data);
    if (pem_len == 0)
        goto ON_ReadRSAKeyInPEM_ERROR;

    pem_len--;
    for (int i = 0, write_ptr = 0; i < pem_len; ++i) {
        if (pem_data[i] == '\n')
            buf[write_ptr++] = '\r';
        buf[write_ptr++] = pem_data[i];
    }

    status = 1;

ON_ReadRSAKeyInPEM_ERROR:
    if (bio_buf)
        BIO_free_all(bio_buf);
    return status;
}

int SaveRSAKeyToFile(RSA* lpRSAKey, const char* lpcszFilePath) {
    int success = 0;
    BIO* bio_file = NULL;

    if (lpRSAKey == NULL || lpcszFilePath == NULL)
        return 0;

    bio_file = BIO_new_file(lpcszFilePath, "w");
    if (bio_file == NULL)
        goto ON_SaveRSAKeyToFile_ERROR;

    if (!PEM_write_bio_RSAPrivateKey(bio_file, lpRSAKey, NULL, NULL, 0, NULL, NULL))
        goto ON_SaveRSAKeyToFile_ERROR;

    success = 1;
ON_SaveRSAKeyToFile_ERROR:
    if (bio_file)
        BIO_free_all(bio_file);
    return success;
}

off_t SearchOfficialPublicKey(uint8_t* lpFileContent, size_t FileSize, off_t start_offset) {
    off_t ret = -1;
    for (off_t i = start_offset; i + sizeof(uintptr_t) < FileSize; ++i)
        if (*(uintptr_t*)(lpFileContent + i) == *(uintptr_t*)OfficialPublicKey)
            if (0 == memcmp(lpFileContent + i, OfficialPublicKey, sizeof(OfficialPublicKey))) {
                ret = (off_t)i;
                break;
            }

    return ret;
}

void DoPatch(uint8_t* lpFileContent, off_t Offset, const uint8_t* lpcNewPublicKey, size_t KeySize) {
    memset(lpFileContent + Offset, 0, sizeof(OfficialPublicKey));
    memcpy(lpFileContent + Offset, lpcNewPublicKey, KeySize);
}

int OpenFile(const char* lpszFilePath) {
    int fd = open(lpszFilePath, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
        printf("Failed to open file. CODE: 0x%08x\n", errno);

    return fd;
}

size_t GetFileSize(int fd) {
    struct stat fd_stat = {};
    if (fstat(fd, &fd_stat) != 0) {
        printf("Failed to get file size. CODE: 0x%08x\n", errno);
        return (size_t)-1;
    } else {
        return (size_t)fd_stat.st_size;
    }
}

void* MapFileContent(int fd, size_t map_size) {
    void* lpFileContent = (void*)-1;
    lpFileContent = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (lpFileContent == (void*)-1) {
        printf("Failed to map file. CODE: 0x%08x\n", errno);
        return NULL;
    } else {
        return lpFileContent;
    }
}

void PatchRoutine(const char* lpcszTargetFilePath, RSA* lpRSAKey) {
    int fd = -1;
    size_t stFileSize = 0;
    uint8_t* lpFileContent = NULL;
    char pem_data[1024] = {};

    if (!ReadRSAKeyInPEM(lpRSAKey, pem_data)) {
        printf("Failed to convert RSA key to PEM format.\n");
        return;
    }

    if (strlen(OfficialPublicKey) != strlen(pem_data)) {
        printf("PEM data length is different from Official key's. To avoid file broken, patch abort.\n");
        return;
    }

    printf("Target file: %s\n", lpcszTargetFilePath);
    fd = OpenFile(lpcszTargetFilePath);
    if (fd == -1)
        goto ON_PatchRoutine_ERROR;

    printf("Open file successfully!\n");

    stFileSize = GetFileSize(fd);
    if (stFileSize == (size_t)-1)
        goto ON_PatchRoutine_ERROR;

    printf("File size: %zu byte(s).\n", stFileSize);

    lpFileContent = MapFileContent(fd, stFileSize);
    if (lpFileContent == NULL)
        goto ON_PatchRoutine_ERROR;

    printf("Map file successfully!\n");

    int found = 0;
    off_t offset = -1;
    while (1) {
        offset = SearchOfficialPublicKey(lpFileContent, stFileSize, offset + 1);
        if (offset == -1)
            break;
        found++;

        printf("offset = 0x%016llx, writing data.....", offset);
        DoPatch(lpFileContent, offset, pem_data, strlen(pem_data));
        printf("Patch is done.\n");
    }

    printf("Modified: %d\n\n", found);

ON_PatchRoutine_ERROR:
    if (lpFileContent)
        munmap(lpFileContent, stFileSize);
    if (fd != -1)
        close(fd);
}

void help() {
    printf("Usage:\n");
    printf("    ./patcher <path to CommanderOne MachO file> [RSA-2048 Private Key (PEM file)]\n");
    printf("\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        help();
        return 0;
    }

    RSA* lpRSAKey = NULL;

    if (argc == 3) {
        lpRSAKey = LoadRSAKeyFromFile(argv[2]);
        if (lpRSAKey == NULL) {
            printf("Failed to load RSA private key.\n");
            goto ON_main_ERROR;
        }
    } else {
        lpRSAKey = GenerateRSAKey(2048);
        if (lpRSAKey == NULL) {
            printf("Failed to generate RSA key.\n");
            goto ON_main_ERROR;
        }
    }

    printf("Your RSA public key is\n");
    printf("\n");
    PrintRSAKeyInPEM(lpRSAKey, 0);
    printf("\n");

    if (!SaveRSAKeyToFile(lpRSAKey, "private.pem")) {
        printf("Failed to save RSA key.\n");
        goto ON_main_ERROR;
    }

    if (RSA_size(lpRSAKey) * 8 != 2048) {
        printf("Not an RSA-2048 key. Patch abort.\n");
        goto ON_main_ERROR;
    }

    PatchRoutine(argv[1], lpRSAKey);
ON_main_ERROR:
    if (lpRSAKey)
        RSA_free(lpRSAKey);
    return 0;
}
