test_buffer_client:test_buffer_client.cpp
	g++ $^ -o $@ -levent -std=c++11
	./$@
clean:
	rm -rf test_buffer_client
kill:
	pkill -9 make
