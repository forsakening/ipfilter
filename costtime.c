//create @20170511
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "costtime.h"

//COSTTIME模块打印函数
#define COSTTIME_LOG_DEBUG(fmt,...) printf("[COSTTIME][DEBUG][%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define COSTTIME_LOG_ERROR(fmt,...) printf("[COSTTIME][ERROR][%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

//linux x86 原子锁
#define ATOMIC_BOOL_COM_SWAP(ptr, old_val, new_val) __sync_bool_compare_and_swap((ptr), (old_val), (new_val))

typedef struct
{
    int loop;                //标记是否需要循环统计
	int calcCnt;             //总共需要计算的次数
	int currentCnt;          //当前已经计算的次数
	struct timespec time_s;  //用于记录代码块前时间戳
	struct timespec time_d;  //用于记录代码块后时间戳
	uint64_t max_time;       //用于记录代码块执行的最大时间
	uint64_t min_time;       //用于记录代码块执行的最小时间
	uint64_t total;          //用于计算总共的执行时间
	uint64_t avg_time;       //用于计算代码块执行的平均时间，ns 纳秒
	COSTTIME_CB callBack;    //到达统计次数后，执行回调
}COSTTIME_INFO;

static COSTTIME_INFO g_costtime_info[MAX_COSTTIME_MODULE_NUM];
static int costtime_module_num;
static int costtimeLock = 0;

int costtime_init(int moduleID, int calcCnt, int loop, COSTTIME_CB callBack)
{
	if (NULL == callBack)
	{
		COSTTIME_LOG_ERROR("Callback function NULL! \n");
		return -1;
	}
	
	if (costtime_module_num >= MAX_COSTTIME_MODULE_NUM)
	{
		COSTTIME_LOG_ERROR("Costtime Module Number already more than %d !\n", MAX_COSTTIME_MODULE_NUM);
		return -1;
	}

	if (costtime_module_num > moduleID)
	{
		COSTTIME_LOG_ERROR("moduleID:%d already init!\n", moduleID);
		return -1;
	}

	g_costtime_info[moduleID].loop = loop;
	g_costtime_info[moduleID].calcCnt = calcCnt;
	g_costtime_info[moduleID].callBack = callBack;
	g_costtime_info[moduleID].min_time = 0xffffffff;
	
	costtime_module_num++;
	COSTTIME_LOG_DEBUG("Init Module:%d, CalcCnt:%d Ok, Current CosttimeNum:%d !\n", \
						moduleID, calcCnt, costtime_module_num);
	return 0;
}

void costtime_tag_start(int moduleID)
{
	clock_gettime(0, &(g_costtime_info[moduleID].time_s));
	return ;
}

void costtime_tag_end(int moduleID)
{
	clock_gettime(0, &(g_costtime_info[moduleID].time_d));
	g_costtime_info[moduleID].currentCnt++;

	//计算一些统计信息
	struct timespec* time_s = &(g_costtime_info[moduleID].time_s);
	struct timespec* time_d = &(g_costtime_info[moduleID].time_d);
	uint64_t min_time = g_costtime_info[moduleID].min_time;
	uint64_t max_time = g_costtime_info[moduleID].max_time;
	uint64_t leap = ((uint64_t)(time_d->tv_sec)-(uint64_t)(time_s->tv_sec))*1000000000+(uint64_t)(time_d->tv_nsec-time_s->tv_nsec);		
	if (min_time > leap) 
		min_time = leap;
	
	if (max_time < leap) 
		max_time = leap;

	g_costtime_info[moduleID].max_time = max_time;
	g_costtime_info[moduleID].min_time = min_time;
	g_costtime_info[moduleID].total += leap;

	//如果大于次数，进行回调
	if (g_costtime_info[moduleID].currentCnt >= g_costtime_info[moduleID].calcCnt)
	{
		g_costtime_info[moduleID].callBack(moduleID);

		//重新初始化
		if (g_costtime_info[moduleID].loop == 1)
		{
			g_costtime_info[moduleID].currentCnt = 0;
			g_costtime_info[moduleID].total = 0;
			g_costtime_info[moduleID].max_time = 0;
			g_costtime_info[moduleID].min_time = 0xffffffff;
		}
	}
	return ;
}

void costtime_get_stat(int moduleID, COSTTIME_STAT* stat)
{
	if (NULL == stat || moduleID >= MAX_COSTTIME_MODULE_NUM)
		return ;

	stat->calcCnt = g_costtime_info[moduleID].calcCnt;
	stat->currentCnt = g_costtime_info[moduleID].currentCnt;
	stat->max_time = g_costtime_info[moduleID].max_time;
	stat->min_time = g_costtime_info[moduleID].min_time;
	stat->total = g_costtime_info[moduleID].total;
	stat->avg_time = g_costtime_info[moduleID].total / g_costtime_info[moduleID].currentCnt;

	return;
}

//为一个hash表的某个桶加锁
void costtime_lock()
{
	while(1)
	{
		if (ATOMIC_BOOL_COM_SWAP(&costtimeLock, 0, 1))
			break;
	}

	return ;
}

//释放一个hash表的某个桶锁
void costtime_unlock()
{
	costtimeLock = 0;	
	return ;
}

void costtime_default_callback(int moduleID)
{
	COSTTIME_STAT stat;
	costtime_get_stat(moduleID, &stat);

	costtime_lock();
	COSTTIME_LOG_DEBUG("=============== Costtime MID:%d Output Start ================== \n", moduleID);
	COSTTIME_LOG_DEBUG("Total  Cnt:%20d \n", stat.calcCnt);
	COSTTIME_LOG_DEBUG("Curr   Cnt:%20d \n", stat.currentCnt);
	COSTTIME_LOG_DEBUG("Min   Time:%20lu ns \n", stat.min_time);
	COSTTIME_LOG_DEBUG("Max   Time:%20lu ns \n", stat.max_time);
	COSTTIME_LOG_DEBUG("Total Time:%20lu ns \n", stat.total);
	COSTTIME_LOG_DEBUG("Avrg  Time:%20lu ns \n", stat.avg_time);
    COSTTIME_LOG_DEBUG("=============== Costtime MID:%d Output End ================== \n", moduleID);
	costtime_unlock();
	
	return ;
}

