test_thread_pool:test_thread_pool.cpp XThreadPool.cpp XThread.cpp XFtpServerCMD.cpp
	g++ $^ -o $@ -levent -lpthread -std=c++11
	./$@
clean:
	rm -rf test_thread_pool 
	rm -rf *.o
