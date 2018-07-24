#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <iomanip>

#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

extern RSA* LoadRSAKeyFromFile(const char* lpcszFilePath);
extern void PrintRSAKeyInPEM(RSA* lpRSAKey, int Private);

std::string GenerateActivationCode() {
    static char HexTable[] = "0123456789ABCDEF";

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<int> dist(0, 15);

    std::string ret;
    for (int i = 0; i < 25; ++i) {
        if (i && i % 5 == 0)
            ret += '-';
        ret += HexTable[dist(rng)];
    }

    return ret;
}

void ReadActivateInfo(const char* lpcszFilePath, std::string& ActivateInfo) {
    std::ifstream File;
    File.open(lpcszFilePath);
    File >> ActivateInfo;
    File.close();
}

template <typename _strType0, typename _strType1>
void AddInfo(std::string& ActivateInfo, _strType0 Key, _strType1 Value) {
    ActivateInfo += '&';
    ActivateInfo += Key;
    ActivateInfo += '=';
    ActivateInfo += Value;
}

int GenerateLicenseBin(std::string& ActivateInfo, RSA* lpRSAKey) {
    int success = 0;
    auto blocks = (ActivateInfo.length() + 199 ) / 200;
    BIO* bio_file = nullptr;

    bio_file = BIO_new_file("License.bin", "wb");
    if (bio_file == nullptr)
        goto ON_GenerateLicenseBin_ERROR;

    for (unsigned long i = 0; i < blocks; ++i) {
        unsigned char enc_data[256] = {};
        if (!RSA_private_encrypt(ActivateInfo.length() - i * 200 > 200 ? 200 : (ActivateInfo.length() - i * 200),
                                 reinterpret_cast<const unsigned char*>(ActivateInfo.c_str() + i * 200),
                                 enc_data,
                                 lpRSAKey,
                                 RSA_PKCS1_PADDING))
            goto ON_GenerateLicenseBin_ERROR;
        if (0 >= BIO_write(bio_file, enc_data, sizeof(enc_data)))
            goto ON_GenerateLicenseBin_ERROR;
    }

    success = 1;

ON_GenerateLicenseBin_ERROR:
    if (bio_file)
        BIO_free_all(bio_file);
    return success;
}

void help() {
    std::cout << "Usage:" << std::endl;
    std::cout << "    ./keygen <path to Commander One.activate> <path to private.pem>" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Activation Code:" << std::endl
                  << GenerateActivationCode() << std::endl
                  << std::endl;
        return 0;
    } else if (argc == 3) {
        RSA* lpRSAKey = nullptr;
        std::string ActivateInfo;
        std::string Name;
        std::time_t ServerTime = 0;
        std::tm ServerTM = {};
        char szDate[32] = {};

        lpRSAKey = LoadRSAKeyFromFile(argv[2]);
        if (lpRSAKey == nullptr)
            goto ON_main_ERROR;

        if (RSA_size(lpRSAKey) * 8 != 2048) {
            std::cout << "Not an RSA-2048 key. Abort!" << std::endl;
            goto ON_main_ERROR;
        }

        std::cout << "Your RSA private key is" << std::endl << std::endl;
        PrintRSAKeyInPEM(lpRSAKey, 1);
        std::cout << std::endl;

        ReadActivateInfo(argv[1], ActivateInfo);
        if (ActivateInfo.empty()) {
            std::cout << "Failed to read .activate file.\n";
            goto ON_main_ERROR;
        }

        std::time(&ServerTime);
        ServerTM = *std::localtime(&ServerTime);
        std::strftime(szDate, sizeof(szDate), "%B %d, %Y", &ServerTM);

        std::cout << "Input your name:";
        std::getline(std::cin, Name);

        AddInfo(ActivateInfo, "errorCode", "0");
        AddInfo(ActivateInfo, "key_type", "0");
        AddInfo(ActivateInfo, "registed_name", Name);
        AddInfo(ActivateInfo, "licenseName", "PRO Version");
        AddInfo(ActivateInfo, "serverTime", std::to_string(ServerTime));
        AddInfo(ActivateInfo, "serverDate", szDate);
        AddInfo(ActivateInfo, "activationDate", szDate);
        AddInfo(ActivateInfo, "firstActivation", szDate);

        if (!GenerateLicenseBin(ActivateInfo, lpRSAKey)) {
            std::cout << "Failed to generate License.bin\n" << std::endl;
            goto ON_main_ERROR;
        }

        std::cout << "License.bin is generated successfully.\n" << std::endl;
ON_main_ERROR:
        if (lpRSAKey)
            RSA_free(lpRSAKey);
    } else {
        help();
    }

    return 0;
}
