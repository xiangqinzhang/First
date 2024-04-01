#include <iostream>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>
using namespace std;
const int N = 300100, P = 998244353;    // 模数为p，数组长度限制为N
int A[N], B[N], C[N], r[N];
int qpow(int x, int y) // 快速模幂算法
{
    int res(1);
    while (y)
    {
        if (y & 1)
            res = 1ll * res * x % P;
        x = 1ll * x * x % P;
        y >>= 1;
    }
    return res;
}
//x为待处理的数组指针
void ntt_partial(int* x, int lim, int opt, int start, int end)//start和end分别是每个线程所需要处理的块的大小 lim是数据序列的长度
{
    int i, j, k, m, gn, g, tmp;
    for (i = start; i < end; ++i)
        if (r[i] < i)
            swap(x[i], x[r[i]]);//进行一次预处理 将需要的元素交换的正确的位置
    for (m = 2; m <= lim; m <<= 1)
    {
        k = m >> 1;
        gn = qpow(3, (P - 1) / m);
        for (i = start; i < end; i += m)
        {
            g = 1;
            for (j = 0; j < k; j++, g = 1ll * g * gn % P)
            {
                tmp = 1ll * x[i + j + k] * g % P;
                x[i + j + k] = (x[i + j] - tmp + P) % P;
                x[i + j] = (x[i + j] + tmp) % P;
            }
        }
    }
}

void ntt(int* x, int lim, int opt)//opt是一个用于指定执行特定操作的参数
{
    vector<thread> threads;
    int num_threads = thread::hardware_concurrency();//获取计算机支持的线程并发数量
    int chunk_size = lim / num_threads;//计算每个线程所处理的数据块大小
    for (int i = 0; i < num_threads; ++i)//循环创立多个线程
    {
        int start = i * chunk_size;//开始的下标（其实就是数据块数乘以对应数据块大小）
        int end = (i == num_threads - 1) ? lim : (i + 1) * chunk_size;//结束的位置  最大为lim
        threads.emplace_back(ntt_partial, x, lim, opt, start, end);//将一个新的线程对象添加到 threads 向量中
    }
    for (auto& th : threads)//确保所有子线程结束后总的线程会继续执行
    {
        th.join();
    }
}

int main()
{
    // 初始化随机数生成器
    srand(time(nullptr));
    int i, lim = 1, n = N / 2; // 设 n 是 N 的一半，确保进行乘法后数据位数不会溢出
    // 用随机数填充 A 和 B，用大数组模拟十进制下的大数表示
    for (i = 0; i < n; i++)
    {
        A[i] = rand() % 10;
        B[i] = rand() % 10;
    }
    // 计算适当的 lim 值
    while (lim < n)
        lim <<= 1;
    // 初始化 r 数组
    for (i = 0; i < lim; ++i)
    {
        r[i] = (i & 1) * (lim >> 1) + (r[i >> 1] >> 1);
    }
    auto start = chrono::high_resolution_clock::now();
    // 对 A 和 B 进行 NTT
    ntt(A, lim, 1);
    ntt(B, lim, 1);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, nano> elapsed = end - start;
    cout << "NTT took " << elapsed.count() << " nanoseconds to execute.\n"; // 计算两次NTT的总时间，单位为纳秒
    // 计算 C = A * B的过程，与实验无关
    // for (i = 0; i < lim; ++i) {
//     C[i] = 1ll * A[i] * B[i] % P;
// }
// 对 C 进行逆 NTT
// ntt(C, lim, -1);
// 进行乘法进位处理
// for (i = 0; i < lim; ++i) {
//     C[i + 1] += C[i] / 10;
//     C[i] %= 10;
// }

    return 0;
}
