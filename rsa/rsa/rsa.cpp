#include "rsa.h"

//文件加密
void RSA::ecrept(const char* plain_file_in, const char* ecrept_file_out,
	bm::int1024_t ekey, bm::int1024_t pkey)
{
	std::ifstream fin(plain_file_in, std::ifstream::binary);
	std::ofstream fout(ecrept_file_out, std::ofstream::binary);
	if (!fin.is_open())
	{
		std::cout << "open file error" << std::endl;
		return;
	}
	const int Num = 256;
	char Buffer[Num];
	bm::int1024_t Buffer_out[Num];
	int Curnum;
	while (!fin.eof())
	{
		fin.read(Buffer, Num);
		Curnum = fin.gcount();
		for (int i = 0; i < Curnum; i++)
			Buffer_out[i] = ecrept((bm::int1024_t)Buffer[i], ekey, pkey);
		fout.write((char*)Buffer_out, Curnum * sizeof(bm::int1024_t));
	}
	fin.close();
	fout.close();
}

//文件解密
void RSA::decrept(const char* ecrept_file_in, const char* plain_file_out,
	bm::int1024_t dkey, bm::int1024_t pkey)
{
	std::ifstream fin(ecrept_file_in, std::ifstream::binary);
	std::ofstream fout(plain_file_out, std::ofstream::binary);
	if (!fin.is_open())
	{
		std::cout << "open file failed" << std::endl;
		return;
	}
	const int Num = 256;
	bm::int1024_t Buffer[Num];
	char Buffer_out[Num];
	int Curnum;
	while (!fin.eof())
	{
		fin.read((char*)Buffer, Num * sizeof(bm::int1024_t));
		Curnum = fin.gcount();
		Curnum /= sizeof(bm::int1024_t);
		for (int i = 0; i < Curnum; i++)
			Buffer_out[i] = (char)ecrept(Buffer[i], dkey, pkey);
		fout.write(Buffer_out, Curnum);
	}
	fin.close();
	fout.close();
}

//字符串加密
std::vector<bm::int1024_t> RSA::ecrept(std::string& str_in, bm::int1024_t ekey, bm::int1024_t pkey)
{
	std::vector<bm::int1024_t> Ciphertext;
	for (const auto& e : str_in)
	{
		Ciphertext.push_back(ecrept(e, ekey, pkey));
	}
	return Ciphertext;
}

//字符串解密
std::string RSA::decrept(std::vector<bm::int1024_t>& ecrept_str, bm::int1024_t dkey, bm::int1024_t pkey)
{
	std::string Plaintext;
	for (const auto& e : ecrept_str)
	{
		Plaintext.push_back((char)ecrept(e, dkey, pkey));
	}
	return Plaintext;
}

//打印
void RSA::printInfo(std::vector<bm::int1024_t>& ecrept_str)
{
	for (const auto& e : ecrept_str)
	{
		std::cout << e << " ";
	}
	std::cout << std::endl;
}



//1. 随机选择两个不相等的质数p和q(实际应用中，这两个质数越大，就越难破解)。
//2. 计算p和q的乘积n，n = pq。
//3. 计算n的欧拉函数φ(n)。
//4. 随机选择一个整数e，条件是1 < e < φ(n)，且e与φ(n) 互质。
//5. 计算e对于φ(n)的模反元素d，使得de≡1 mod φ(n)，即：
//6.		(de) mod φ(n) = 1
//7. 产生公钥(e, n)，私钥(d, n)。



RSA::RSA()
{
	produce_keys();
}

//加密单个信息	return pow(msg, key) % pkey
//An = (An-1 * An-1) % pkey;
bm::int1024_t RSA::ecrept(bm::int1024_t msg, bm::int1024_t key, bm::int1024_t pkey)
{
	//初始化
	bm::int1024_t msg_des = 1;
	//a, 即要加密的信息， a^b % c
	bm::int1024_t a = msg;
	//key: b
	//pkey : c
	bm::int1024_t b = key;
	while (b)
	{
		//如果二进制为1则连乘
		if (b & 1)
			msg_des = (msg_des * a) % pkey;
		b >>= 1;
		//Ai = (A(i-1) * A(i-1)) % c
		a = (a * a) % pkey;
	}
	return msg_des;
}

//随机生成一个素数
bm::int1024_t RSA::produce_prime()
{
	boost::random::mt19937 gen(time(nullptr));
	//指定随机数的范围 0 ~ (1<<128)
	boost::random::uniform_int_distribution<bm::int1024_t> dist(0, bm::int1024_t(1) << 128);
	/*srand(time(nullptr));
	bm::int1024_t prime = 0;
	while (1)
	{
		prime = rand() % 50 + 2;
		if (is_prime(prime))
			return prime;
	}*/
	bm::int1024_t prime = 0;
	while (1)
	{
		prime = dist(gen);
		if (is_prime_bigInt(prime))
			break;
	}
	return prime;
}

//素数判断
bool RSA::is_prime(bm::int1024_t prime)
{
	if (prime < 2)
		return false;
	for (bm::int1024_t i = 2; i <= sqrt(prime); i++)
	{
		if (prime % i == 0)
		{
			return false;
		}
	}
	return true;
}

bool RSA::is_prime_bigInt(bm::int1024_t digit)
{
	boost::random::mt11213b gen(time(nullptr));
	if (miller_rabin_test(digit, 25, gen))
	{
		if (miller_rabin_test((digit - 1) / 2, 25, gen))
		return true;
	}
	return false;
}

//定义所有的密钥
void RSA::produce_keys()
{
	bm::int1024_t prime1 = produce_prime();
	bm::int1024_t prime2 = 0;
	while (1)
	{
		prime2 = produce_prime();
		if (prime2 != prime1)
			break;
	}
	std::cout << "finished" << std::endl;
	_key.pkey = produce_pkey(prime1, prime2);
	std::cout << "pkey:" << _key.pkey << std::endl;
	bm::int1024_t orla = produce_orla(prime1, prime2);
	std::cout << "orla:" << orla << std::endl;
	_key.ekey = produce_ekey(orla);
	std::cout << "ekey:" << _key.ekey << std::endl;
	_key.dkey = produce_dkey(_key.ekey, orla);
	std::cout << "dkey:" << _key.dkey << std::endl;

	std::cout << is_prime_bigInt(prime1) << " " << is_prime_bigInt(prime2) << std::endl;
	std::cout << "pkey:";
	if (prime1*prime2 == _key.pkey)
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
	std::cout << "orla:";
	if ((prime1 - 1)*(prime2 - 1) == orla)
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
	std::cout << "ekey:";
	if (produce_gcd(_key.ekey, orla) == 1)
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
	std::cout << "dkey:";
	if ((_key.dkey*_key.ekey)%orla == 1)
		std::cout << "true" << std::endl;
	else
		std::cout << "false:" << (_key.dkey*_key.ekey) % _key.pkey << std::endl;
}

bm::int1024_t RSA::produce_pkey(bm::int1024_t prime1, bm::int1024_t prime2)
{
	return prime1 * prime2;
}

//计算欧拉函数
bm::int1024_t RSA::produce_orla(bm::int1024_t prime1, bm::int1024_t prime2)
{
	return (prime1 - 1) * (prime2 - 1);
}

//生成公钥
bm::int1024_t RSA::produce_ekey(bm::int1024_t orla)
{
	boost::random::mt19937 gen(time(nullptr));
	//指定随机数的范围 0 ~ (1<<128)
	boost::random::uniform_int_distribution<bm::int1024_t> dist(0, bm::int1024_t(1) << 128);
	srand(time(nullptr));
	bm::int1024_t ekey = 0;
	//if (is_prime(orla))
	//{
	//	ekey = rand() % (orla - 1) + 1;
	//	return ekey;
	//}
	while (1)
	{
		ekey = dist(gen) % orla;
		//std::cout << ekey << std::endl;
		if (ekey > 1 && produce_gcd(ekey, orla) == 1)
			break;
	}
	return ekey;

}

//辗转相除法求最大公约数
bm::int1024_t RSA::produce_gcd(bm::int1024_t ekey, bm::int1024_t orla)
{
	bm::int1024_t a;
	while (a = ekey % orla)
	{
		ekey = orla;
		orla = a;
	}
	return orla;
}

//生成私钥
bm::int1024_t RSA::produce_dkey(bm::int1024_t ekey, bm::int1024_t orla)
{
	bm::int1024_t x = 0;
	bm::int1024_t y = 0;
	exgcd(ekey, orla, x, y);
	return (x % orla + orla) % orla;
	//bm::int1024_t dkey = orla / ekey;
	//while (1)
	//{
	//	if ((dkey * ekey) % orla == 1)
	//		break;
	//	++dkey;
	//}
	//return dkey;
}
bm::int1024_t RSA::exgcd(bm::int1024_t a, bm::int1024_t b, bm::int1024_t& x, bm::int1024_t& y)
{
	if (b == 0)
	{
		x = 1;
		y = 0;
		return a;
	}
	//返回一个最大公约数
	bm::int1024_t ret = exgcd(b, a%b, x, y);
	bm::int1024_t x1 = x;
	bm::int1024_t y1 = y;
	x = y1;
	y = x1 - (a / b) * y1;
	return ret;
}