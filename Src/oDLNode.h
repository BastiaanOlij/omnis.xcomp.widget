/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDLNode.h
 *  Header for our datalist node object
 *
 *  oDLNode is a support object for building our tree node
 *
 *  Bastiaan Olij
 */

#ifndef odlnodeh
#define odlnodeh

#include "omnis.xcomp.framework.h"

// Forward defines
class oDLNode;
typedef qArray<oDLNode *>		oDLNodeArray;							// array of node pointers

//////////
class oDLNode {
private:
	bool			mTouched;											// if true this node is still part of our data list
	bool			mExpanded;											// if true our node is expanded
	qlong			mLineNo;											// line number in our data list this relates to, 0 if just grouping
	qstring			mDescription;										// our description
	
	// These are only set after we've drawn our tree so we can see where nodes are
	qdim			mTop;												// top position 
	qdim			mBottom;											// bottom position
	
	// nodes
	oDLNodeArray	mChildNodes;										// child nodes
	qlongArray		mLineNodes;											// line nodes

public:
	oDLNode(void);
	oDLNode(qstring &pDescription, qlong pLineNo=0);
	~oDLNode(void);
	
	// properties
	bool			touched(void);										// touched?
	void			setTouched(bool pTouched);							// set value of touched
	bool			expanded(void);										// expanded?
	void			setExpanded(bool pExpanded);						// set expanded

	// read only
	qlong			lineNo(void);										// related line number
	const qchar *	description(void);									// description
	
	// methods
	void			clearChildNodes();									// Clear all the child nodes
	void			addNode(oDLNode * pNewNode);						// Add this node
	unsigned long	childNodeCount();									// Returns the number of child nodes
	oDLNode	*		findChildByDescription(qstring & pDescription);		// Find a child node by description
	oDLNode *		getChildByIndex(unsigned long pIndex);				// Get child at specific index
	
	void			addLineNo(qlong pLineNo);							// Add a line
	unsigned long	lineNodeCount();									// Returns the number of line nodes
	qlong			getLineNoAtIndex(unsigned long pIndex);				// Get the line number at this index
	
	void			unTouchChildren(void);								// Marks all children as untouched and remove any line nodes
	void			removeUntouched(void);								// Removes children that are untouched	
};



#endif
