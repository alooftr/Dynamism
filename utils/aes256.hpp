#pragma once

#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>
#include <cryptopp/ccm.h>
#include <minwindef.h>
#pragma comment(lib, "cryptopp_release.lib")

namespace aes256
{
	const BYTE key[CryptoPP::AES::MAX_KEYLENGTH] = { 0x3F, 0x25, 0x64, 0x79, 0x39, 0x50, 0x57, 0x6D, 0x33, 0x52, 0x2D, 0x64, 0x62, 0x78, 0x51, 0x53, 0x33, 0x35, 0x3A, 0x5D, 0x4E, 0x35, 0x7A, 0x53, 0x41, 0x21, 0x75, 0x54, 0x2C, 0x25, 0x67, 0x4D };
	const BYTE iv[CryptoPP::AES::BLOCKSIZE] = { 0x64, 0x4D, 0x79, 0x68, 0x36, 0x53, 0x7B, 0x35, 0x26, 0x56, 0x7D, 0x6A, 0x63, 0x7D, 0x4B, 0x77 };

	inline std::string sha1(std::string input)
	{
		std::string result;
		CryptoPP::SHA1 hash;
		try
		{
			CryptoPP::StringSource str(input, true, new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(result), false)));
		}
		catch (CryptoPP::Exception ex)
		{
			result = "";
		}
		return result;
	}

	inline std::string encrypt_text(std::string input)
	{
		std::string result;
		try
		{
			CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryption;
			encryption.SetKeyWithIV(key, sizeof(key) / sizeof(key[0]), iv);
			CryptoPP::StringSource str(input, true, new CryptoPP::StreamTransformationFilter(encryption, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(result), true)));
		}
		catch (CryptoPP::Exception ex)
		{
			result = "";
		}
		return result;
	}

	inline std::string decrypt_text(std::string input)
	{
		std::string result;
		try
		{
			CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryption;
			decryption.SetKeyWithIV(key, sizeof(key) / sizeof(key[0]), iv);
			CryptoPP::StringSource str(input, true, new CryptoPP::Base64Decoder(new CryptoPP::StreamTransformationFilter(decryption, new CryptoPP::StringSink(result))));
		}
		catch (CryptoPP::Exception ex)
		{
			result = "";
		}
		return result;
	}

	inline std::vector<std::uint8_t> decrypt_vec(std::string input)
	{
		std::vector<std::uint8_t> result;
		try
		{
			CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryption;
			decryption.SetKeyWithIV(key, sizeof(key) / sizeof(key[0]), iv);
			CryptoPP::StringSource str(input, true, new CryptoPP::Base64Decoder(new CryptoPP::StreamTransformationFilter(decryption, new CryptoPP::VectorSink(result))));
		}
		catch (CryptoPP::Exception ex)
		{
			result = { };
		}
		return result;
	}
}