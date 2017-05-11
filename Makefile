test_filter:
	gcc -Wall -g -o test_filter ./costtime.c ./ip_filter.c ./test_filter.c
test_pcap_filter:
	gcc -Wall -g -lpcap -o test_pcap_filter ./costtime.c ./ip_filter.c ./test_pcap_filter.c
clean:
	rm -rf ./test_filter ./test_pcap_filter
