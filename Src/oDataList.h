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
	oDL_maxrowheight    = 104,
	oDL_groupcalcs		= 120,
	oDL_treeIndent		= 121,
};

enum oDataListEventIDs {
	oDL_evClick			= 100,
};

enum oDataListHittest {
	oDL_none, oDL_horzSplitter, oDL_treeIcon, oDL_node, oDL_row
};

struct sDLHitTest {
	oDataListHittest	mAbove;
	unsigned int		mColNo;
	oDLNode *			mNode;
	qlong				mLineNo;
};

class oDataList : public oBaseVisComponent {
private:
	bool						mRebuildNodes;														// if true we need to rebuild our nodes
	bool						mUpdatePositions;													// update our position data

	bool						mShowSelected;														// if true we show selected lines, if false we only show the current line
	qdim						mIndent;															// Indent for our tree
	qdim						mLineSpacing;														// Empty space between lines in pixels
	
	qArray<qstring *>			mGroupCalculations;													// Calculations by which we're grouping
	
	qlong						mColumnCount;														// Number of columns we are displaying
	qArray<qstring *>			mColumnCalculations;												// Calculations for displaying our column data
	qdimArray					mColumnWidths;														// Our column widths
	qArray<qjst>				mColumnAligns;														// Our column aligns
	qdim						mMaxRowHeight;														// Maximum rowheight

	oDLNode						mRootNode;															// Our root node
	
	sDLHitTest					mMouseHitTest;														// Our hittest info when our mouse button was pressed
	qpoint						mMouseLast;															// Last mouse position to calculate deltas
	
	void						clearGroupCalcs(void);												// Clear our group calculations
	void						clearColumnCalcs(void);												// Clear our column calculations
	
	void						checkColumns(void);													// Check if our column data is complete
	qdim						drawDividers(qdim pTop, qdim pBottom);								// Draw divider lines
	qdim						drawNode(EXTCompInfo* pECI, oDLNode &pNode, EXTqlist* pList, qdim pIndent, qdim pTop);	// Draw this node
	qdim						drawRow(EXTCompInfo* pECI, qlong pLineNo, EXTqlist* pList, qdim pIndent, qdim pTop);	// Draw this row
	
	void						clearHitTest(void);													// clear our hitttest info
	sDLHitTest					doHitTest(qpoint pAt);												// find what we are above
	
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
	virtual qbool				getPrimaryData(EXTfldval &pGetValue);								// Retrieves our primary data, return false if we do not manage a copy
	virtual qlong				cmpPrimaryData(EXTfldval &pWithValue);								// Compare with our primary data, return true if same
	
	// methods
	static  qMethods *			methods(void);														// return array of method meta data
	virtual int					invokeMethod(qlong pMethodId, EXTCompInfo* pECI);					// invoke a method
	
	// events
	static	qEvents *			events(void);														// return an array of events meta data
	
	// mouse
	virtual HCURSOR				getCursor(qpoint pAt, qword2 pHitTest);								// return the mouse cursor we should show
	virtual void				evMouseLDown(qpoint pDownAt);										// mouse left button pressed down
	virtual void				evMouseLUp(qpoint pDownAt);											// mouse left button released
	virtual void				evMouseMoved(qpoint pMovedTo);										// mouse moved to this location while mouse button is not down
	virtual bool				canDrag(qpoint pFrom);												// Can we drag from this location? Return false if we can't
	virtual void				evClick(qpoint pAt, EXTCompInfo* pECI);								// mouse click at this location
	
};

#endif
