/****************************************!
*@brief  迭代局部搜索求解TSP问题
*@author ZhangYunjia
*@date   2019/5/30/17:18

****************************************/

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <windows.h>
#include <memory.h>
#include <string.h>
#include <iomanip>

#define DEBUG

using namespace std;

#define CITY_SIZE 31 //城市数量


//城市坐标
typedef struct candidate
{
	int x;
	int y;
}city, CITIES;

//优化值
int **Delta;

//解决方案
typedef struct Solution
{
	int permutation[CITY_SIZE]; //城市排列
	int cost;                        //该排列对应的总路线长度
}SOLUTION;
// 计算邻域操作优化值 
int calc_delta(int i, int k, int *tmp, CITIES * cities);

//计算两个城市间距离
int distance_2city(city c1, city c2);

//根据产生的城市序列，计算旅游总距离
int cost_total(int * cities_permutation, CITIES * cities);

//获取随机城市排列, 用于产生初始解
void random_permutation(int * cities_permutation);

//颠倒数组中下标begin到end的元素位置, 用于two_opt邻域动作
void swap_element(int *p, int begin, int end);

//邻域动作 反转index_i <-> index_j 间的元素
void two_opt_swap(int *cities_permutation, int *new_cities_permutation, int index_i, int index_j);

//本地局部搜索，边界条件 max_no_improve
void local_search(SOLUTION & best, CITIES * cities, int max_no_improve);

//判断接受准则
bool AcceptanceCriterion(int *cities_permutation, int *old_cities_permutation, CITIES * p_cities);

//将城市序列分成4块，然后按块重新打乱顺序。
//用于扰动函数
void double_bridge_move(int *cities_permutation, int * new_cities_permutation);

//扰动
void perturbation(CITIES * cities, SOLUTION &best_solution, SOLUTION &current_solution);

//迭代搜索
void iterated_local_search(SOLUTION & best, CITIES * cities, int max_iterations, int max_no_improve);

// 更新Delta 
void Update(int i, int k, int *tmp, CITIES * cities);

//城市排列
int permutation[CITY_SIZE];
//城市坐标数组
CITIES cities[CITY_SIZE];


//berlin31城市坐标
CITIES berlin31[CITY_SIZE] = {
	{ 1304,2312 },{ 3639,1315 },{ 4177,2244 },{ 3712,1399 },
{ 3488,1535 },{ 3326,1556 },{ 3238,1229 },{ 4196,1004 },
{ 4312,790 },{ 4386,570 },{ 3007,1970 },{ 2562,1756 },
{ 2788,1491 },{ 2381,1676 },{ 1332,695 },
{ 3715,1678 },{ 3918,2179 },{ 4061,2370 },
{ 3780,2212 },{ 3676,2578 },{ 4029,2838 },
{ 4263,2931 },{ 3429,1908 },{ 3507,2367 },
{ 3394,2643 },{ 3439,3201 },{ 2935,3240 },
{ 3140,3550 },{ 2545,2357 },{ 2778,2826 },
{ 2370,2975 } };




//计算两个城市间距离
int distance_2city(city c1, city c2)
{
	int distance = 0;
	distance = sqrt((double)((c1.x - c2.x)*(c1.x - c2.x) + (c1.y - c2.y)*(c1.y - c2.y)));

	return distance;
}

//根据产生的城市序列，计算旅游总距离
//所谓城市序列，就是城市先后访问的顺序，比如可以先访问ABC，也可以先访问BAC等等
//访问顺序不同，那么总路线长度也是不同的
//p_perm 城市序列参数
int cost_total(int * cities_permutation, CITIES * cities)
{
	int total_distance = 0;
	int c1, c2;
	//逛一圈，看看最后的总距离是多少
	for (int i = 0; i < CITY_SIZE; i++)
	{
		c1 = cities_permutation[i];
		if (i == CITY_SIZE - 1) //最后一个城市和第一个城市计算距离
		{
			c2 = cities_permutation[0];
		}
		else
		{
			c2 = cities_permutation[i + 1];
		}
		total_distance += distance_2city(cities[c1], cities[c2]);
	}

	return total_distance;
}

//获取随机城市排列
void random_permutation(int * cities_permutation)
{
	int i, r, temp;
	for (i = 0; i < CITY_SIZE; i++)
	{
		cities_permutation[i] = i; //初始化城市排列，初始按顺序排
	}


	for (i = 0; i < CITY_SIZE; i++)
	{
		//城市排列顺序随机打乱
		r = rand() % (CITY_SIZE - i) + i;//r最大为CITY_SIZE-1
		temp = cities_permutation[i];
		cities_permutation[i] = cities_permutation[r];
		cities_permutation[r] = temp;
	}
}




//颠倒数组中下标begin到end的元素位置
void swap_element(int *p, int begin, int end)
{
	int temp;
	while (begin < end)
	{
		temp = p[begin];
		p[begin] = p[end];
		p[end] = temp;
		begin++;
		end--;
	}
}


//邻域动作 反转index_i <-> index_j 间的元素
void two_opt_swap(int *cities_permutation, int *new_cities_permutation, int index_i, int index_j)
{
	for (int i = 0; i < CITY_SIZE; i++)
	{
		new_cities_permutation[i] = cities_permutation[i];
	}
	//new_cities_permutation = cities_permutation;

	swap_element(new_cities_permutation, index_i, index_j);
}



int calc_delta(int i, int k, int *tmp, CITIES * cities) {
	int delta = 0;
	/*
	以下计算说明：
	对于每个方案，翻转以后没必要再次重新计算总距离
	只需要在翻转的头尾做个小小处理

	比如：
	有城市序列   1-2-3-4-5 总距离 = d12 + d23 + d34 + d45 + d51 = A
	翻转后的序列 1-4-3-2-5 总距离 = d14 + d43 + d32 + d25 + d51 = B
	由于 dij 与 dji是一样的，所以B也可以表示成 B = A - d12 - d45 + d14 + d25
	下面的优化就是基于这种原理
	*/
	if (i == 0)
	{
		if (k == CITY_SIZE - 1)
		{
			delta = 0;
		}
		else
		{
			delta = 0
				- distance_2city(cities[tmp[k]], cities[tmp[k + 1]])
				+ distance_2city(cities[tmp[i]], cities[tmp[k + 1]])
				- distance_2city(cities[tmp[CITY_SIZE - 1]], cities[tmp[i]])
				+ distance_2city(cities[tmp[CITY_SIZE - 1]], cities[tmp[k]]);
		}

	}
	else
	{
		if (k == CITY_SIZE - 1)
		{
			delta = 0
				- distance_2city(cities[tmp[i - 1]], cities[tmp[i]])
				+ distance_2city(cities[tmp[i - 1]], cities[tmp[k]])
				- distance_2city(cities[tmp[0]], cities[tmp[k]])
				+ distance_2city(cities[tmp[i]], cities[tmp[0]]);
		}
		else
		{
			delta = 0
				- distance_2city(cities[tmp[i - 1]], cities[tmp[i]])
				+ distance_2city(cities[tmp[i - 1]], cities[tmp[k]])
				- distance_2city(cities[tmp[k]], cities[tmp[k + 1]])
				+ distance_2city(cities[tmp[i]], cities[tmp[k + 1]]);
		}
	}

	return delta;

}


/*
去重处理，对于Delta数组来说，对于城市序列1-2-3-4-5-6-7-8-9-10，如果对3-5应用了邻域操作2-opt ， 事实上对于
7-10之间的翻转是不需要重复计算的。 所以用Delta提前预处理一下。

当然由于这里的计算本身是O（1） 的，事实上并没有带来时间复杂度的减少（更新操作反而增加了复杂度）
如果delta计算 是O（n）的，这种去重操作效果是明显的。
*/



void Update(int i, int k, int *tmp, CITIES * cities)
{
	if (i && k != CITY_SIZE - 1) {
		i--; k++;
		for (int j = i; j <= k; j++) {
			for (int l = j + 1; l < CITY_SIZE; l++) {
				Delta[j][l] = calc_delta(j, l, tmp, cities);

			}

		}

		for (int j = 0; j < k; j++) {
			for (int l = i; l <= k; l++) {
				if (j >= l) continue;
				Delta[j][l] = calc_delta(j, l, tmp, cities);

			}

		}

	}// 如果不是边界，更新(i-1, k + 1)之间的 
	else {
		for (i = 0; i < CITY_SIZE - 1; i++)
		{
			for (k = i + 1; k < CITY_SIZE; k++)
			{
				Delta[i][k] = calc_delta(i, k, tmp, cities);
			}
		}

	}// 边界要特殊更新 
}



//本地局部搜索，边界条件 max_no_improve
//best_solution最优解
//current_solution当前解
void local_search(SOLUTION & best_solution, CITIES * cities, int max_no_improve)
{
	int count = 0;
	int i, k;

	int inital_cost = best_solution.cost; //初始花费

	int now_cost = 0;

	SOLUTION *current_solution = new SOLUTION; //为了防止爆栈……直接new了，你懂的

	for (i = 0; i < CITY_SIZE - 1; i++)
	{
		for (k = i + 1; k < CITY_SIZE; k++)
		{
			Delta[i][k] = calc_delta(i, k, best_solution.permutation, cities);
		}
	}

	do
	{
		//枚举排列
		for (i = 0; i < CITY_SIZE - 1; i++)
		{
			for (k = i + 1; k < CITY_SIZE; k++)
			{
				//邻域动作
				two_opt_swap(best_solution.permutation, current_solution->permutation, i, k);
				now_cost = inital_cost + Delta[i][k];
				current_solution->cost = now_cost;
				if (current_solution->cost < best_solution.cost)
				{
					count = 0; //better cost found, so reset
					for (int j = 0; j < CITY_SIZE; j++)
					{
						best_solution.permutation[j] = current_solution->permutation[j];
					}
					best_solution.cost = current_solution->cost;
					inital_cost = best_solution.cost;
					Update(i, k, best_solution.permutation, cities);
				}

			}
		}

		count++;
	} while (count <= max_no_improve);
}
//判断接受准则
bool AcceptanceCriterion(int *cities_permutation, int *old_cities_permutation, CITIES * p_cities)
{
	int acceptance = 500; //接受条件,与当前最解相差不超过acceptance
	int old_cost = cost_total(old_cities_permutation, p_cities);
	int new_cost = cost_total(cities_permutation, p_cities);

	if ((new_cost <= (old_cost + acceptance)) || (new_cost >= (old_cost - acceptance)))
	{
		return true;
	}

	return false;
}

//将城市序列分成4块，然后按块重新打乱顺序。
//用于扰动函数
void double_bridge_move(int *cities_permutation, int * new_cities_permutation)
{
	int temp_perm[CITY_SIZE];

	int pos1 = 1 + rand() % (CITY_SIZE / 4);
	int pos2 = pos1 + 1 + rand() % (CITY_SIZE / 4);
	int pos3 = pos2 + 1 + rand() % (CITY_SIZE / 4);

	int i;
	vector<int> v;
	//第一块
	for (i = 0; i < pos1; i++)
	{
		v.push_back(cities_permutation[i]);
	}

	//第二块
	for (i = pos3; i < CITY_SIZE; i++)
	{
		v.push_back(cities_permutation[i]);
	}
	//第三块
	for (i = pos2; i < pos3; i++)
	{
		v.push_back(cities_permutation[i]);
	}

	//第四块
	for (i = pos1; i < pos2; i++)
	{
		v.push_back(cities_permutation[i]);
	}


	for (i = 0; i < (int)v.size(); i++)
	{
		temp_perm[i] = v[i];
	}
	//if accept判断是否接受当前解
	if (AcceptanceCriterion(cities_permutation, temp_perm, cities))
	{
		memcpy(new_cities_permutation, temp_perm, sizeof(temp_perm));//accept
	}


}

//扰动
void perturbation(CITIES * cities, SOLUTION &best_solution, SOLUTION &current_solution)
{
	double_bridge_move(best_solution.permutation, current_solution.permutation);
	current_solution.cost = cost_total(current_solution.permutation, cities);
}

//迭代搜索
//max_iterations用于迭代搜索次数
//max_no_improve用于局部搜索边界条件
void iterated_local_search(SOLUTION & best_solution, CITIES * cities, int max_iterations, int max_no_improve)
{
	SOLUTION *current_solution = new SOLUTION;

	//获得初始随机解
	random_permutation(best_solution.permutation);


	best_solution.cost = cost_total(best_solution.permutation, cities);
	local_search(best_solution, cities, max_no_improve); //初始搜索

	for (int i = 0; i < max_iterations; i++)
	{
		perturbation(cities, best_solution, *current_solution); //扰动+判断是否接受新解
		local_search(*current_solution, cities, max_no_improve);//继续局部搜索

																//找到更优解
		if (current_solution->cost < best_solution.cost)
		{
			for (int j = 0; j < CITY_SIZE; j++)
			{
				best_solution.permutation[j] = current_solution->permutation[j];
			}
			best_solution.cost = current_solution->cost;
		}
		cout <<setw(20) << setiosflags(ios::left) << "迭代搜索 " << i << " 次\t" << "最优解 = " << best_solution.cost << " 当前解 = " << current_solution->cost << endl;
	}

}

int main()
{
	//srand((unsigned)time(NULL));//使用当前时钟作为随机数种子
	srand(1);//void srand(unsigned int seed)
	int max_iterations = 600;
	int max_no_improve = 50;
	//初始化指针数组 
	Delta = new int*[CITY_SIZE];
	for (int i = 0; i < CITY_SIZE; i++)
		Delta[i] = new int[CITY_SIZE];

	SOLUTION best_solution;

	iterated_local_search(best_solution, berlin31, max_iterations, max_no_improve);

	cout << endl << endl << "搜索完成！ 最优路线总长度 = " << best_solution.cost << endl;
	cout << "最优访问城市序列如下：" << endl;
	for (int i = 0; i < CITY_SIZE-1; i++)
	{
		cout << setiosflags(ios::left) << best_solution.permutation[i]<<"->";
	}
	cout << best_solution.permutation[CITY_SIZE-1] << endl;
	cout << endl << endl;
	system("pause");
	return 0;
}