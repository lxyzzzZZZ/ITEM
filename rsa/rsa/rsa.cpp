#include "rsa.h"

//�ļ�����
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

//�ļ�����
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

//�ַ�������
std::vector<bm::int1024_t> RSA::ecrept(std::string& str_in, bm::int1024_t ekey, bm::int1024_t pkey)
{
	std::vector<bm::int1024_t> Ciphertext;
	for (const auto& e : str_in)
	{
		Ciphertext.push_back(ecrept(e, ekey, pkey));
	}
	return Ciphertext;
}

//�ַ�������
std::string RSA::decrept(std::vector<bm::int1024_t>& ecrept_str, bm::int1024_t dkey, bm::int1024_t pkey)
{
	std::string Plaintext;
	for (const auto& e : ecrept_str)
	{
		Plaintext.push_back((char)ecrept(e, dkey, pkey));
	}
	return Plaintext;
}

//��ӡ
void RSA::printInfo(std::vector<bm::int1024_t>& ecrept_str)
{
	for (const auto& e : ecrept_str)
	{
		std::cout << e << " ";
	}
	std::cout << std::endl;
}



//1. ���ѡ����������ȵ�����p��q(ʵ��Ӧ���У�����������Խ�󣬾�Խ���ƽ�)��
//2. ����p��q�ĳ˻�n��n = pq��
//3. ����n��ŷ��������(n)��
//4. ���ѡ��һ������e��������1 < e < ��(n)����e���(n) ���ʡ�
//5. ����e���ڦ�(n)��ģ��Ԫ��d��ʹ��de��1 mod ��(n)������
//6.		(de) mod ��(n) = 1
//7. ������Կ(e, n)��˽Կ(d, n)��



RSA::RSA()
{
	produce_keys();
}

//���ܵ�����Ϣ	return pow(msg, key) % pkey
//An = (An-1 * An-1) % pkey;
bm::int1024_t RSA::ecrept(bm::int1024_t msg, bm::int1024_t key, bm::int1024_t pkey)
{
	//��ʼ��
	bm::int1024_t msg_des = 1;
	//a, ��Ҫ���ܵ���Ϣ�� a^b % c
	bm::int1024_t a = msg;
	//key: b
	//pkey : c
	bm::int1024_t b = key;
	while (b)
	{
		//���������Ϊ1������
		if (b & 1)
			msg_des = (msg_des * a) % pkey;
		b >>= 1;
		//Ai = (A(i-1) * A(i-1)) % c
		a = (a * a) % pkey;
	}
	return msg_des;
}

//�������һ������
bm::int1024_t RSA::produce_prime()
{
	boost::random::mt19937 gen(time(nullptr));
	//ָ��������ķ�Χ 0 ~ (1<<128)
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

//�����ж�
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

//�������е���Կ
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

//����ŷ������
bm::int1024_t RSA::produce_orla(bm::int1024_t prime1, bm::int1024_t prime2)
{
	return (prime1 - 1) * (prime2 - 1);
}

//���ɹ�Կ
bm::int1024_t RSA::produce_ekey(bm::int1024_t orla)
{
	boost::random::mt19937 gen(time(nullptr));
	//ָ��������ķ�Χ 0 ~ (1<<128)
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

//շת����������Լ��
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

//����˽Կ
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
	//����һ�����Լ��
	bm::int1024_t ret = exgcd(b, a%b, x, y);
	bm::int1024_t x1 = x;
	bm::int1024_t y1 = y;
	x = y1;
	y = x1 - (a / b) * y1;
	return ret;
}