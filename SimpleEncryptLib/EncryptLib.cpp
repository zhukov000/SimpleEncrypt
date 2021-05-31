// EncryptLib.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include "EncryptLib.h"
#include "Tools.h"

std::string CreateGamma(unsigned n, std::string alphabet) {
	if (alphabet == "") {
		for (char c = 'a'; c <= 'z'; ++c) alphabet.push_back(c);
		for (char c = 'A'; c <= 'Z'; ++c) alphabet.push_back(c);
		for (char c = '0'; c <= '9'; ++c) alphabet.push_back(c);
	}

	unsigned m = alphabet.size();
	std::string result = "";

	for (unsigned i = 0; i < n; ++i) {
		int j = Rnd::genInt(0, m - 1);
		result.push_back(alphabet[j]);
	}

	return result;
}

std::vector<unsigned> CreatePermutationKey(std::string Password) {
	unsigned length = Password.size();
	std::vector< std::pair< char, unsigned > > temp;
	for (unsigned i = 0; i < length; ++i) {
		temp.push_back({Password[i], i});
	}
	sort(temp.begin(), temp.end());

	std::vector<unsigned> key;
	for (auto& p : temp) {
		key.push_back(p.second);
	}
	return key;
}

std::vector<unsigned> ReversePermutationKey(std::vector<unsigned> key) {
	unsigned length = key.size();
	std::vector<unsigned> res(length);
	for (unsigned i = 0; i < length; ++i) {
		res[ key[i] ] = i;
	}
	return res;
}

std::vector<char> PermutationEncryptBlock(std::vector<char> Block, std::vector<unsigned> Key) {
	assert(Block.size() == Key.size() && "Permutation encrypt error: Block and Key must be the same size");
	unsigned length = Key.size();
	std::vector<char> res;
	for (unsigned i : Key) {
		res.push_back(Block[i]);
	}
	return res;
}

std::vector<char> Encrypt(std::vector<char> Data, std::string Password, std::string Gamma) {
	std::vector<char> Result;
	
	unsigned dataLen = Data.size();
	unsigned passLen = Password.size();

	// 1. Permutations encrypt

	std::vector<unsigned> key = CreatePermutationKey(Password);

	for (unsigned iBlock = 0; iBlock < dataLen / passLen; ++iBlock) {
		std::vector<char> block;
		std::copy(Data.begin() + iBlock * passLen, Data.begin() + (iBlock + 1) * passLen, std::back_inserter(block) );
		block = PermutationEncryptBlock(block, key);
		std::copy(block.begin(), block.end(), std::back_inserter(Result));
	}

	if (dataLen % passLen) { // last, not complicated block
		unsigned partialLen = dataLen % passLen;
		std::vector<unsigned> key = CreatePermutationKey(Password.substr(0, partialLen));
		std::vector<char> block;
		std::copy(Data.begin() + (dataLen / passLen) * passLen, Data.end(), std::back_inserter(block));
		block = PermutationEncryptBlock(block, key);
		std::copy(block.begin(), block.end(), std::back_inserter(Result));
	}

	// 2. Replacement encrypt

	for (unsigned i = 0; i < dataLen; ++i) {
		Result[i] = (char) (((unsigned char)Result[i] + (unsigned char)Password[i % passLen]) ^ ((unsigned char)Gamma[i]));
	}
	
	return Result;
}

std::vector<char> Decrypt(std::vector<char> Data, std::string Password, std::string Gamma) {
	std::vector<char> Result;

	unsigned dataLen = Data.size();
	unsigned passLen = Password.size();

	// 1. Replacement decrypt

	for (unsigned i = 0; i < dataLen; ++i) {
		Result.push_back((char)(((unsigned char)Data[i] ^ ((unsigned char)Gamma[i])) - (unsigned char)Password[i % passLen]));
	}

	// 2. Permutations decrypt

	std::vector<unsigned> key = ReversePermutationKey( CreatePermutationKey(Password) );

	for (unsigned iBlock = 0; iBlock < dataLen / passLen; ++iBlock) {
		std::vector<char> block;
		std::copy(Result.begin() + iBlock * passLen, Result.begin() + (iBlock + 1) * passLen, std::back_inserter(block));
		block = PermutationEncryptBlock(block, key);
		
		for (unsigned j = 0; j < passLen; ++j) {
			Result[iBlock * passLen + j] = block[j];
		}
	}

	if (dataLen % passLen) { // last, not complicated block
		unsigned partialLen = dataLen % passLen;
		std::vector<unsigned> key = ReversePermutationKey( CreatePermutationKey(Password.substr(0, partialLen)) );
		std::vector<char> block;
		std::copy(Result.begin() + (dataLen / passLen) * passLen, Result.end(), std::back_inserter(block));
		block = PermutationEncryptBlock(block, key);

		for (unsigned j = 0; j < partialLen; ++j) {
			Result[dataLen / passLen * passLen + j] = block[j];
		}
	}

	return Result;
}