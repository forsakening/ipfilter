#ifndef __COST_TIME_H__
#define __COST_TIME_H__

#include <stdint.h>

////////////////////////////////////////////////////////////////
////////////////////////耗时统计信息////////////////////////////
////////////////////////////////////////////////////////////////

#define MAX_COSTTIME_MODULE_NUM 32

typedef struct
{
	int      calcCnt;        //用于记录当前代码块需要统计的次数
	int      currentCnt;     //用于记录当前代码块已经执行的次数
	uint64_t max_time;       //用于记录代码块执行的最大时间，ns 纳秒
	uint64_t min_time;       //用于记录代码块执行的最小时间，ns 纳秒
	uint64_t total;          //用于计算总共的执行时间，ns 纳秒
	uint64_t avg_time;       //用于计算代码块执行的平均时间，ns 纳秒
}COSTTIME_STAT;

//耗时统计回调函数，当统计次数达到指定时，会调用该回调
typedef void (*COSTTIME_CB)(int moduleID); 

//耗时统计初始化接口，各个使用模块需要统计时初始化一次
//moduleID - 模块ID，各个模块确保不同,必须 >= 0 小于 MAX_COSTTIME_MODULE_NUM
//calcCnt  - 本模块统计次数，到达此次数时进行一次计算
//loop     - 标记达到预定的统计次数后是否继续统计，loop = 1 继续统计
//callBack - 耗时统计回调函数，当统计次数达到指定时，会调用该回调
int costtime_init(int moduleID, int calcCnt, int loop, COSTTIME_CB callBack);

//在被统计代码段之前调用一次
void costtime_tag_start(int moduleID);

//在被统计代码段之后调用一次
void costtime_tag_end(int moduleID);

//在最终之后获取统计信息
void costtime_get_stat(int moduleID, COSTTIME_STAT* stat);

//costtime 模块默认的回调函数
void costtime_default_callback(int moduleID);

#endif

