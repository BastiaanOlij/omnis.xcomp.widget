//
//  monitor_mac.mm
//
//
//  Created by Bas Olij on 16/05/2016.
//
//

#include "monitor_mac.h"
#include <ApplicationServices/ApplicationServices.h>

monitor_mac::monitor_mac(void) {
	/* inspired on https://gist.github.com/atr000/207602 */

	CGDisplayCount maxDisplays = MAX_MONITOR_INFO;
	CGDirectDisplayID onlineDspys[maxDisplays];
	CGDisplayCount dspyCnt;

	// get list of current active displays
	CGGetOnlineDisplayList(maxDisplays, onlineDspys, &dspyCnt);

	mNumActiveMonitors = dspyCnt;

	// now get some details about our monitors
	for (int i = 0; i < mNumActiveMonitors; i++) {
		CGRect rect = CGDisplayBounds(onlineDspys[i]);
		mMonitors[i].left = rect.origin.x;
		mMonitors[i].top = rect.origin.y;
		mMonitors[i].right = rect.origin.x + rect.size.width;
		mMonitors[i].bottom = rect.origin.y + rect.size.height;
	};
};