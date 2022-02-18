//
//  monitor_mac.h
//
//
//  Created by Bas Olij on 16/05/2016.
//
//

#ifndef ____monitor_mac__
#define ____monitor_mac__

#define MAX_MONITOR_INFO 32

typedef struct monitor_mac_info {
	int left;
	int top;
	int right;
	int bottom;
} monitor_mac_info;

class monitor_mac {
public:
	int mNumActiveMonitors;
	monitor_mac_info mMonitors[MAX_MONITOR_INFO];

	monitor_mac(void);
};

#endif /* defined(____monitor_mac__) */
