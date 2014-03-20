/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDataList.h
 *  Header for our datalist object
 *
 *  Omnis has a number of list controls including a very poor tree list control. I wanted something with a little more umpf.
 *  This is my attempt at creating a cool data list control.
 *  Note that on purpose I am not subclassing from the list objects in the SDK. They do not give me the level of control I wish
 *
 *  Bastiaan Olij
 */

#ifndef odatalisth
#define odatalisth

#include "omnis.xcomp.framework.h"
#include "oDLNode.h"

enum oDataListPropIDs {
	oDL_columncount		= 100,
	oDL_columncalcs		= 101,
	oDL_columnwidths	= 102,
	oDL_columnaligns	= 103,
	oDL_groupcalcs		= 104
};

enum oDataListEventIDs {
	oDL_evClick			= 100,
};

class oDataList : public oBaseVisComponent {
private:
	bool						mShowSelected;														// if true we show selected lines, if false we only show the current line
	
	qArray<qstring *>			mGroupCalculations;													// Calculations by which we're grouping
	
	qlong						mColumnCount;														// Number of columns we are displaying
	qArray<qstring *>			mColumnCalculations;												// Calculations for displaying our column data
	qdimArray					mColumnWidths;														// Our column widths
	qArray<qjst>				mColumnAligns;														// Our column aligns

	oDLNode						mRootNode;															// Our root node
	
	void						clearGroupCalcs(void);												// Clear our group calculations
	void						clearColumnCalcs(void);												// Clear our column calculations
	
	qdim						drawNode(EXTCompInfo* pECI, oDLNode &pNode, EXTqlist* pList, qdim pIndent, qdim pTop);	// Draw this node
	qdim						drawRow(EXTCompInfo* pECI, qlong pLineNo, EXTqlist* pList, qdim pIndent, qdim pTop);	// Draw this row
public:
	oDataList(void);
	~oDataList(void);
	
	static oDataList *			newObject(void);
	
	virtual void				doPaint(EXTCompInfo* pECI);											// Do our drawing in here
	
	// properties
	static  qProperties *		properties(void);													// return an array of property data
	virtual qbool				setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI);	// set the value of a property
	virtual void				getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI);	// get the value of a property

	virtual qbool				setPrimaryData(EXTfldval &pNewValue);								// Changes our primary data
	virtual void				getPrimaryData(EXTfldval &pGetValue);								// Retrieves our primary data
	virtual qbool				cmpPrimaryData(EXTfldval &pWithValue);								// Compare with our primary data
	virtual qlong				getPrimaryDataLen();												// Get our primary data size
	virtual void				primaryDataHasChanged();											// Omnis is just letting us know our primary data has changed, this is especially handy if we do not keep a copy ourselves and thus ignore the other functions
	
	
	// methods
	static  qMethods *			methods(void);														// return array of method meta data
	virtual int					invokeMethod(qlong pMethodId, EXTCompInfo* pECI);					// invoke a method
	
	// events
	static	qEvents *			events(void);														// return an array of events meta data
	virtual void				evClick(qpoint pAt);												// mouse click at this location
};

#endif
