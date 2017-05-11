//create @20170511
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "costtime.h"

//COSTTIMEģ���ӡ����
#define COSTTIME_LOG_DEBUG(fmt,...) printf("[COSTTIME][DEBUG][%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define COSTTIME_LOG_ERROR(fmt,...) printf("[COSTTIME][ERROR][%s:%d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

//linux x86 ԭ����
#define ATOMIC_BOOL_COM_SWAP(ptr, old_val, new_val) __sync_bool_compare_and_swap((ptr), (old_val), (new_val))

typedef struct
{
    int loop;                //����Ƿ���Ҫѭ��ͳ��
	int calcCnt;             //�ܹ���Ҫ����Ĵ���
	int currentCnt;          //��ǰ�Ѿ�����Ĵ���
	struct timespec time_s;  //���ڼ�¼�����ǰʱ���
	struct timespec time_d;  //���ڼ�¼������ʱ���
	uint64_t max_time;       //���ڼ�¼�����ִ�е����ʱ��
	uint64_t min_time;       //���ڼ�¼�����ִ�е���Сʱ��
	uint64_t total;          //���ڼ����ܹ���ִ��ʱ��
	uint64_t avg_time;       //���ڼ�������ִ�е�ƽ��ʱ�䣬ns ����
	COSTTIME_CB callBack;    //����ͳ�ƴ�����ִ�лص�
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

	//����һЩͳ����Ϣ
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

	//������ڴ��������лص�
	if (g_costtime_info[moduleID].currentCnt >= g_costtime_info[moduleID].calcCnt)
	{
		g_costtime_info[moduleID].callBack(moduleID);

		//���³�ʼ��
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

//Ϊһ��hash���ĳ��Ͱ����
void costtime_lock()
{
	while(1)
	{
		if (ATOMIC_BOOL_COM_SWAP(&costtimeLock, 0, 1))
			break;
	}

	return ;
}

//�ͷ�һ��hash���ĳ��Ͱ��
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

