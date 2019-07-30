#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/random.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
namespace bm = boost::multiprecision;
struct Key
{
	//公钥(ekey, pkey): (e,n)
	bm::int1024_t pkey;
	bm::int1024_t ekey;
	//私钥(dkey, pkey): (d, n)
	bm::int1024_t dkey;
};

class RSA
{
public:
	RSA();
	Key getKey()
	{
		return _key;
	}
	//加密
	void ecrept(const char* plain_file_in, const char* ecrept_file_out,
		bm::int1024_t ekey, bm::int1024_t pkey);
	//解密
	void decrept(const char* ecrept_file_in, const char* plain_file_out,
		bm::int1024_t dkey, bm::int1024_t pkey);

	std::vector<bm::int1024_t> ecrept(std::string& str_in, bm::int1024_t ekey, bm::int1024_t pkey);
	std::string decrept(std::vector<bm::int1024_t>& ecrept_str, bm::int1024_t dkey, bm::int1024_t pkey);

	void printInfo(std::vector<bm::int1024_t>& ecrept_str);
private:
	//加密单个信息
	bm::int1024_t ecrept(bm::int1024_t msg, bm::int1024_t key, bm::int1024_t pkey);


	bm::int1024_t produce_prime();
	bool is_prime(bm::int1024_t prime);
	bool is_prime_bigInt(bm::int1024_t digit);
	void produce_keys();
	bm::int1024_t produce_pkey(bm::int1024_t prime1, bm::int1024_t prime2);
	bm::int1024_t produce_orla(bm::int1024_t prime1, bm::int1024_t prime2);
	bm::int1024_t produce_ekey(bm::int1024_t orla);
	bm::int1024_t produce_gcd(bm::int1024_t ekey, bm::int1024_t orla);
	bm::int1024_t produce_dkey(bm::int1024_t ekey, bm::int1024_t orla);
	bm::int1024_t exgcd(bm::int1024_t a, bm::int1024_t b, bm::int1024_t& x, bm::int1024_t& y);
private:
	Key _key;
};