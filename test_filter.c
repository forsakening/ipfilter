#include "ip_filter.h"
#include "costtime.h"
#include <stdio.h>
#include <stdlib.h>

#define COSTTIME_MODULE_ID 1
#define MAX_IPV4_NUM 200000
#define ADD_IPV4_NUM 100000
#define INVALID_IPV4 0xffffffff
int32_t test_array_intable[ADD_IPV4_NUM] = {0};
int32_t test_array_random[ADD_IPV4_NUM] = {0};
IP_FILTER_TABLE g_filter_table = {0};

int test_func()
{
	//初始化costtime
	costtime_init(COSTTIME_MODULE_ID, 10000, 1, costtime_default_callback);
	
	//random ipv4 addr add
	int32_t ipAddr;
	int i = 0;
	for (; i < ADD_IPV4_NUM; i++)
	{
		ipAddr = (int32_t)random();
		if (IP_FILTER_OK != ip_filter_add(ipAddr, &g_filter_table))
		{
			printf("ID:%d ==========>Error====>Add ip:%d Error!\n", i, ipAddr);
		}
		else
		{
			//printf("ID:%d ==========>OK====>Add ip:%d OK!\n", i, ipAddr);
		}

		test_array_intable[i] = ipAddr;
	}

	for (i = 0; i < ADD_IPV4_NUM; i++)
	{
		ipAddr = (int32_t)random();
		test_array_random[i] = ipAddr;
	}

	//match ipv4
	int j;
	for (j = 0; j < 10000; j++)
	{
		for (i = 0; i < ADD_IPV4_NUM; i++)
		{
			//ipAddr = test_array_intable[i];
			ipAddr = test_array_random[i];
			
			costtime_tag_start(COSTTIME_MODULE_ID);

			if (IP_FILTER_OK != ip_filter_match(ipAddr, &g_filter_table))
			{
				//printf("ID:%d ==========>Error====>Match ip:%d Error!\n", i, ipAddr);
			}
			else
			{
				//printf("ID:%d ==========>OK====>Match ip:%d OK!\n", i, ipAddr);
			}

			costtime_tag_end(COSTTIME_MODULE_ID);
		}
	}
	
	//测试不匹配的IP
	if (IP_FILTER_OK != ip_filter_match(INVALID_IPV4, &g_filter_table))
	{
		printf("Match IvalidIP ==========>Error====>Match ip:%d Error!\n", INVALID_IPV4);
	}
	else
	{
		printf("Match IvalidIP ==========>OK====>Match ip:%d OK!\n", INVALID_IPV4);
	}
	

	return 0;
}

int main()
{
	//initial the ip_filter table
	if (IP_FILTER_OK != ip_filter_init(MAX_IPV4_NUM, &g_filter_table))
	{
		return -1;
	}

	test_func();

	ip_filter_destroy(&g_filter_table);
	return 0;
}
