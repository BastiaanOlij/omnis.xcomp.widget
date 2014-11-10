/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  widget.cpp
 *  Widget library implementation
 *
 *  Bastiaan Olij
 */

#include "widget.h"

qshort	mainlib::major() {
	return 1;
};

qshort	mainlib::minor() {
	return 8;
};

/* see omnis.xcomp.framework\oXCompLib.h for methods to implement here */ 
qint mainlib::ecm_connect(void) {
	OXFcomponent	lvComponent;
	
	// add our component definitions here...
	lvComponent.componentType		= cObjType_NVObject;
	lvComponent.componentID			= 2000;
	lvComponent.bitmapID			= 0;
	lvComponent.flags				= 0;
	lvComponent.groupResID			= 0;
	lvComponent.newObjectFunction	= (void * (*)()) &(oDateTime::newObject);
	lvComponent.mProperties			= oDateTime::properties();
	lvComponent.mMethods			= oDateTime::methods();
	lvComponent.mEventMethodID		= 0;
	lvComponent.mEvents				= oDateTime::events();
	
	addComponent(lvComponent);
	
	lvComponent.componentType		= cObjType_Basic;
	lvComponent.componentID			= 2001;
	lvComponent.bitmapID			= 1;
	lvComponent.flags				= 0;
	lvComponent.groupResID			= 0;
	lvComponent.newObjectFunction	= (void * (*)()) &(oCountButton::newObject);
	lvComponent.mProperties			= oCountButton::properties();
	lvComponent.mMethods			= oCountButton::methods();
	lvComponent.mEventMethodID		= 10000;
	lvComponent.mEvents				= oCountButton::events();
	
	addComponent(lvComponent);	

	lvComponent.componentType		= cObjType_Basic;
	lvComponent.componentID			= 2002;
	lvComponent.bitmapID			= 1;
	lvComponent.flags				= 0;
	lvComponent.groupResID			= 0;
	lvComponent.newObjectFunction	= (void * (*)()) &(oDataList::newObject);
	lvComponent.mProperties			= oDataList::properties();
	lvComponent.mMethods			= oDataList::methods();
	lvComponent.mEventMethodID		= 10010;
	lvComponent.mEvents				= oDataList::events();
	
	addComponent(lvComponent);	

	lvComponent.componentType		= cObjType_DropList;
	lvComponent.componentID			= 2003;
	lvComponent.bitmapID			= 1;
	lvComponent.flags				= 0;
	lvComponent.groupResID			= 0;
	lvComponent.newObjectFunction	= (void * (*)()) &(oDropDown::newObject);
	lvComponent.mProperties			= oDropDown::properties();
	lvComponent.mMethods			= oDropDown::methods();
	lvComponent.mEventMethodID		= 10020;
	lvComponent.mEvents				= oDropDown::events();

	addComponent(lvComponent);

	lvComponent.componentType		= cObjType_NVObject;
	lvComponent.componentID			= 2004;
	lvComponent.bitmapID			= 0;
	lvComponent.flags				= 0;
	lvComponent.groupResID			= 0;
	lvComponent.newObjectFunction	= (void * (*)()) &(oImage::newObject);
	lvComponent.mProperties			= oImage::properties();
	lvComponent.mMethods			= oImage::methods();
	lvComponent.mEventMethodID		= 0;
	lvComponent.mEvents				= oImage::events();
		
	addComponent(lvComponent);	
	
	return oXCompLib::ecm_connect();
};

qbool	mainlib::ecm_disconnect(void) {
	// nothing to do here..

	return oXCompLib::ecm_disconnect();
};

int	mainlib::invokeMethod(qlong pMethodId, EXTCompInfo* pECI) {
	// nothing to do here..

	return oXCompLib::invokeMethod(pMethodId, pECI);
};
