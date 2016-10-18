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
#ifdef iswin32
  /* not yet supported */
#else
#include "monitor_mac.h"
#endif

qshort	mainlib::major() {
	return OMNISSDK;
};

qshort	mainlib::minor() {
	return 118;
};

ECOmethodEvent widgetStaticFuncs[] = {
    // ID, Resource, Flags
    9000, 9000, fftList, 0, 0, 0, 0,
};

/* see omnis.xcomp.framework\oXCompLib.h for methods to implement here */ 
qint mainlib::ecm_connect(void) {
	OXFcomponent	lvComponent;

    addStaticMethods(widgetStaticFuncs, 1);
	
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
	lvComponent.componentID			= 2006;
	lvComponent.bitmapID			= 1;
	lvComponent.flags				= 0;
	lvComponent.groupResID			= 0;
	lvComponent.newObjectFunction	= (void * (*)()) &(oRoundedButton::newObject);
	lvComponent.mProperties			= oRoundedButton::properties();
	lvComponent.mMethods			= oRoundedButton::methods();
	lvComponent.mEventMethodID		= 10000;
	lvComponent.mEvents				= oRoundedButton::events();
	
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

	lvComponent.componentType		= cObjType_DropList;
	lvComponent.componentID			= 2005;
	lvComponent.bitmapID			= 1;
	lvComponent.flags				= 0;
	lvComponent.groupResID			= 0;
	lvComponent.newObjectFunction	= (void * (*)()) &(oFontDropDown::newObject);
	lvComponent.mProperties			= oFontDropDown::properties();
	lvComponent.mMethods			= oFontDropDown::methods();
	lvComponent.mEventMethodID		= 10020;
	lvComponent.mEvents				= oFontDropDown::events();

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
	switch (pMethodId) {
        case 9000: { /* $monitors - get list with monitor info */
            EXTfldval   result;
            EXTqlist *  monitorList = new EXTqlist(listVlen);
            str255      colName;
            
            /* add some columns */
            colName = QTEXT("Left");
            monitorList->addCol(1, fftInteger, 0, 0, NULL, &colName);
            colName = QTEXT("Top");
            monitorList->addCol(2, fftInteger, 0, 0, NULL, &colName);
            colName = QTEXT("Right");
            monitorList->addCol(3, fftInteger, 0, 0, NULL, &colName);
            colName = QTEXT("Bottom");
            monitorList->addCol(4, fftInteger, 0, 0, NULL, &colName);
            
#ifdef iswin32
            /* not yet supported */
#else
            /* lets see if we can call some obj-c */
            monitor_mac * moninfo = new monitor_mac();
            
            for (int i = 0; i < moninfo->mNumActiveMonitors; i++) {
                EXTfldval	colFld;
                qlong		rowNo = monitorList->insertRow();
                
                monitorList->getColValRef(rowNo, 1, colFld, qtrue);
                colFld.setLong(moninfo->mMonitors[i].left);
                monitorList->getColValRef(rowNo, 2, colFld, qtrue);
                colFld.setLong(moninfo->mMonitors[i].top);
                monitorList->getColValRef(rowNo, 3, colFld, qtrue);
                colFld.setLong(moninfo->mMonitors[i].right);
                monitorList->getColValRef(rowNo, 4, colFld, qtrue);
                colFld.setLong(moninfo->mMonitors[i].bottom);
            };
            
            delete moninfo;
#endif
            /* and return our list */
            result.setList(monitorList, qtrue);
            delete monitorList;
            
            ECOaddParam(pECI, &result);
            return 1L;
        } break;
        default: {
            return oXCompLib::invokeMethod(pMethodId, pECI);
        } break;
    }
};
