#include <string.h>
#ifndef _WIN32 
#include <signal.h>
#endif 
#include <event2/event.h>
#include <iostream>
using namespace std;

int main()
{
#ifdef _WIN32
    //初始化socket库
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#else 
    //忽略管道信号，发送数据给已关闭的socket
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        return 1;
#endif
    std::cout << "test conf!\n";
    //创建配置的上下文
    event_config* conf = event_config_new();
    
	//显示支持的网络模式
	const char** methods = event_get_supported_methods();
	cout << "supported methods:" << endl;
	for (int i = 0; methods[i] != NULL; i++) {
		cout << methods[i] << endl;
	}

	//设置特征
    //设置了EV_FEATURE_FDS其他特征就无法设置，再windows中EV_FEATURE_FDS无效
	//event_config_require_features(conf, EV_FEATURE_FDS|EV_FEATURE_ET);
	event_config_require_features(conf, EV_FEATURE_FDS);

	//初始化配置libevent的上下文
	event_base* base = event_base_new_with_config(conf);
	event_config_free(conf);

	if (!base) {
		cout << "event_base_new_with_config failed" << endl;
		base = event_base_new();
		if (!base)
		{
			cerr << "event_base_new failed！" << endl;
			return 0;
		}
	}
	else {
		//确认特征是否生效
		int f = event_base_get_features(base);
		if (f&EV_FEATURE_ET)
			cout << "EV_FEATURE_ET events are supported." << endl;
		else
			cout << "EV_FEATURE_ET events are not supported." << endl;
		if (f&EV_FEATURE_O1)
			cout << "EV_FEATURE_O1 events are supported." << endl;
		else
			cout << "EV_FEATURE_O1 events are not supported." << endl;
		if (f&EV_FEATURE_FDS)
			cout << "EV_FEATURE_FDS events are supported." << endl;
		else
			cout << "EV_FEATURE_FDS events are not supported." << endl;
		if (f&EV_FEATURE_EARLY_CLOSE)
			cout << "EV_FEATURE_EARLY_CLOSE events are supported." << endl;
		else
			cout << "EV_FEATURE_EARLY_CLOSE events are not supported." << endl;
		cout << "event_base_new_with_config successed" << endl;
		event_base_free(base);
	}

#ifdef _WIN32
    WSACleanup();
    system("pause");
#endif
    return 0;
}

