#pragma once

#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>

#include <iomanip>
#include <iostream>

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

void generate_private_and_public_keys(std::string* private_key,
                                      std::string* public_key) {
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 3072);

    CryptoPP::RSA::PrivateKey privkey(params);
    CryptoPP::RSA::PublicKey pubkey(params);

    CryptoPP::Base64Encoder privkeysink(new CryptoPP::StringSink(*private_key));
    privkey.DEREncode(privkeysink);
    privkeysink.MessageEnd();

    CryptoPP::Base64Encoder pubkeysink(new CryptoPP::StringSink(*public_key));
    pubkey.DEREncode(pubkeysink);
    pubkeysink.MessageEnd();
}
std::string encrypt_with_public_key(std::string message,
                                    std::string public_key) {
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::RSA::PublicKey key;
    CryptoPP::ByteQueue bytes;
    CryptoPP::StringSource src(public_key, true, new CryptoPP::Base64Decoder);
    src.TransferTo(bytes);
    bytes.MessageEnd();
    key.Load(bytes);

    CryptoPP::RSAES_PKCS1v15_Encryptor e(key);
    std::string encrypted_message;

    CryptoPP::StringSource(
        message, true,
        new CryptoPP::PK_EncryptorFilter(
            rng, e, new CryptoPP::StringSink(encrypted_message)));

    return encrypted_message;
}
std::string decrypt_with_private_key(std::string encrypted_message,
                                     std::string private_key) {
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::RSA::PrivateKey key;
    CryptoPP::ByteQueue bytes;
    CryptoPP::StringSource src(private_key, true, new CryptoPP::Base64Decoder);
    src.TransferTo(bytes);
    bytes.MessageEnd();
    key.Load(bytes);

    CryptoPP::RSAES_PKCS1v15_Decryptor d(key);
    std::string message;

    CryptoPP::StringSource(encrypted_message, true,
                           new CryptoPP::PK_DecryptorFilter(
                               rng, d, new CryptoPP::StringSink(message)));

    return message;
}

}  // namespace enc