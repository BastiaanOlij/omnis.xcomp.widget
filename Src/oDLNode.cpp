/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDLNode.cpp
 *  Implementation of our datalist node object
 *
 *  Bastiaan Olij
 */

#include "oDLNode.h"

oDLNode::oDLNode(void) {
	mTouched		= true;
	mExpanded		= true;
	mLineNo			= 0;
	mDescription	= QTEXT("");
	mTop			= 0;
	mBottom			= 0;
};

oDLNode::oDLNode(qstring &pDescription, qlong pLineNo) {
	mTouched		= true;
	mExpanded		= true;
	mLineNo			= pLineNo;
	mDescription	= pDescription;
	mTop			= 0;
	mBottom			= 0;	
};

oDLNode::~oDLNode(void) {
	clearChildNodes();
};

////////////////////////////////////////////////
// properties
////////////////////////////////////////////////

// touched?
bool	oDLNode::touched(void) {
	return mTouched;
};

// set value of touched
void	oDLNode::setTouched(bool pTouched) {
	mTouched = pTouched;
};

// expanded?
bool	oDLNode::expanded(void) {
	return mExpanded;
};

// set expanded
void	oDLNode::setExpanded(bool pExpanded) {
	mExpanded = pExpanded;
};

// related line number
qlong	oDLNode::lineNo(void) {
	return mLineNo;
};

// description
const qchar *	oDLNode::description(void) {
	return mDescription.cString();
};

////////////////////////////////////////////////
// methods
////////////////////////////////////////////////

// Clear all the child nodes
void	oDLNode::clearChildNodes() {
	// free all the children!
	while (mChildNodes.numberOfElements()>0) {
		oDLNode *child = mChildNodes.pop();
		delete child;
	};	
};

// Add this node
void	oDLNode::addNode(oDLNode * pNewNode) {
	mChildNodes.push(pNewNode);
};

// Returns the number of child nodes
unsigned long	oDLNode::childNodeCount() {
	return mChildNodes.numberOfElements();
};

// Find a child node by description
oDLNode	*	oDLNode::findChildByDescription(qstring & pDescription) {
	for (unsigned long index = 0; index < mChildNodes.numberOfElements(); index++) {
		oDLNode *child = mChildNodes[index];
	
		if (pDescription == child->description()) {
			return child;
		};
	};
	return NULL;
};

// Get child at specific index
oDLNode *	oDLNode::getChildByIndex(unsigned long pIndex) {
	if (pIndex < mChildNodes.numberOfElements()) {
		return mChildNodes[pIndex];
	} else {
		return NULL;
	};
};


// Add a line
void	oDLNode::addLineNo(qlong pLineNo) {
	mLineNodes.push(pLineNo);
};

// Returns the number of line nodes
unsigned long	oDLNode::lineNodeCount() {
	return mLineNodes.numberOfElements();
};

// Get the line number at this index
qlong	oDLNode::getLineNoAtIndex(unsigned long pIndex) {
	if (pIndex < mLineNodes.numberOfElements()) {
		return mLineNodes[pIndex];
	} else {
		return 0;
	};
};

// Marks all children as untouched and remove any line nodes
void	oDLNode::unTouchChildren(void) {
	for (unsigned long index = 0; index < mChildNodes.numberOfElements(); index++) {
		oDLNode *child = mChildNodes[index];
		child->setTouched(false);
		child->unTouchChildren();
	};
	
	mLineNodes.clear();
};

// Removes children that are untouched
void	oDLNode::removeUntouched(void) {
	unsigned long index = 0;
	
	while (index < mChildNodes.numberOfElements()) {
		oDLNode * child = mChildNodes[index];
		if (child->touched()) {
			child->removeUntouched();
			index++;
		} else {
			mChildNodes.remElementAtIndex(index);
			delete child;
		};
	};
};
