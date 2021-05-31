// EncryptLib.h - Contains declarations of enctyption functions
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>

#ifdef SIMPLEENCRYPTLIB_EXPORTS
#define SIMPLEENCRYPTLIB_API __declspec(dllexport)
#else
#define SIMPLEENCRYPTLIB_API __declspec(dllimport)
#endif

/// <summary>
/// Create randomized gamma for XOR containes latin chars and numbers
/// </summary>
/// <param name="n">length of gamma</param>
/// <param name="alphabet">using symbols for gamma, using "" for [0-9A-Za-z]</param>
/// <returns></returns>
extern "C++" SIMPLEENCRYPTLIB_API std::string CreateGamma(unsigned, std::string);

/// <summary>
/// Create key for permutation cipher
/// </summary>
/// <param name="Password">Used for define order</param>
/// <returns></returns>
extern "C++" SIMPLEENCRYPTLIB_API std::vector<unsigned> CreatePermutationKey(
    std::string Password);

/// <summary>
/// Resverse permutation key for decrypt
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
extern "C++" SIMPLEENCRYPTLIB_API std::vector<unsigned> ReversePermutationKey(
    std::vector<unsigned> key);

/// <summary>
/// Permutations encrypt data-block with using key
/// </summary>
/// <param name="Block"></param>
/// <param name="Key"></param>
/// <returns></returns>
extern "C++" SIMPLEENCRYPTLIB_API std::vector<char> PermutationEncryptBlock(
    std::vector<char> Block, std::vector<unsigned> Key);

/// <summary>
/// Encrypt data
/// </summary>
/// <param name="Data"></param>
/// <param name="Password"></param>
/// <param name="Gamma"></param>
/// <returns></returns>
extern "C++" SIMPLEENCRYPTLIB_API std::vector<char> Encrypt(
    std::vector<char> Data, std::string Password, std::string Gamma);

/// <summary>
/// Decrypt data
/// </summary>
/// <param name="Data"></param>
/// <param name="Password"></param>
/// <param name="Gamma"></param>
/// <returns></returns>
extern "C++" SIMPLEENCRYPTLIB_API std::vector<char> Decrypt(
    std::vector<char> Data, std::string Password, std::string Gamma);