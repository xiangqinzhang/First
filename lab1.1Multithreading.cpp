#include <iostream>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>
using namespace std;
const int N = 300100, P = 998244353;    // ģ��Ϊp�����鳤������ΪN
int A[N], B[N], C[N], r[N];
int qpow(int x, int y) // ����ģ���㷨
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
//xΪ�����������ָ��
void ntt_partial(int* x, int lim, int opt, int start, int end)//start��end�ֱ���ÿ���߳�����Ҫ����Ŀ�Ĵ�С lim���������еĳ���
{
    int i, j, k, m, gn, g, tmp;
    for (i = start; i < end; ++i)
        if (r[i] < i)
            swap(x[i], x[r[i]]);//����һ��Ԥ���� ����Ҫ��Ԫ�ؽ�������ȷ��λ��
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

void ntt(int* x, int lim, int opt)//opt��һ������ָ��ִ���ض������Ĳ���
{
    vector<thread> threads;
    int num_threads = thread::hardware_concurrency();//��ȡ�����֧�ֵ��̲߳�������
    int chunk_size = lim / num_threads;//����ÿ���߳�����������ݿ��С
    for (int i = 0; i < num_threads; ++i)//ѭ����������߳�
    {
        int start = i * chunk_size;//��ʼ���±꣨��ʵ�������ݿ������Զ�Ӧ���ݿ��С��
        int end = (i == num_threads - 1) ? lim : (i + 1) * chunk_size;//������λ��  ���Ϊlim
        threads.emplace_back(ntt_partial, x, lim, opt, start, end);//��һ���µ��̶߳�����ӵ� threads ������
    }
    for (auto& th : threads)//ȷ���������߳̽������ܵ��̻߳����ִ��
    {
        th.join();
    }
}

int main()
{
    // ��ʼ�������������
    srand(time(nullptr));
    int i, lim = 1, n = N / 2; // �� n �� N ��һ�룬ȷ�����г˷�������λ���������
    // ���������� A �� B���ô�����ģ��ʮ�����µĴ�����ʾ
    for (i = 0; i < n; i++)
    {
        A[i] = rand() % 10;
        B[i] = rand() % 10;
    }
    // �����ʵ��� lim ֵ
    while (lim < n)
        lim <<= 1;
    // ��ʼ�� r ����
    for (i = 0; i < lim; ++i)
    {
        r[i] = (i & 1) * (lim >> 1) + (r[i >> 1] >> 1);
    }
    auto start = chrono::high_resolution_clock::now();
    // �� A �� B ���� NTT
    ntt(A, lim, 1);
    ntt(B, lim, 1);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, nano> elapsed = end - start;
    cout << "NTT took " << elapsed.count() << " nanoseconds to execute.\n"; // ��������NTT����ʱ�䣬��λΪ����
    // ���� C = A * B�Ĺ��̣���ʵ���޹�
    // for (i = 0; i < lim; ++i) {
//     C[i] = 1ll * A[i] * B[i] % P;
// }
// �� C ������ NTT
// ntt(C, lim, -1);
// ���г˷���λ����
// for (i = 0; i < lim; ++i) {
//     C[i + 1] += C[i] / 10;
//     C[i] %= 10;
// }

    return 0;
}
