#include <stdio.h>
#include <pcap.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "ip_filter.h"
#include "costtime.h"

static IP_FILTER_TABLE filter_table;
#define MAX_IPV4_NUM 200000
#define ADD_IPV4_NUM 100000
#define COSTTIME_MODULE_ID 1

//回调函数
void pcap_handle(u_char* user,const struct pcap_pkthdr* header,const u_char* pkt_data)
{
	uint8_t* pkt = (uint8_t *)pkt_data;
	uint16_t len = header->len;

	costtime_tag_start(COSTTIME_MODULE_ID);
	int32_t ipv4;
	if (IP_FILTER_OK == pkt_ip_match(pkt, len, &ipv4, &filter_table))
	{
		//printf("Match A pkt, ip:%d !\n", ipv4);
	}
	costtime_tag_end(COSTTIME_MODULE_ID);
	
	return;
}


int init_filter_costtime(void)
{
	//初始化filter模块
	if (IP_FILTER_OK != ip_filter_init(MAX_IPV4_NUM, &filter_table))
	{
		return -1;
	}

	int i, ipv4Addr;
	for (i = 0; i < ADD_IPV4_NUM; i++)
	{
		//添加一定数量的随机IP进入表中，测试压力
		ipv4Addr = (int32_t)random();
		if (IP_FILTER_OK != ip_filter_add(ipv4Addr, &filter_table))
		{
			printf("ID:%d ==========>Error====>Add ip:%d Error!\n", i, ipv4Addr);
		}
	}

	//添加几个常见的IP
	//www.163.com 0x376F130A 55.111.19.10
	//本机ip 0xC0A81180
	ipv4Addr = 0x376F130A;
	if (IP_FILTER_OK != ip_filter_add(ipv4Addr, &filter_table))
	{
		printf("ID:%d ==========>Error====>Add ip:%d Error!\n", i, ipv4Addr);
	}

	ipv4Addr = 0xC0A81180;
	if (IP_FILTER_OK != ip_filter_add(ipv4Addr, &filter_table))
	{
		printf("ID:%d ==========>Error====>Add ip:%d Error!\n", i, ipv4Addr);
	}

	//初始化COSTTIME模块
	if ( 0 > costtime_init(COSTTIME_MODULE_ID, 500, 1, costtime_default_callback))
		return -1;

	return 0;
}

int main(int argc, char **argv)
{
    char *device = argv[1];
    char errbuf[1024];
    pcap_t *phandle;
    
    bpf_u_int32 ipaddress,ipmask;
    struct bpf_program fcode;
    int datalink;
    
    phandle=pcap_open_live(device,1600,0,500,errbuf);
    if(phandle==NULL){
        perror(errbuf);
        return 1;
    }
    
    if(pcap_lookupnet(device,&ipaddress,&ipmask,errbuf)==-1){
        perror(errbuf);
        return 1;
    }
    else{
        char ip[INET_ADDRSTRLEN],mask[INET_ADDRSTRLEN];
        if(inet_ntop(AF_INET,&ipaddress,ip,sizeof(ip))==NULL)
            perror("inet_ntop error");
        else if(inet_ntop(AF_INET,&ipmask,mask,sizeof(mask))==NULL)
            perror("inet_ntop error");
        printf("IP address: %s, Network Mask: %s\n",ip,mask);
    }
    
    int flag=1;
    while(flag){
        if(pcap_compile(phandle,&fcode,NULL,0,ipmask)==-1)
            fprintf(stderr,"pcap_compile: %s,please input again....\n",pcap_geterr(phandle));
        else
            flag=0;
    }
    
    if(pcap_setfilter(phandle,&fcode)==-1){
        fprintf(stderr,"pcap_setfilter: %s\n",pcap_geterr(phandle));
        return 1;
    }
    
    if((datalink=pcap_datalink(phandle))==-1){
        fprintf(stderr,"pcap_datalink: %s\n",pcap_geterr(phandle));
        return 1;
    }
    
    printf("datalink= %d\n",datalink);

	if (0 > init_filter_costtime())
		return 0;

    pcap_loop(phandle,-1,pcap_handle,NULL);
    
    return 0;
}

