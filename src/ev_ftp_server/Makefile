SRC=ev_ftp_server.cpp XThreadPool.cpp XThread.cpp XFtpServerCMD.cpp\
	XFtpFactory.cpp XFtpTask.cpp XFtpUSER.cpp 

ev_ftp_server:$(SRC)	
	g++ $^ -o $@ -levent -lpthread -std=c++11
	./$@
clean:
	rm -rf ev_ftp_server
	rm -rf *.o
