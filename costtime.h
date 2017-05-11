#ifndef __COST_TIME_H__
#define __COST_TIME_H__

#include <stdint.h>

////////////////////////////////////////////////////////////////
////////////////////////��ʱͳ����Ϣ////////////////////////////
////////////////////////////////////////////////////////////////

#define MAX_COSTTIME_MODULE_NUM 32

typedef struct
{
	int      calcCnt;        //���ڼ�¼��ǰ�������Ҫͳ�ƵĴ���
	int      currentCnt;     //���ڼ�¼��ǰ������Ѿ�ִ�еĴ���
	uint64_t max_time;       //���ڼ�¼�����ִ�е����ʱ�䣬ns ����
	uint64_t min_time;       //���ڼ�¼�����ִ�е���Сʱ�䣬ns ����
	uint64_t total;          //���ڼ����ܹ���ִ��ʱ�䣬ns ����
	uint64_t avg_time;       //���ڼ�������ִ�е�ƽ��ʱ�䣬ns ����
}COSTTIME_STAT;

//��ʱͳ�ƻص���������ͳ�ƴ����ﵽָ��ʱ������øûص�
typedef void (*COSTTIME_CB)(int moduleID); 

//��ʱͳ�Ƴ�ʼ���ӿڣ�����ʹ��ģ����Ҫͳ��ʱ��ʼ��һ��
//moduleID - ģ��ID������ģ��ȷ����ͬ,���� >= 0 С�� MAX_COSTTIME_MODULE_NUM
//calcCnt  - ��ģ��ͳ�ƴ���������˴���ʱ����һ�μ���
//loop     - ��ǴﵽԤ����ͳ�ƴ������Ƿ����ͳ�ƣ�loop = 1 ����ͳ��
//callBack - ��ʱͳ�ƻص���������ͳ�ƴ����ﵽָ��ʱ������øûص�
int costtime_init(int moduleID, int calcCnt, int loop, COSTTIME_CB callBack);

//�ڱ�ͳ�ƴ����֮ǰ����һ��
void costtime_tag_start(int moduleID);

//�ڱ�ͳ�ƴ����֮�����һ��
void costtime_tag_end(int moduleID);

//������֮���ȡͳ����Ϣ
void costtime_get_stat(int moduleID, COSTTIME_STAT* stat);

//costtime ģ��Ĭ�ϵĻص�����
void costtime_default_callback(int moduleID);

#endif

