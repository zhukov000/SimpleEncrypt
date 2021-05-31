// EncryptTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/**
* Alexander Zhukov
* 
* I used MSDN for create dll: https://docs.microsoft.com/en-us/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp?view=msvc-160
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#include "EncryptLib.h"

// CreateGamma(5, "")

void PrintHelp() {
	std::ifstream fin("help.txt");
	while (!fin.eof()) {
		std::string line;
		getline(fin, line);
		std::cout << line << std::endl;
	}
}

std::vector<char> readFile(std::string fileName) {
	std::vector<char> buffer;
	std::ifstream fin(fileName, std::ios_base::binary);
	// get length
	fin.seekg(0, std::ios::end);
	unsigned length = fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	if (length > 0) {
		buffer.resize(length);
		fin.read(&buffer[0], length);
	}
	return buffer;
}

void saveFile(std::string fileName, std::vector<char> buffer) {
	std::ofstream fout(fileName);
	std::string res;
	std::copy(buffer.begin(), buffer.end(), std::back_inserter(res));
	fout << res;
	fout.close();
}

void _Encrypt(std::string fileName, std::string password) {
	std::string fileNameWithoutExt = fileName.substr(0, fileName.find_last_of('.'));

	std::string keyName = fileNameWithoutExt + ".key";
	std::string encName = fileNameWithoutExt + ".enc";

	// 1. read all data from file to buffer
	std::vector<char> buffer = readFile(fileName);

	// 2. produce gamma and save to file
	std::string gamma = CreateGamma(buffer.size(), "");
	{
		std::ofstream fout(keyName);
		fout << gamma;
		fout.close();
	}

	// 3. encrypt
	buffer = Encrypt(buffer, password, gamma);
	
	// 4. save
	saveFile(encName, buffer);

	// 5. print message
	std::cout << "2 files were created: " << keyName << ", " << encName << "\n";
}

void _Decrypt(std::string fileName, std::string keyName, std::string password) {
	std::string fileNameWithoutExt = fileName.substr(0, fileName.find_last_of('.'));

	std::string fileOut = fileNameWithoutExt + ".out";

	// 1. read all data from file to buffer
	std::vector<char> buffer = readFile(fileName);

	// 2. read gamma
	std::string gamma = "";
	{
		std::vector<char> buffer = readFile(keyName);
		std::copy(buffer.begin(), buffer.end(), std::back_inserter(gamma));
	}

	// 3. decrypt
	buffer = Decrypt(buffer, password, gamma);

	// 4. save
	saveFile(fileOut, buffer);
	
	// 5. print message
	std::cout << "1 file was created: " << fileOut << "\n";
}

int main(int argc, char **argv) {

	if (argc == 1) {
		PrintHelp();
		return 0;
	}

	bool isEncrypt = false, isDecrypt = false;
	std::string fileName = "";
	std::string password = "";
	std::string keyName = "";

	// Parse options
	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-encrypt"))  isEncrypt = true;
		else if (!strcmp(argv[i], "-decrypt"))  isDecrypt = true;
		else {
			std::string param = argv[i];

			if (param.rfind("-file=", 0) == 0) {
				if (fileName != "") {
					std::cout << "Multiply files doesn't support\n";
					return 0;
				}
				fileName = param.substr(6);
			}
			else if (param.rfind("-key=", 0) == 0) {
				if (keyName != "") {
					std::cout << "Multiply keys doesn't support\n";
					return 0;
				}
				keyName = param.substr(5);
			}
			else if (param.rfind("-pass=", 0) == 0) {
				if (password != "") {
					std::cout << "Multiply password doesn't support\n";
					return 0;
				}
				password = param.substr(6);
			}
		}
	}

	// check options
	if (isEncrypt && isDecrypt) {
		std::cout << "Wrong options: can't encrypt and decrypt simultaniously!\n";
		return 0;
	}
	if (!isEncrypt && !isDecrypt) {
		std::cout << "You don't specify what is need to do: encrypt or decrypt\n";
		return 0;
	}

	// enter password
	while (password == "") {
		std::cout << "\nYou must specify password: " << std::endl;
		std::cin >> password;
	}

	// do smth
	if (isEncrypt) {
		_Encrypt(fileName, password);
	}
	else {
		_Decrypt(fileName, keyName, password);
	}
	return 0;
}

