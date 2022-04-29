#pragma once

#include <iomanip>
#include <iostream>

#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/modes.h>
#include <cryptopp/sha.h>

#define KEYLENGTH CryptoPP::AES::MAX_KEYLENGTH
#define IVLENGTH CryptoPP::AES::BLOCKSIZE

namespace enc {

void set_key(CryptoPP::byte* key, std::string pass) {
    for (int i = 0; (i < pass.length()) && (i < KEYLENGTH); i++) {
        int index = (i >= KEYLENGTH) ? (i - KEYLENGTH) : (i);

        int current_val = int(key[index]);
        int new_val = current_val + int(pass[i]);
        key[index] = CryptoPP::byte(new_val);
    }
}
void set_iv(CryptoPP::byte* iv, std::string pass) {
    for (int i = 0; (i < pass.length()) && (i < IVLENGTH); i++) {
        int index = (i >= IVLENGTH) ? (i - IVLENGTH) : (i);

        int current_val = int(iv[index]);
        int new_val = int(pass[i]) % current_val;
        iv[index] = CryptoPP::byte(new_val);
    }
}

std::string encrypt(std::string decrypted_str, std::string pass) {
    CryptoPP::byte key[KEYLENGTH];
    CryptoPP::byte iv[IVLENGTH];
    memset(key, pass.length(), KEYLENGTH);
    memset(iv, pass.length(), IVLENGTH);

    set_key(key, pass);
    set_iv(iv, pass);

    std::string cypher_str;

    CryptoPP::AES::Encryption aes_encryption(key, KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbc_encryption(aes_encryption,
                                                                 iv);

    CryptoPP::StreamTransformationFilter stf_encryptor(
        cbc_encryption, new CryptoPP::StringSink(cypher_str));
    stf_encryptor.Put(
        reinterpret_cast<const unsigned char*>(decrypted_str.c_str()),
        decrypted_str.length());
    stf_encryptor.MessageEnd();

    return cypher_str;
}

std::string decrypt(std::string cypher_str, std::string pass) {
    CryptoPP::byte key[KEYLENGTH];
    CryptoPP::byte iv[IVLENGTH];
    memset(key, pass.length(), KEYLENGTH);
    memset(iv, pass.length(), IVLENGTH);

    set_key(key, pass);
    set_iv(iv, pass);

    std::string decrypted_str;

    CryptoPP::AES::Decryption aes_decryption(key, KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbc_decryption(aes_decryption,
                                                                 iv);

    CryptoPP::StreamTransformationFilter stf_decryptor(
        cbc_decryption, new CryptoPP::StringSink(decrypted_str));
    stf_decryptor.Put(
        reinterpret_cast<const unsigned char*>(cypher_str.c_str()),
        cypher_str.length());
    stf_decryptor.MessageEnd();

    return decrypted_str;
}

}  // namespace enc