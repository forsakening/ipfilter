# ipfilter
Use hash-table for ipv4 match

Usage：
make clean;make
./test_filter

=======performance
[COSTTIME][DEBUG][costtime_default_callback:147]=============== Costtime MID:1 Output Start ================== 
[COSTTIME][DEBUG][costtime_default_callback:148]Total  Cnt:               10000 
[COSTTIME][DEBUG][costtime_default_callback:149]Curr   Cnt:               10000 
[COSTTIME][DEBUG][costtime_default_callback:150]Min   Time:                  69 ns 
[COSTTIME][DEBUG][costtime_default_callback:151]Max   Time:               96061 ns 
[COSTTIME][DEBUG][costtime_default_callback:152]Total Time:            10077842 ns 
[COSTTIME][DEBUG][costtime_default_callback:153]Avrg  Time:                1007 ns 
[COSTTIME][DEBUG][costtime_default_callback:154]=============== Costtime MID:1 Output End ================== 
[COSTTIME][DEBUG][costtime_default_callback:147]=============== Costtime MID:1 Output Start ================== 
[COSTTIME][DEBUG][costtime_default_callback:148]Total  Cnt:               10000 
[COSTTIME][DEBUG][costtime_default_callback:149]Curr   Cnt:               10000 
[COSTTIME][DEBUG][costtime_default_callback:150]Min   Time:                  89 ns 
[COSTTIME][DEBUG][costtime_default_callback:151]Max   Time:              236195 ns 
[COSTTIME][DEBUG][costtime_default_callback:152]Total Time:            10908027 ns 
[COSTTIME][DEBUG][costtime_default_callback:153]Avrg  Time:                1090 ns 
[COSTTIME][DEBUG][costtime_default_callback:154]=============== Costtime MID:1 Output End ==================

means:the match interface cost about 1000ns !

Thanks for contact forsakening@sina.cn
