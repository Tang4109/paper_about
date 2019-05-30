/****************************************!
*@brief  ʹ��ģ���˻��㷨(SA)���TSP����(31������)
*@author ZhangYunjia
*@date   2019/5/30/14:20

****************************************/

#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
using namespace std;
#define T0 50000.0                   //��ʼ�¶�
#define T_end 0.5                 //�����¶�
#define q 0.98                       // �˻�ϵ��
#define L 10000                       // ÿ���¶�ʱ�ĵ���������������
#define N 31                         // ��������
int city_list[N];                    // ���ڴ��һ����

                                     // �й�31����������
double city_pos[N][2] =
{
{ 1304,2312 },{ 3639,1315 },{ 4177,2244 },{ 3712,1399 },
{ 3488,1535 },{ 3326,1556 },{ 3238,1229 },{ 4196,1004 },
{ 4312,790 },{ 4386,570 },{ 3007,1970 },{ 2562,1756 },
{ 2788,1491 },{ 2381,1676 },{ 1332,695 },{ 3715,1678 },
{ 3918,2179 },{ 4061,2370 },{ 3780,2212 },{ 3676,2578 },
{ 4029,2838 },{ 4263,2931 },{ 3429,1908 },{ 3507,2367 },
{ 3394,2643 },{ 3439,3201 },{ 2935,3240 },{ 3140,3550 },
{ 2545,2357 },{ 2778,2826 },{ 2370,2975 } 
};

//��������                                   
double distance(double *, double *); // �����������о���
double path_len(int *);              // ����·������
void  init();                        //��ʼ������
void create_new();                   // �����½�

// ���뺯��
double distance(double * city1, double * city2)
{
	double x1 = *city1;
	double y1 = *(city1 + 1);
	double x2 = *(city2);
	double y2 = *(city2 + 1);
	double dis = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
	return dis;
}

// ����·������                                    
double path_len(int * arr)
{
	double path = 0;                 // ��ʼ��·������
	int index = *arr;                // ��λ����һ������(�������)
	for (int i = 0; i < N - 1; i++)
	{
		int index1 = *(arr + i);
		int index2 = *(arr + i + 1);
		double dis = distance(city_pos[index1 - 1],city_pos[index2 - 1]);
		path += dis;
	}
	int last_index = *(arr + N - 1); // ���һ���������
	int first_index = *arr;          // ��һ���������
	double last_dis = distance(city_pos[last_index - 1],city_pos[first_index - 1]);
	path = path + last_dis;
	return path;                     // �����ܵ�·������
}

// ��ʼ������
void init()
{
	for (int i = 0; i < N; i++)
		city_list[i] = i + 1;        // ��ʼ��һ����
}

// ����һ���½�
// �˴����������������λ�õķ�ʽ�����µĽ�
void create_new()
{
	double r1 = ((double)rand()) / (RAND_MAX + 1.0);//����[0,1��֮������С��
	double r2 = ((double)rand()) / (RAND_MAX + 1.0);
	int pos1 = (int)(N*r1);          //��һ��������λ��
	int pos2 = (int)(N*r2);

	int temp = city_list[pos1];
	city_list[pos1] = city_list[pos2];
	city_list[pos2] = temp;          // ����������
}

// ������

int main()
{

	srand((unsigned)time(NULL));                                   //��ʼ�����������
	time_t start, finish;
	start = clock();                                               // �������п�ʼ��ʱ
	double T;
	int count = 0;                                                 // ��¼���´���
	T = T0;                                                        //��ʼ�¶�
	init();                                                        //��ʼ��һ����
	int city_list_copy[N];                                         // ���ڱ���ԭʼ��
	double f1, f2, df;                                             //f1Ϊ��ʼ��Ŀ�꺯��ֵ��
					                                               //f2Ϊ�½�Ŀ�꺯��ֵ��dfΪ���߲�ֵ

	double r;                                                      // 0-1֮�������������������Ƿ�����½�
	while (T > T_end)                                              // ���¶ȵ��ڽ����¶�ʱ���˻����
	{
		for (int i = 0; i < (int)(L / T); i++)
		{
			                                                       // ��������
			memcpy(city_list_copy, city_list, N * sizeof(int));
			create_new();                                          // �����½�
			f1 = path_len(city_list_copy);
			f2 = path_len(city_list);
			df = f2 - f1;
			                                                       // ������Metropolis׼��
			if (df >= 0)
			{
				r = ((double)rand()) / (RAND_MAX);
				if (exp(-df / T) <= r)                             // ����ԭ���Ľ�
				{
					memcpy(city_list, city_list_copy, N * sizeof(int));
				}
			}
		}
		T *= q;                                                    // ����
		count++;
	}
	finish = clock();                                              // �˻���̽���
	double duration = ((double)(finish - start)) / CLOCKS_PER_SEC; // ����ʱ��
	cout << "ģ���˻��㷨" << endl;
	cout << "��ʼ�¶�T0:" << T0 << endl;
	cout << "����ϵ��q:" << q << endl;
	cout << "�����´���:" << count << endl;

	for (int i = 0; i < N - 1; i++)                                // �������·��
	{
		cout << city_list[i] << "--->";
	}
	cout << city_list[N - 1] << endl;
	double len = path_len(city_list);                              // ����·������
	cout << "����·������Ϊ:" << len << endl;
	cout << "�������к�ʱ" << duration << "s" << endl;
	
	
	cout<<"end.."<<endl;
	system("pause");
	return 0;
}

