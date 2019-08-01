地址十进制
页号 = 逻辑地址 / 页大小
页偏移 = 逻辑地址 % 页大小
物理地址 = 物理页号 * 页大小 + 页偏移

1111 1111 1111 1111 1111 1111 1100 0000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void niming()
{
	int fds[2];
	char buf[100];
	int len = 0;

	if (pipe(fds) == -1)
		perror("make pipe"), exit(1);

	while (fgets(buf, 100, stdin))
	{
		len = strlen(buf);

		if (write(fds[1], buf, len) != len)
		{
			perror("write to pipe");
			break;
		}

		memset(buf, 0x00, sizeof(buf));

		if ((len = read(fds[0], buf, 100)) == -1)
		{
			perror("read from pipe");
			break;
		}

		if (write(1, buf, len) != len)
		{
			perror("write to stdout");
			break;
		}
	}

	return;
}

int main()
{
	niming();
	return 0;
}

成熟 幼年
1      1
2 1    1
3 2    4
4 6    6
5 12   12
9 1
8 1
5 1
5 1
5 1
4
36 5

25
5 5 5 5 5      1
3 3 3 3 3 15   2

3 3 3 9        3
3 3            4

5 ^ 4  3
5 5 5 5 % 3
2 * 5 5 5 % 3

1 * 5 5 % 3
2 * 5 % 3
1
if（key % 2 == 0)
return pow(msg, 2) % pkey;
else
return msg % pkey;

ret = 1;
for (int i = 0; i < key; i++)
{
	ret = ret * msg % pkey;
}
1 - 99
101 - 999  2 * 19
1001 - 9999 3 * 19

9
2
2 2
2 2 2 2
2 2 2 2 2 1 1 1
1 1 1 0 0 0 0 0 0 0 0 0 0
0 0 0
1
2 3
4 5 6 7
https://www.nowcoder.com/cts/16754304/summary?id=3A814DC7648DF007 
3 4 5 0 6 7
3 4 4 0 3 6
1 1 2 1 5 2
2
4
8
16
32
46 63

10 28 15 30 40 50 70

3
8 12
19 28 20 15
22 5

n: ? 0100 0000 0000 ?
	m : ? 0001 0011 ?

	m << j : 0000 0100 1100
	n : 0100 0000 0000

	n | m << j : 0100 0100 1100

	? return 0100 0100 1100 ? -- - 》1100
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
	int main()
{
	int arr[3];
	int i = 0;
	int j = 0;
	int a = 0;
	printf("请输入三个数\n");
	for (i = 0; i < 3; i++)
	{
		scanf("%d", &arr[i]);
	}
	for (i = 1; i < 3; i++)
	{
		for (j = i; j > 0; j--)
		{
			if (arr[j] > arr[j - 1])
			{
				a = arr[j];
				arr[j] = arr[j - 1];
				arr[j - 1] = a;
			}
		}

	}
	for (i = 0; i < 3; i++)
	{
		printf("%d ", arr[i]);
	}
	system("pause");
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int main()
{
	int a = 0;
	int b = 0;
	printf("输入俩个数\n");
	scanf("%d %d", &a, &b);
	a = a + b;
	b = a - b;
	a = a - b;
	printf("%d %d\n", a, b);
	system("pause");
	return 0;
}
#define _crt_secure_no_warnings 1
#include <stdio.h>
int main()
{
	int a = 0;
	int b = 0;
	int c = 0;
	printf("输入俩个数\n");
	scanf("%d %d", &a, &b);
	c = a;
	a = b;
	b = c;
	printf("%d %d\n", a, b);
	system("pause");
	return 0;
}
//交换俩数组内容
#define _CRT_SECURE_NO_WARNINGS 1
int main()
{
	int arr1[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	int arr2[10] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		arr1[i] = arr1[i] + arr2[i];
		arr2[i] = arr1[i] - arr2[i];
		arr1[i] = arr1[i] - arr2[i];
	}
	for (i = 0; i < 10; i++)
	{
		printf("%d ", arr1[i]);
	}
	printf("\n");
	for (i = 0; i < 10; i++)
	{
		printf("%d ", arr2[i]);
	}
	system("pause");
	return 0;
}
//计算1 / 1 - 1 / 2 + 1 / 3到1 / 100
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int main()
{
	float i = 0;
	float sum1 = 0;
	float sum2 = 0;
	float num = 0;
	for (i = 1; i < 101; i += 2)
	{
		sum1 = sum1 + (1.0 / i);
	}for (i = 2; i < 101; i += 2)
	{
		sum2 = sum2 - (1.0 / i);
	}
	printf("%f\n", sum1 + sum2);
	system("pause");
	return 0;
}
//1-100中9出现的次数
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int main()
{
	int one = 0;
	int ten = 0;
	int hundred = 0;
	int i = 0;
	int count = 0;
	for (i = 1; i <= 100; i++)
	{
		hundred = i / 100;
		ten = (i - hundred * 100) / 10;
		one = i - hundred * 100 - ten * 10;
		if (one == 9)
		{
			count++;
		}
		if (ten == 9)
		{
			count++;
		}
		if (hundred == 9)
		{
			count++;
		}
	}
	printf("出现9的次数为：%d\n", count);
	system("pause");
	return 0;
}
//打印菱形
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int main()
{
	int i = 0;
	int j = 0;
	int k = 0;
	for (i = 1; i <= 7; i++)
	{
		for (k = 1; k <= 7 - i; k++)
		{
			printf(" ");
		}
		for (j = 1; j <= 2 * i - 1; j++)
		{
			printf("*");
		}
		printf("\n");
	}
	for (i = 6; i >= 1; i--)
	{
		for (k = 1; k <= 7 - i; k++)
		{
			printf(" ");
		}
		for (j = 1; j <= 2 * i - 1; j++)
		{
			printf("*");
		}
		printf("\n");
	}
	system("pause");
	return 0;
}
//求出0～999之间的所有“水仙花数”并输出
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int main()
{
	int one = 0;
	int ten = 0;
	int hundred = 0;
	int i = 0;
	for (i = 0; i <= 999; i++)
	{
		hundred = i / 100;
		ten = (i - hundred * 100) / 10;
		one = i - hundred * 100 - ten * 10;
		if (hundred >= 1)
		{
			if (i == hundred * hundred *hundred + ten * ten*ten + one * one*one)
			{
				printf("%d ", i);
			}
		}
	}
	printf("\n");
	system("pause");
	return 0;
}
//a + aa + aaa + aaaa + aaaaa的前5项之和
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <math.h>
int main()
{
	int Sn = 0;
	int i = 0;
	int j = 0;
	int num = 0;
	printf("输入一个整数：");
	scanf("%d", &num);
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j <= i; j++)
		{
			Sn = Sn + num * pow(10, j);
		}
	}
	printf("Sn=%d\n", Sn);
	system("pause");
	return 0;
}
//猜数字游戏
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void Game()
{
	int num = 0;
	int ret = rand() % 101;
	while (1)
	{
		printf("请输入一个数:\n");
		scanf("%d", &num);
		if (num == ret)
		{
			printf("恭喜你，猜对了！！！\n");
			break;
		}
		else if (num > ret)
		{
			printf("真遗憾，猜大了\n");
		}
		else if (num < ret)
		{
			printf("真遗憾，猜小了\n");
		}
		else
		{
			printf("输入错误！！！");
		}
	}
}
int main()
{
	int n = 0;
	srand((unsigned int)time(NULL));
	do
	{
		int n = 0;
		printf("********************************\n");
		printf("********|  1.开始游戏  |********\n");
		printf("********************************\n");
		printf("********|  0.退出游戏  |********\n");
		printf("********************************\n");
		printf("请选择:");
		scanf("%d", &n);
		switch (n)
		{
		case 1:
			Game();
			break;
		case 0:
			printf("退出游戏！！！\n");
			break;
		default:
			printf("选择错误！！！\n");
			break;
		}
	} while (n);
	system("pause");
	return 0;
}
//在整型有序数组中查找想要的数字
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int main()
{
	int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	int num = 0;
	printf("请输入你想要查找的数:\n");
	scanf("%d", &num);
	int left = 0;
	int right = sizeof(arr) / sizeof(arr[0]) - 1;
	int mid = ((right - left) / 2) + left;
	while (left <= right)
	{
		int mid = ((right - left) / 2) + left;
		if (arr[mid] == num)
		{
			printf("找到了!!!，%d\n", mid);
			break;
		}
		else if (arr[mid] > num)
		{
			right = mid - 1;
		}
		else if (arr[mid] < num)
		{
			left = mid + 1;
		}
	}
	if (left > right)
	{
		printf("找不到!!!\n");
	}
	system("pause");
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <string.h>
int main()
{
	int count = 0;
	char pssword[20] = { 0 };
	printf("请输入密码:\n");
	while (count < 3)
	{
		scanf("%s", &pssword);
		if (0 == strcmp(pssword, "159753"))
		{
			break;
		}
		else
		{
			printf("密码错误!!!请重新输入：\n");
		}
		count++;

	}
	if (count < 3)
	{
		printf("登陆成功\n");
	}
	else
	{
		printf("退出程序\n");
	}
	system("pause");
	return 0;
}
//可以一直接收键盘字符，如果是小写字符就输出对应的大写字符，如果接收的是大写字符，就输出对应的小写字符，如果是数字不输出
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int main()
{
	int ch = 0;
	while ((ch = getchar()) != EOF)
	{
		if (ch >= 'a'&&ch <= 'z')
		{
			printf("%c\n", ch + ('A' - 'a'));
		}
		else if (ch >= 'A'&&ch <= 'Z')
		{
			printf("%c\n", ch - ('A' - 'a'));
		}
	}
	system("pause");
	return 0;
}
//自定义乘法表
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>

void multiplication(int n)
{
	int i = 0;
	int j = 0;
	for (i = 1; i <= n; i++)
	{
		for (j = 1; j <= i; j++)
		{
			printf("%d*%d=%d\t ", j, i, i*j);
		}
		printf("\n");
	}
}
int main()
{
	int n = 0;
	scanf("%d", &n);
	multiplication(n);
	system("pause");
	return 0;
}
//交换俩个数
#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
void swap(int* p1, int* p2)
{
	int tmp = 0;
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}
int main()
{
	int num1 = 0;
	int num2 = 0;
	scanf("%d %d", &num1, &num2);
	swap(&num1, &num2);
	printf("%d %d\n", num1, num2);
	system("pause");
	return 0;
}
//闰年
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int leap_year(int n)
{
	if ((n % 100 == 0 && n % 100 != 0) || n % 400 == 0)
	{
		return -1;
	}
	return 0;
}
int main()
{
	int year = 0;
	int a;
	scanf("%d", &year);
	a = leap_year(year);
	if (-1 == a)
	{
		printf("是闰年\n");
	}
	else
	{
		printf("不是闰年\n");
	}
	system("pause");
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
void reverse(int arr[], int n)
{
	for (int i = 0; i < n / 2; i++)
	{
		int temp = arr[i];
		arr[i] = arr[n - 1 - i];
		arr[n - 1 - i] = temp;
	}
}
void init(int arr[], int n)
{
	for (int i = 0; i < n; i++)
	{
		arr[i] = 0;
	}
}
void empty(int arr[], int n)
{
	for (int i = 0; i < n; i++)
	{
		arr[i] = '\0';
	}
}
int main()
{
	int arr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int arr_length = sizeof(arr) / sizeof(arr[0]);
	reverse(arr, arr_length);
	printf("逆置：");
	for (int i = 0; i < arr_length; i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
	printf("初始：");
	init(arr, arr_length);
	for (int i = 0; i < arr_length; i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
	empty(arr, arr_length);
	printf("清空：");
	for (int i = 0; i < arr_length; i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
	system("pause");
	return 0;
}
//素数
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
int is_prime(int num)
{
	int i = 0;
	for (i = 2; i <= sqrt(num); i++)
	{
		if (num % i == 0)
		{
			break;
		}
	}
	if (i > sqrt(num))
	{
		return -1;
	}
	return 0;
}
int main()
{
	int num = 0;
	int a = 0;
	scanf("%d", &num);
	a = is_prime(num);
	if (-1 == a)
	{
		printf("%d是素数\n", num);
	}
	else
	{
		printf("%d不是素数\n", num);
	}
	system("pause");
	return 0;
}
https://github.com/lxyzzzZZZ/example-1.git
//斐波那契数
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>

int fib(int n)
{
	if (n <= 2)
	{
		return 1;
	}
	else
	{
		return fib(n - 1) + fib(n - 2);
	}
}
int main()
{
	int n = 0;
	int ret = 0;
	scanf("%d", &n);
	ret = fib(n);
	printf("%d\n", ret);
	system("pause");
	return 0;
}
//使用递归实现n^k
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>

int power(int n, int k)
{
	int i = k;
	if (k <= 0)
	{
		if (0 == i)
		{
			return 1;
		}
		if (i < 0)
		{
			return n * power(n, i + 1);
		}
	}
	if (1 == k)
	{
		return n;
	}
	if (k >= 1)
	{
		if (1 == i)
		{
			return n;
		}
		if (i > 1)
		{
			return n * power(n, i - 1);
		}
	}
}

int main()
{
	int n = 0;
	int k = 0;
	double ret = 0;
	scanf("%d%d", &n, &k);
	if (k <= 0)
	{
		ret = 1.0 / power(n, k);
	}
	if (k >= 1)
	{
		ret = power(n, k);
	}
	printf("%lf\n", ret);
	system("pause");
	return 0;
}
//用递归实现输入一个非负整数，返回组成它的数字之和
#define _CRT_SECURE_NO_WARNINGS 1 
#include <stdio.h>
int DigitSum(n)
{
	while (n > 0)
	{
		int sum = n % 10;
		return sum + DigitSum(n / 10);
	}
	return 0;
}
int main()
{
	int n = 0;
	int ret = 0;
	scanf("%d", &n);
	ret = DigitSum(n);
	printf("%d\n", ret);
	system("pause");
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
//递归
int	Factorial_1(int n)
{
	if (1 == n)
	{
		return 1;
	}
	else
	{
		return n * Factorial_1(n - 1);
	}
}
//非递归
int Factorial_2(int n)
{
	int product = 1;
	for (int i = n; i > 0; i--)
	{
		product *= i;
	}
	return product;
}
int main()
{
	int n = 0;
	int ret_1 = 0;
	int ret_2 = 0;
	scanf("%d", &n);
	ret_1 = Factorial_1(n);
	ret_2 = Factorial_2(n);
	printf("%d %d\n", ret_1, ret_2);
	system("pause");
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
//递归
int	Seeking_every_digit1(int num)
{
	if (num > 9)
	{
		Seeking_every_digit1(num / 10);
	}
	printf("%d ", num % 10);
}
//非递归
void Seeking_every_digit2(int num)
{
	while (num > 0)
	{
		printf("%d ", num % 10);
		num /= 10;
	}
}
int main()
{
	int num = 0;
	scanf("%d", &num);
	Seeking_every_digit1(num);
	printf("\n");
	Seeking_every_digit2(num);
	system("pause");
	return 0;
}
//将参数字符串中的字符反向排列
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
//int strlen(char *string)
//{
//	return sizeof(string) / sizeof(string[0]);
//}
void reverse_string(char * string)
{
	if (*string != '0')
	{
		reverse_string(string + 1);
		printf("%s", *string);
	}
	//char tmp = 0;
	//int left = 0;
	//int right = strlen(string) - 2;
	//if (left<right)
	//{
	//	tmp = string[left];
	//	string[left] = string[right];
	//	string[right] = tmp;
	//	left++;
	//	right--;
	//	reverse_string(&string);
	//}
	//return string;
}
int main()
{
	char* string = "abcdefg";
	//int sz = sizeof(string) / sizeof(string[0])-1;
	//reverse_string(&string, sz);
	reverse_string(string);
	//printf("%s\n", reverse_string(&string, sz));
	system("pause");
	return 0;
}
//递归和非递归分别实现strlen
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <string.h>
//递归
int Strlen1(char*string)
{
	if ('\0' == *string)
	{
		return 0;
	}
	else
	{
		return Strlen1(++string) + 1;
	}
}
//非递归
int Strlen2(char *string)
{
	int count = 0;
	while (*string)
	{
		count++;
		*string++;
	}
	return count;
}
int main()
{
	char string[] = "acdds";
	printf("%d %d\n", Strlen1(string), Strlen2(string));
	system("pause");
	return 0;
}
//	int ret = 0;
//	int i = 0;
//	int j = 0;
//	int count1 = 0;
//	int count2 = 0;
//	ret = IsFull(board, row, col);
//	if (1 == ret)
//	{
//		return 'd';
//	}
//	for (i = 0; i < row; i++)
//	{
//		count1 = 0;
//		count2 = 0;
//		for (j = 0; j < col; j++)
//		{
//			if ('O' == board[i][j] )
//			{
//				count1++;
//			}
//			if ('X' == board[i][j])
//			{
//				count2++;
//			}
//		}
//	}
//	if (row == count1)
//	{
//		return 'x';
//	}
//	if (row == count2)
//	{
//		return 'o';
//	}
//	for (i = 0; i < col; i++)
//	{
//		for (j = 0; j < row; j++)
//		{
//			count1 = 0;
//			count2 = 0;
//			if ('O' == board[i][j])
//			{
//				count1++;
//			}
//			if ('X' == board[i][j])
//			{
//				count2++;
//			}
//		}
//	}
//	if (row == count1)
//	{
//		return 'x';
//	}
//	if (row == count2)
//	{
//		return 'o';
//	}
//	for (i = 0; i < row; i++)
//	{
//		count1 = 0;
//		count2 = 0;
//		if ('O' == board[i][i])
//		{
//			count1++;
//		}
//		if ('X' == board[i][i])
//		{
//			count2++;
//		}
//	}
//	if (row == count1)
//	{
//		return 'x';
//	}
//	if (row == count2)
//	{
//		return 'o';
//	}
//	for (i = 0; i < row;i++)
//	for (j = col; j >= 0; j--)
//	{
//		count1 = 0;
//		count2 = 0;
//		if ('O' == board[i][j])
//		{
//			count1++;
//		}
//		if ('X' == board[i][j])
//		{
//			count2++;
//		}
//	}
//	if (row == count1)
//	{
//		return 'x';
//	}
//	if (row == count2)
//	{
//		return 'o';
//	}
//	return ' ';
//
//}
//写一个函数返回参数二进制中 1 的个数 
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int count_one_bits(unsigned int value)
{
	int count = 0;
	while (value != 0)
	{
		if (1 == value % 2)
		{
			count++;
		}
		value = value / 2;
	}
	return count;
}
int main()
{
	unsigned int num = 15;
	int ret = 0;
	scanf("%d", &num);
	ret = count_one_bits(num);
	printf("%d\n", ret);
	system("pause");
	return 0;
}
//获取一个数二进制序列中所有的偶数位和奇数位，分别输出二进制序列。 
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
void print_Binary_Sequence(int num)
{
	int count = 0;
	int i = 0;
	int arr[32] = { 0 };
	while (num)
	{
		arr[i] = num % 2;
		count++;
		i++;
		num /= 2;
	}
	printf("偶数序列：");
	for (i = count; i >= 0; i -= 2)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
	printf("奇数序列：");
	for (i = count - 1; i >= 0; i -= 2)
	{
		printf("%d ", arr[i]);
	}

}
int main()
{
	int num = 0;
	scanf("%d", &num);
	print_Binary_Sequence(num);
	system("pause");
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
void fun(int num)
{
	if (num > 0)
	{
		printf("%d ", num % 10);
		fun(num / 10);
	}
}
int main()
{
	int num = 0;
	scanf("%d", &num);
	fun(num);
	system("pause");
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int Different_Numbers(int num1, int num2)
{
	int count = 0;
	int tmp = 0;
	tmp = num1 ^ num2;
	while (tmp)
	{
		if (1 == tmp % 2)
		{
			count++;
		}
		tmp /= 2;
	}
	return count;
}
int main()
{
	int num1 = 0;
	int num2 = 0;
	int ret = 0;
	scanf("%d%d", &num1, &num2);
	ret = Different_Numbers(num1, num2);
	printf("%d\n", ret);
	system("pause");
	return 0;

}
//5位运动员参加了10米台跳水比赛，有人让他们预测比赛结果 
//A选手说：B第二，我第三；b2,a3
//B选手说：我第二，E第四；b2,e4
//C选手说：我第一，D第二；c1,d2
//D选手说：C最后，我第三；c5,d3
//E选手说：我第四，A第一；e4,a1
//比赛结束后，每位选手都说对了一半，请编程确定比赛的名次。
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int main()
{
	int A = 0;
	int B = 0;
	int C = 0;
	int D = 0;
	int E = 0;
	for (A = 1; A < 6; A++)
	{
		for (B = 1; B < 6; B++)
		{
			for (C = 1; C < 6; C++)
			{
				for (D = 1; D < 6; D++)
				{
					for (E = 1; E < 6; E++)
					{
						if (1 == (2 == B) + (3 == A) &&
							1 == (2 == B) + (4 == E) &&
							1 == (1 == C) + (2 == D) &&
							1 == (5 == C) + (3 == D) &&
							1 == (4 == E) + (1 == A))
						{
							if (A != B && A != C && A != D && A != E && C != B && D != B && E != B && C != D && C != E && E != D)
							{
								printf("A:%d B:%d C:%d D:%d E:%d\n", A, B, C, D, E);
							}

						}
					}
				}
			}
		}
	}
	system("pause");
	return 0;
}
//日本某地发生了一件谋杀案，警察通过排查确定杀人凶手必为4个
//嫌疑犯的一个。以下为4个嫌疑犯的供词。
//A说：不是我。
//B说：是C。
//C说：是D。
//D说：C在胡说
//已知3个人说了真话，1个人说的是假话。
//现在请根据这些信息，写一个程序来确定到底谁是凶手
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
int main()
{
	char arr[] = { 'A', 'B', 'C', 'D' };
	int i;
	char A = 'a';
	char B = 'C';
	char C = 'D';
	char D = 'C';
	for (i = 0; i < 4; i++)
	{
		if (3 == (arr[i] != 'A') + (arr[i] == B) + (arr[i] == C) + (arr[i] == D))
		{
			printf("%c\n", arr[i]);
		}
	}
	system("pause");
	return 0;
}