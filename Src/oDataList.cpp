/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDataList.cpp
 *  Implementation of our datalist object
 *
 *  Todos:
 *  - find out why divider lines disappear when scrolling
 *  - make sure selection changes are restricted to our filter
 *  - improve drag and drop, including implementing a mouseover to find out what line we're dropping on. 
 *  - implement that one of our groupings can also represent a row in our list
 *  - implement totaling amounts
 *  - figure out how to make our column calculations and group calculations show up in a proper editor in the property manager
 *
 *  Bastiaan Olij
 */

#include "oDataList.h"

// Constructor to initialize object
oDataList::oDataList(void) {
	mFilter					= "";
	mColumnCount			= 1;
	mShowSelected			= false;
	mRebuildNodes			= true;
	mDrawRecurseCount		= 0;
	mDeselectOnNodeClick	= false;
	mEvenColor				= GDI_COLOR_QDEFAULT;
	mSelectColor			= GDI_COLOR_QDEFAULT;
	mIndent					= 20;
	mLineSpacing			= 4;
	mOmnisList				= 0;
	mLastCurrentLineTop		= 0;
    mCheckedDataName        = false;
	mInWindowScrolled		= false;
};

// Destructor to clean up
oDataList::~oDataList(void) {
	clearGroupCalcs();
	clearColumnCalcs();
};

// Clear our group calculations
void	oDataList::clearGroupCalcs(void) {
	while (mGroupCalculations.size()>0) {
		sDLGrouping grouping = mGroupCalculations.back();	// get the last entry
		mGroupCalculations.pop_back();				// remove the last entry
		
		// free memory related to our strings
		if (grouping.mGroupCalc != NULL) {
			delete grouping.mGroupCalc;
		};
		if (grouping.mParentCalc != NULL) {
			delete grouping.mParentCalc;
		};
	};	
};

// Clear our column calculations
void	oDataList::clearColumnCalcs(void) {
	while (mColumnCalculations.numberOfElements()>0) {
		qstring *calc = mColumnCalculations.pop();
		delete calc;
	};	
};

// instantiate a new object
oDataList * oDataList::newObject(void) {
	oDataList *lvNewDataList = new oDataList();
	
	return lvNewDataList;
};

// Check if our column data is complete	and we do not have widths that don't make sense..
void	oDataList::checkColumns(void) {
	for (qulong i = 0; i<mColumnCount; i++) {
		if (i>=mColumnWidths.numberOfElements()) {
			// missing a width? add it now so we can trust it later...
			mColumnWidths.push(100);
		} else if (mColumnWidths[i]<10) {
			mColumnWidths.setElementAtIndex(i, 10);
		};
		
		if (i>=mColumnAligns.numberOfElements()) {
			// make sure we've got all our aligns as well so we can trust it later..
			mColumnAligns.push(jstLeft);
		};
		
		if (i>=mColumnCalculations.numberOfElements()) {
			// make sure we've got all our column calculations as well so we can trust it later..
			mColumnCalculations.push(new qstring(QTEXT("")));
		};		
	};
};

// Draw divider lines
qdim	oDataList::drawDividers(qdim pTop, qdim pBottom) {
	qdim	left = 0;
	for (qulong i = 0; i<mColumnCount; i++) {
		left += mColumnWidths[i];
		mCanvas->drawLine(qpoint(left-mHorzScrollPos,mClientRect.top), qpoint(left-mHorzScrollPos,mClientRect.bottom), 1, GDI_COLOR_QGRAY, patStd0); // should make the color and linestyle configurable
	};
	
	return left;
};

// Draw this node, might move this into oDLNode one day..
qdim	oDataList::drawNode(EXTCompInfo* pECI, oDLNode &pNode, qdim pIndent, qdim pTop, qlong & pListLineNo, bool & pIsEven) {
	qdim	headerHeight = 0;

	// increase our list line number and write it back into our node
	pListLineNo++;
	pNode.setListLineNo(pListLineNo);
	
	// write top info back into our node..
	pNode.setTop(pTop);
	
	if (pIndent == -1) {
		// our root node, ignore this...
		pIndent = 0;
	} else {
		bool	needIcon = pNode.childNodeCount()>0;
		
		if ((pListLineNo <= mVertScrollPos) || (pTop > mClientRect.bottom)) {
			// off screen, no need to draw but we do continue..
		} else {			
			if (pNode.lineNo()!=0) {
				// draw as a full line
				headerHeight = drawRow(pECI, pNode.lineNo(), pIndent + (needIcon ? mIndent : 0), pTop + 2, pIsEven);
				headerHeight -= pTop;
			} else {
				// Draw our description
				qrect	columnRect;
				qdim	colwidth	= 10000; // No longer using mColumnWidths[0], may make this switchable, allow groupings to go along as far as they like..
				qdim	width		= colwidth - pIndent - (needIcon ? mIndent : 0) - 4;
				
				headerHeight = 2 + mCanvas->getTextHeight(pNode.description().cString(), width > 10 ? width : 10, true, true);
				if (headerHeight > mMaxRowHeight) {
					headerHeight = mMaxRowHeight;
				};
				
				if (pIsEven && (mEvenColor!=GDI_COLOR_QDEFAULT)) {
					// draw even color background...
					qrect	backGroundRect;
					backGroundRect.left		= mClientRect.left;
					backGroundRect.top		= pTop - mHorzScrollPos;
					backGroundRect.right	= mClientRect.right;
					backGroundRect.bottom	= backGroundRect.top + headerHeight + mLineSpacing;
					
					mCanvas->drawRect(backGroundRect, mEvenColor, mEvenColor);
				};
				
				columnRect.left		= pIndent - mHorzScrollPos + (needIcon ? mIndent : 0) + 2;
				columnRect.top		= pTop + 2;
				columnRect.right	= colwidth - mHorzScrollPos - 2;
				columnRect.bottom	= columnRect.top + headerHeight;
				
				mCanvas->drawText(pNode.description().cString(), columnRect, mTextColor, jstLeft, true, true);
			};
			
			if (needIcon) {
				// now draw expanded/collapsed icon
				qrect treeRect;
				
				treeRect.left	= pIndent + 1;
				treeRect.right	= treeRect.left + mIndent;
				treeRect.top	= pTop;
				treeRect.bottom	= treeRect.top + mIndent;
				
				pNode.setTreeIconRect(treeRect);
				
                qrect at = treeRect;
                treeRect.left -= mHorzScrollPos;
				mCanvas->drawIcon((pNode.expanded() ? 1120 : 1121), treeRect);
			};
			
			headerHeight	+= mLineSpacing;	// add some spacing..
		} 
		
		pIndent		+= mIndent;
		pIsEven		= !pIsEven;			// toggle
	};
	
	if (pNode.expanded()) {
		qulong i;
		pTop += headerHeight;
		
		// draw child nodes
		for (i = 0; i<pNode.childNodeCount(); i++) {
			oDLNode *child = pNode.getChildByIndex(i);
			pTop = drawNode(pECI, *child, pIndent, pTop, pListLineNo, pIsEven);
		};
		
		// draw totals?
	} else {
		// see if we need to draw totals...

		
		// now we can add our headerheight...
		pTop += headerHeight;
	};
	
	// write bottom  back into our node..
	pNode.setBottom(pTop);

	if ((pListLineNo > mVertScrollPos) && (pTop <= mClientRect.bottom)) {
		// only counts if it is fully visible
		mLastVisListNo = pListLineNo;
	};
	
	return pTop;
};

// Draw this line, might move this into oDLNode one day..
qdim	oDataList::drawRow(EXTCompInfo* pECI, qlong pLineNo, qdim pIndent, qdim pTop, bool pIsEven) {
	qdim				left			= 0;
	qdim				lineheight		= mCanvas->getFontHeight(); // minimum line height...
	qulong				i;
	qlong				oldCurRow		= mOmnisList->getCurRow();
	bool				isSelected		= ((mShowSelected && mOmnisList->isRowSelected(pLineNo)) || (!mShowSelected && (pLineNo == oldCurRow)));
	qArray<qstring *>	columndata;
	
	mOmnisList->setCurRow(pLineNo);
	
	// !BAS! This is now no longer called unless the line is visible.
	// We should be able to optimise this code by storing the wrapped text in columndata and save us some overhead.

	// 1) find the line height of each text to find the highest line
	for (i = 0; i < mColumnCount; i++) {
		qstring 		calcstr;
		qstring *		newdata;
		
		if (mColumnPrefix.length()!=0) {
			if (mColumnCalculations[i]->length()==0) {
				EXTfldval	dataNameFld;
				ECOgetProperty(mHWnd, anumFieldname, dataNameFld);
				qstring		dataStr(dataNameFld);
				
				calcstr.appendFormattedString("con(%qs,%qs.%li)",&mColumnPrefix, &dataStr, i);
			} else {
				calcstr.appendFormattedString("con(%qs,%qs)",&mColumnPrefix, mColumnCalculations[i]);
			};
		} else {
			calcstr = *mColumnCalculations[i];
		};
		
		// addToTraceLog(calcstr);
		
		if (calcstr.length()==0) {
			EXTfldval colFld;
			
			// just get the column...
			mOmnisList->getColValRef(pLineNo, i+1, colFld, qfalse);
			newdata = new qstring(colFld);
		} else {
			// should fine a relyable way to cache our calculations....
			// also should move this code into something more central
			EXTfldval	* calcFld = newCalculation(calcstr, pECI);			
			if (calcFld == NULL) {
				newdata = new qstring();
				*newdata += QTEXT("???");
			} else {
				EXTfldval	result;
				calcFld->evalCalculation(result, pECI->mLocLocp, mOmnisList, qfalse);
				newdata = new qstring(result);
				
				delete calcFld;
			};			
		};
				
		columndata.push(newdata);

		qdim width = mColumnWidths[i] - 4 - (i==0 ? pIndent : 0);
		qdim columnHeight = mCanvas->getTextHeight(newdata->cString(), width > 10 ? width : 10, true, true);
		if (columnHeight>lineheight && (i < 256 ? mColumnExtend[i] : false)) {				
			lineheight = columnHeight;
		};
	};
	
	// Too high?
	if (lineheight > mMaxRowHeight) {
		lineheight = mMaxRowHeight;
	};
	
	qrect	rowRect;

	// 2) Do highlighted drawing
	rowRect.left = mClientRect.left;
	rowRect.top = pTop;
	rowRect.right = mClientRect.right;
	rowRect.bottom = pTop + lineheight + mLineSpacing;
	if (isSelected) {
		if (mSelectColor == GDI_COLOR_QDEFAULT) {
			GDIhiliteTextStart(mCanvas->hdc(), &rowRect, mTextColor);	// !BAS! Move into canvas!
		} else {
			mCanvas->drawRect(rowRect, mSelectColor, mSelectColor);
		}
	} else if (pIsEven && (mEvenColor!=GDI_COLOR_QDEFAULT)) {
		mCanvas->drawRect(rowRect, mEvenColor, mEvenColor);
	};
		
	// 3) draw our text
	left = -mHorzScrollPos;
	for (i = 0; i < mColumnCount; i++) {
		qstring *	data = columndata[i];
		qrect		columnRect;
			
		if (i==0) {
			columnRect.left	= left + pIndent + 2;
		} else {
			columnRect.left	= left + 2;
		}
		columnRect.right	= left + mColumnWidths[i] - 2;
		columnRect.top		= pTop;
		columnRect.bottom	= pTop + lineheight;
			
		mCanvas->drawText(data->cString(), columnRect, mTextColor, mColumnAligns[i], true, true);
			
		left += mColumnWidths[i];
	};
		
	if (isSelected && (mSelectColor == GDI_COLOR_QDEFAULT)) {
		// 4) unhighlight
		GDIhiliteTextEnd(mCanvas->hdc(), &rowRect, mTextColor);	// !BAS! Move into canvas!
	};

	// cleanup...
	while(columndata.numberOfElements()>0) {
		qstring * olddata = columndata.pop();
		delete olddata;
	};
	
	mOmnisList->setCurRow(oldCurRow);
	
	return pTop + lineheight;
};


// Do our drawing in here
void oDataList::doPaint(EXTCompInfo* pECI) {
	// The way this is structured is that as long as the contents of the list doesn't change nor the way we display our list, we reuse as much of what we've calculated before
	// That means the first time we draw our list we calculate the size of all texts and build our nodes
	// After that we skip as much of the logic as we can and effectively only draw lines that are visible.

	// prevent recursive calls to this to go on and on and on...
	if (mDrawRecurseCount > 3) {
		addToTraceLog("Draw recurse count hit 4");
		return;
	}
	mDrawRecurseCount++;

	qbool	redraw = false; // if our scroll position changes, we draw twice!
	clock_t	t = clock();
	
	// call base class to draw background
	oBaseVisComponent::doPaint(pECI);
	
    // check if we have a list name
    if ((mListName.length() == 0) && (!mCheckedDataName)) {
        /* convert from when we used dataname */
        EXTfldval	dataNameFld;
        
        /* get the value of $dataname, i.e. "ivList" */
        ECOgetProperty(mHWnd, anumFieldname, dataNameFld);
        
        if (dataNameFld.getCharLen() != 0) {
            str255		emptyString;

            // copy it into our list name, using ECOsetProperty ensures we update our property manager
            ECOsetProperty(mHWnd, anumListName, dataNameFld);
//            addToTraceLog("Set $listname to %qs", &mListName);
            
            // and clear our dataname
            dataNameFld.setChar(emptyString);
            ECOsetProperty(mHWnd, anumFieldname, dataNameFld);
        };
        
        mCheckedDataName = true; // do this only once...
    };
    
	// check our columns
	checkColumns();
		
	if ( ECOisDesign(mHWnd) ) {
		// Don't draw anything else..
	} else {
        // Get our omnis list
		mOmnisList = getNamedList(mListName, pECI);
					
		if (mOmnisList==0) {
			// no list to draw?
		} else {
			qlong		rowCount = mOmnisList->rowCnt();
			qlong		currentRow = mOmnisList->getCurRow();
			
            mRebuildNodes = true; // for now always true, we need to improve on this somehow!
			if (mRebuildNodes) {
				// Update our nodes7
				mRootNode.unTouchChildren(); // untouch children
				
				if (rowCount!=0) {
					// !BAS! need to investigate if we can cache these calculations and reuse them....
					
					// setup our filter calculation
					EXTfldval *		filtercalc = NULL;
					if (mFilter.length() > 0) {
						filtercalc = newCalculation(mFilter, pECI);
					};
					
					// setup our grouping calculations
					EXTfldvalArray	groupcalcs;
					EXTfldvalArray	parentcalcs;
					unsigned int	group;					
					
					// loop through our list
					for (group = 0; group < mGroupCalculations.size(); group++) {
						sDLGrouping	grouping	= mGroupCalculations[group];
						EXTfldval *	calcFld		= newCalculation(*grouping.mGroupCalc, pECI);
						
						if (calcFld != NULL) {
							groupcalcs.push_back(calcFld);
							
							// also our parent calculation..
							EXTfldval * parentFld = NULL;
							if (grouping.mParentCalc != NULL) {
								parentFld = newCalculation(*grouping.mParentCalc, pECI);
							};
							parentcalcs.push_back(parentFld); // always add even if NULL
						};
					};
					
					
					for (qlong lineno = 1; lineno <= rowCount; lineno++) {
						bool		showNode = true;
						oDLNode *	node = &mRootNode;
						
						mOmnisList->setCurRow(lineno);
						
						if (filtercalc != NULL) {
							EXTfldval	result;
							filtercalc->evalCalculation(result, pECI->mLocLocp, mOmnisList, qfalse);
							if (result.getBool() != 2) {
								showNode = false;
							};
						};
						
						if (showNode) {
							bool	addFinalNode = true;
							
							// check our grouping
							group = 0;
							while(addFinalNode && (group < groupcalcs.size())) {
								EXTfldval * calcFld = groupcalcs[group];
								EXTfldval	result;
								
								calcFld->evalCalculation(result, pECI->mLocLocp, mOmnisList, qfalse);
								qstring		groupdesc(result), value;
								
								if (groupdesc.length()>0) {
									oDLNode *childnode;
									
									// we split our description by |. This will allow us to optionally include a unique identifier
									qlong	pos = groupdesc.pos('|');
									if (pos > 0) {
										value		= groupdesc.mid(0,pos);
										groupdesc	= groupdesc.mid(pos+1);
										childnode	= node->findChildByValue(value);
									} else {
										value		= "";
										if (pos == 0) {
											groupdesc	= groupdesc.mid(pos+1);
										};
										childnode	= node->findChildByDescription(groupdesc, true);
									};
									
									if (childnode == NULL) {
										childnode = new oDLNode(value, groupdesc, 0);
										node->addNode(childnode);
									} else {
										childnode->setTouched(true);
									};
									
									if (childnode->sortOrder()==0) {
										// set the sort order to the first line related to our node
										childnode->setSortOrder(lineno);
									};
									
									EXTfldval * parentFld = parentcalcs[group];
									if (parentFld != NULL) {
										EXTfldval	isParent;
										parentFld->evalCalculation(isParent, pECI->mLocLocp, mOmnisList, qfalse);
										
										if (isParent.getBool()==2) {
											childnode->setLineNo(lineno);
											addFinalNode = false; // we've already found it
										};
									};
																		
									node = childnode;									
								};
								
								group++;
							};
							
							if (addFinalNode) {
								// now add our final node, if this is a self reference node it would already have been added
								oDLNode * finalNode = node->findChildByLineNo(lineno);
								if (finalNode!=NULL) {
									finalNode->setTouched(true);
								} else {
									finalNode = new oDLNode(qstring(""), qstring(""), lineno);
									finalNode->setSortOrder(lineno);
									node->addNode(finalNode);									
								};
							};
						};
					};					
					
					// cleanup our EXTFlds
					while (parentcalcs.size()) {
						EXTfldval *	calcFld = parentcalcs.back();
						parentcalcs.pop_back();
						
						if (calcFld != NULL) {
							delete calcFld;							
						};
					};
					while (groupcalcs.size()) {
						EXTfldval *	calcFld = groupcalcs.back();
						groupcalcs.pop_back();
						
						delete calcFld;
					};
					if (filtercalc != NULL) {
						delete filtercalc;
					};
					
					// check if our hittest info is still valid
					if ((mMouseHitTest.mAbove==oDL_node) || (mMouseHitTest.mAbove==oDL_row)) {
						oDLNode *	childnode = mMouseHitTest.mNode;
						
						if (childnode->touched() == false) {
							// this node was not touched and will be removed, our hittest data is no longer valid
							clearHitTest();						
						} else if (mMouseHitTest.mLineNo != 0) {
							if (childnode->lineNo()!=mMouseHitTest.mLineNo) {
								clearHitTest();														
							};
						};
					};
					
					// remove untouched children
					mRootNode.removeUntouched();
					
					// sort our nodes
					mRootNode.sortChildren();
					
					// make sure our current row is current again, we need this later!
					mOmnisList->setCurRow(currentRow);
				} else {
					if ((mMouseHitTest.mAbove==oDL_node) || (mMouseHitTest.mAbove==oDL_row)) {
						// we're about to remove all nodes so this can't be valid anymore
						clearHitTest();						
					};

					mRootNode.clearChildNodes();
				};
				
				mRebuildNodes = false;
			};
						
			// Now draw our stuff...		
			qdim	top				= 0;
			qlong	listLineNo		= 0;	// this will count the total of lines we have in our list including our grouping lines, excluding any collapsed lines
			bool	isEven			= false;
			mLastVisListNo			= 0;
			top = drawNode(pECI, mRootNode, -1, top, listLineNo, isEven);
			
			// add one extra line for spacing...
			listLineNo++;
			
			// set our page size
			qlong	pageSize	= listLineNo > 4 ? 4 : listLineNo;
			
//			addToTraceLog("First vis: %li, Last vis: %li, Total lines: %li", mVertScrollPos+1, mLastVisListNo, listLineNo);
			
			// update our vertical scroll range which we now do in line numbers, this may trigger another redraw..
			if (mVertScrollPos > listLineNo - pageSize) {
				qdim newScrollPos		= listLineNo - pageSize;
				newScrollPos			= newScrollPos > 0 ? newScrollPos : 0;
				
				if (mVertScrollPos != newScrollPos) {
					mVertScrollPos = newScrollPos;
					WNDsetScrollPos(mHWnd, SB_VERT, mVertScrollPos, qtrue);				
					redraw = true;
				};
			};
			{
				qdim wasMin, wasMax, wasPage;
				WNDgetScrollRange(mHWnd, SB_VERT, &wasMin, &wasMax, &wasPage);
				if (wasMin != 0 || wasMax != listLineNo || wasPage != pageSize) {
					addToTraceLog("Changing vert scroll from %li, %li to %li, %li", wasMax, wasPage, listLineNo, pageSize);
					WNDsetScrollRange(mHWnd, SB_VERT, 0, listLineNo, pageSize, qtrue);
				}
			};
									
			// We are done with our list...
			mOmnisList->setCurRow(currentRow);
			delete mOmnisList;
			mOmnisList = 0;

			// If our current line has changed, check if it is on screen, this may trigger another redraw..
			if (currentRow!=0) {
				// !BAS! Need to redo this now that we use line numbers, we should find our node by list line number and scroll accordingly
				
				qdim	currentListLine = mRootNode.findListLineNo(currentRow);
				
				// addToTraceLog("Current list line: %li, was: %li", currentListLine, mLastCurrentLineTop);
				
				if (currentListLine==-1) {
					// line not visible at all? we can ignore this, there is no sensible position to scroll too.
					mLastCurrentLineTop = 0;
				} else if (mLastCurrentLineTop == currentListLine) {
					// still the same? then we do NOT adjust our positioning, the user may have scrolled down and wants to select another line. Lets not undo this!
				} else if ((currentListLine<=mVertScrollPos) || (currentListLine>mLastVisListNo)) { 
					// note, mVertScrollPos starts at 0 for showing list line 1.
					
					qdim	newScrollPos = currentListLine - ((mLastVisListNo-mVertScrollPos+1) / 2);
					newScrollPos = newScrollPos > listLineNo - pageSize ? listLineNo - pageSize : newScrollPos;
					newScrollPos = newScrollPos > 0 ? newScrollPos : 0;
					if (mVertScrollPos != newScrollPos) {
						mVertScrollPos = newScrollPos;
						WNDsetScrollPos(mHWnd, SB_VERT, mVertScrollPos, qtrue);
						redraw = true;
					};

					mLastCurrentLineTop = currentListLine;
				} else {
					// just copy it...
					mLastCurrentLineTop = currentListLine;
				};
			} else {
				// no current line? no reason to scroll!
				mLastCurrentLineTop = 0;
			};
		}; 
	};	

	// finally draw our divider lines and update our horizontal scroll range, this may trigger another redraw..	
	qdim	horzPageSize	= mClientRect.width() / 2;
	qdim	maxHorzScroll	= drawDividers(mClientRect.top, mClientRect.bottom);
	horzPageSize			= horzPageSize > 1 ? horzPageSize : 1;
	maxHorzScroll			= maxHorzScroll + 32;
	maxHorzScroll			= maxHorzScroll > 0 ? maxHorzScroll : 0;
	if (mHorzScrollPos > maxHorzScroll - horzPageSize) {
		qdim newOffsetX			= maxHorzScroll - horzPageSize;
		newOffsetX				= newOffsetX > 0 ? newOffsetX : 0;
		if (mHorzScrollPos != newOffsetX) {
			mHorzScrollPos = newOffsetX;
			WNDsetScrollPos(mHWnd, SB_HORZ, mHorzScrollPos, qtrue);				
			redraw = true;
		};
	}

	{
		qdim wasMin, wasMax, wasPage;
		WNDgetScrollRange(mHWnd, SB_HORZ, &wasMin, &wasMax, &wasPage);
		if (wasMin != 0 || wasMax != maxHorzScroll || wasPage != horzPageSize) {
			addToTraceLog("Changing horz scroll from %li, %li to %li, %li", wasMax, wasPage, maxHorzScroll, horzPageSize);
			WNDsetScrollRange(mHWnd, SB_HORZ, 0, maxHorzScroll, horzPageSize, qtrue);
		}
	};

	// addToTraceLog("finished drawing in %li ms",((clock()-t) * 1000)/CLOCKS_PER_SEC);
	
	if (redraw) {
		// just draw again, may need to protect against recursive calls? I think it'll be alright.
		doPaint(pECI);
	};

	mDrawRecurseCount--;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOproperty oDataListProperties[] = { 
	//	ID						ResID	Type			Flags					ExFlags	EnumStart	EnumEnd
	anumListName,				0,		fftCharacter,	EXTD_FLAG_FAR_SRCH
														+EXTD_FLAG_PROPGENERAL,	0,		0,			0,		// $listname
	anumHScroll,				0,		fftInteger,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $hscroll
	anumVScroll,				0,		fftInteger,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $vscroll
	anumHorzscroll,				0,		fftBoolean,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $horzscroll
	anumVertscroll,				0,		fftBoolean,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $vortscroll
	
	oDL_columncount,			4110,	fftInteger,		EXTD_FLAG_PROPDATA,		0,		0,			0,		// $columncount
	oDL_columncalcs,			4111,	fftCharacter,	EXTD_FLAG_PROPDATA
														+EXTD_FLAG_PWINDMLINE
														+EXTD_FLAG_FAR_SRCH,	0,		0,			0,		// $columncalcs
	oDL_columnwidths,			4112,	fftCharacter,	EXTD_FLAG_PROPDATA,		0,		0,			0,		// $columnwidths
	oDL_columnaligns,			4113,	fftCharacter,	EXTD_FLAG_PROPDATA,		0,		0,			0,		// $columnaligns
	oDL_maxrowheight,			4114,   fftInteger,     EXTD_FLAG_PROPAPP,		0,		0,			0,		// $maxrowheight
	oDL_columnprefix,			4115,	fftCharacter,	EXTD_FLAG_PROPDATA,		0,		0,			0,		// $columnprefix
	oDL_verticalExtend,			4116,	fftCharacter,	EXTD_FLAG_PROPDATA,		0,		0,			0,		// $verticalExtend
	oDL_evenColor,				4117,	fftInteger,		EXTD_FLAG_PROPAPP
														+EXTD_FLAG_PWINDCOL,	0,		0,			0,		// $evencolor
	oDL_selectColor,			4118,	fftInteger,		EXTD_FLAG_PROPAPP
														+ EXTD_FLAG_PWINDCOL,	0,		0,			0,		// $selectcolor

	oDL_groupcalcs,				4120,	fftCharacter,	EXTD_FLAG_PROPDATA
														+EXTD_FLAG_PWINDMLINE
														+EXTD_FLAG_FAR_SRCH,	0,		0,			0,		// $groupcalcs
	oDL_treeIndent,				4121,	fftInteger,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $treeIndent
	anumLineHtExtra,			0,		fftInteger,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $linespacing
	anumShowselected,			0,		fftBoolean,		EXTD_FLAG_PROPACT,		0,		0,			0,		// $multiselect
	oDL_deselNodeClick,			4123,	fftBoolean,		EXTD_FLAG_PROPACT,		0,		0,			0,		// $deselectOnGroupClick
	oDL_parentCalcs,			4124,	fftCharacter,	EXTD_FLAG_PROPDATA
														+EXTD_FLAG_PWINDMLINE
														+EXTD_FLAG_FAR_SRCH,	0,		0,			0,		// $parentCalcs

	oDL_filtercalc,				4122,	fftCharacter,	EXTD_FLAG_PROPDATA
														+EXTD_FLAG_FAR_SRCH,	0,		0,			0,		// $filtercalc
};	

qProperties * oDataList::properties(void) {
	qProperties *	lvProperties = oBaseVisComponent::properties();
	
	// Add the property definition for our visual component here...
	lvProperties->addElements(oDataListProperties, sizeof(oDataListProperties) / sizeof(ECOproperty));
	
	return lvProperties;
};

// set the value of a property
qbool oDataList::setProperty(qlong pPropID,EXTfldval &pNewValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves, no idea why...
	
	switch (pPropID) {
        case anumListName: {
            mListName = pNewValue;
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
        }; break;
		case oDL_columncount: {
			mColumnCount = pNewValue.getLong();
			if (mColumnCount < 1) mColumnCount = 1;
			
			ECOupdatePropInsp(mHWnd, oDL_columncalcs);
			ECOupdatePropInsp(mHWnd, oDL_columnwidths);
			ECOupdatePropInsp(mHWnd, oDL_columnaligns);
			
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_columncalcs: {
			qstring		newcalc(pNewValue);
			qstring *	calc = new qstring();
			
			// Change any newlines to \n for easy recognition
			newcalc.replace("\r\n", "\n");
			newcalc.replace("\r", "\n");
			
			// clear our column definitions
			clearColumnCalcs();
			
			for (qulong i = 0; i < newcalc.length(); i++) {
				qchar	digit = newcalc[i];
				
				if ((digit == '\t') || (digit == '\n')) { // we use to use tabs as delimiters, left that here for backwards compatibility
					mColumnCalculations.push(calc);
					calc = new qstring();
				} else {
					*calc += digit;
				};
			};
			
			mColumnCalculations.push(calc);
			
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_columnwidths: {
			qstring			widths(pNewValue);
			qdim			width = 0;
			
			mColumnWidths.clear();
			
			for (qulong i = 0; i<widths.length(); i++) {
				qchar	digit = widths[i];
				
				if (digit == ',') {
					mColumnWidths.push(width);
					width = 0;
				} else if ((digit >= '0') && (digit <= '9')) {
					width = 10 * width;
					width += digit - '0'; 
				};
			};

			mColumnWidths.push(width);
			
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_columnaligns: {
			qstring		aligns(pNewValue);
			
			mColumnAligns.clear();
			mColumnAligns.push(jstLeft); // our first column is ALWAYS left aligned!
			
			for (qulong i = 1; i<aligns.length(); i++) {
				qchar	digit = aligns[i];
				
				if ((digit == 'l') || (digit == 'L')) {
					mColumnAligns.push(jstLeft);
				} else if ((digit == 'r') || (digit == 'R')) {
					mColumnAligns.push(jstRight);
				} else if ((digit == 'c') || (digit == 'C')) {
					mColumnAligns.push(jstCenter);
				};
			};
			
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_maxrowheight: {
			mMaxRowHeight = pNewValue.getLong();
			if (mMaxRowHeight < 14) {
				mMaxRowHeight = 14;
			} else if (mMaxRowHeight > 200) {
				mMaxRowHeight = 200;
			};
						
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_columnprefix: {
			mColumnPrefix = pNewValue;

			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_verticalExtend: {
			qstring	bools(pNewValue);
			qlong	len = bools.length();
			
			for (long i = 0; i < 256 ; i++) {
				if (i>=len) {
					mColumnExtend[i] = true; // default is true
				} else if ((bools[i]=='t') || (bools[i]=='T')) {
					mColumnExtend[i] = true;
				} else {
					mColumnExtend[i] = false;
				};
			};

			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;			
		}; break;
		case oDL_evenColor: {
			mEvenColor = pNewValue.getLong();

			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_selectColor: {
			mSelectColor = pNewValue.getLong();

			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_groupcalcs: {
			qstring		newcalc(pNewValue);
			sDLGrouping	grouping;

			// prepare our grouping
			grouping.mGroupCalc		= new qstring();
			grouping.mParentCalc	= NULL;

			// Change any newlines to \n for easy recognition
			newcalc.replace("\r\n", "\n");
			newcalc.replace("\r", "\n");
			
			// clear our grouping array
			clearGroupCalcs();
			
			for (qulong i = 0; i < newcalc.length(); i++) {
				qchar	digit = newcalc[i];
				
				if ((digit == '\t') || (digit == '\n')) {
					mGroupCalculations.push_back(grouping);
					
					grouping.mGroupCalc = new qstring();
				} else {
					*grouping.mGroupCalc += digit;
				};
			};
			
			if (grouping.mGroupCalc->length()>0) {
				mGroupCalculations.push_back(grouping);
			} else {
				delete grouping.mGroupCalc;
			};
			
			mRebuildNodes = true;
			
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_treeIndent:{
			mIndent = pNewValue.getLong();
			
			// limit our value
			if (mIndent<16){
				mIndent = 16;
			} else if (mIndent > 100) {
				mIndent = 100;
			};
			
			WNDinvalidateRect(mHWnd, NULL);
			
			return qtrue;
		}; break;
		case anumLineHtExtra: {
			mLineSpacing = pNewValue.getLong();
			
			// limit our value
			if (mLineSpacing < 1) {
				mLineSpacing = 1;
			} else if (mLineSpacing > 100) {
				mLineSpacing = 100;
			};
			
			WNDinvalidateRect(mHWnd, NULL);

			return qtrue;
		}; break;
		case anumShowselected: {
			mShowSelected = pNewValue.getBool() == 2;
			return qtrue;
		}; break;
		case oDL_filtercalc: {
			mFilter = pNewValue;

			mRebuildNodes = true;
			mLastCurrentLineTop = 0;
			mVertScrollPos = 0;
            WNDsetScrollPos(mHWnd, SB_VERT, mVertScrollPos, qfalse); // no need to redraw, we're about to redraw the whole control...
			WNDinvalidateRect(mHWnd, NULL);
			
			return qtrue;
		}; break;
		case oDL_deselNodeClick: {
			mDeselectOnNodeClick = pNewValue.getBool() == 2;
			return qtrue;
		}; break;
		case oDL_parentCalcs: {
			/* !BAS! Our parent calculations share an array with our grouping calculations. We assume the grouping calculations are always set before our parent calculations and our array is thus defined */

			qstring		newcalc(pNewValue);
			qstring *	calc = new qstring();
			qulong		group;
			
			// Change any newlines to \n for easy recognition
			newcalc.replace("\r\n", "\n");
			newcalc.replace("\r", "\n");
			
			// Clear our parent calculations
			for (group = 0; group < mGroupCalculations.size(); group++) {
				if (mGroupCalculations[group].mParentCalc != NULL) {
					delete mGroupCalculations[group].mParentCalc;
					mGroupCalculations[group].mParentCalc = NULL;
				};
			};
			
			// now process our new string
			group = 0;
			for (qulong i = 0; i < newcalc.length(); i++) {
				qchar	digit = newcalc[i];
				
				if ((digit == '\t') || (digit == '\n')) {
					if ((calc->length()>0) && (group < mGroupCalculations.size())) {
						mGroupCalculations[group].mParentCalc = calc;
					} else {
						delete calc;
					};
					
					// next!
					group++;
					calc = new qstring();
				} else {
					*calc += digit;
				};
			};
			
			if ((calc->length()>0) && (group < mGroupCalculations.size())) {
				mGroupCalculations[group].mParentCalc = calc;
			} else {
				delete calc;
			};
			
			mRebuildNodes = true;
			
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		default:
			return oBaseVisComponent::setProperty(pPropID, pNewValue, pECI);
			break;
	};
};

// get the value of a property
qbool oDataList::getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves...
	
	switch (pPropID) {
        case anumListName: {
            pGetValue.setChar((qchar *)mListName.cString(), mListName.length());
			return true;
        }; break;
		case oDL_columncount: {
			pGetValue.setLong(mColumnCount);
			return true;
		}; break;
		case oDL_columncalcs: {
			qstring	columncalcs;
			
			for (qulong i = 0; i<mColumnCount; i++) {
				if (i!=0) {
					columncalcs += QTEXT("\r\n");
				};
				if (i<mColumnCalculations.numberOfElements()) {
					qstring *	calcstr = mColumnCalculations[i];
					columncalcs += *calcstr;
				};
			};
			
			pGetValue.setChar((qchar *)columncalcs.cString(), columncalcs.length());
			
            return true;
		}; break;
		case oDL_columnwidths: {
			qstring		widths = QTEXT("");;
			for (qulong i = 0; i<mColumnCount; i++) {
				if (i!=0) {
					widths += QTEXT(",");
				};

				if (i<mColumnWidths.numberOfElements()) {
					widths.appendFormattedString("%li",mColumnWidths[i]);
				} else {
					widths += QTEXT("100");
				};
			};
						
			pGetValue.setChar((qchar *)widths.cString(), widths.length());					
            return true;
		}; break;
		case oDL_columnaligns: {
			qstring		aligns = QTEXT("");
			for (qulong i = 0; i<mColumnCount; i++) {				
				if (i<mColumnWidths.numberOfElements()) {
					switch (mColumnAligns[i]) {
						case jstRight:
							aligns += QTEXT("R");
							break;
						case jstCenter:
							aligns += QTEXT("C");
							break;
						default:
							aligns += QTEXT("L");
							break;
					};
				} else {
					aligns += QTEXT("L");
				};
			};
			
			pGetValue.setChar((qchar *)aligns.cString(), aligns.length());					
            return true;
		}; break;
		case oDL_maxrowheight: {
			pGetValue.setLong(mMaxRowHeight);
            return true;
		}; break;
		case oDL_columnprefix: {
			pGetValue.setChar((qchar *) mColumnPrefix.cString(), mColumnPrefix.length());
            return true;
		}; break;
		case oDL_verticalExtend: {
			qstring bools;
			
			for (qulong i = 0; i < mColumnCount; i++ ) {
				if (mColumnExtend[i]) {
					bools += 'T';
				} else {
					bools += 'F';					
				};
			}

			pGetValue.setChar((qchar *) bools.cString(), bools.length());
            return true;
		}; break;
		case oDL_evenColor: {
			pGetValue.setLong(mEvenColor);
            return true;
		}; break;
		case oDL_selectColor: {
			pGetValue.setLong(mSelectColor);
			return true;
		}; break;
		case oDL_groupcalcs: {
			qstring	groupcalcs;
			
			for (qulong i = 0; i < mGroupCalculations.size(); i++) {
				qstring * calc = mGroupCalculations[i].mGroupCalc;
				if (i!=0) {
					groupcalcs += QTEXT("\r\n");
				};
					
				if (calc != NULL) {
					groupcalcs += *calc;
				};
			};
			
			pGetValue.setChar((qchar *)groupcalcs.cString(), groupcalcs.length());
            return true;
		}; break;
		case oDL_treeIndent:{
			pGetValue.setLong(mIndent);
            return true;
		}; break;
		case anumLineHtExtra: {
			pGetValue.setLong(mLineSpacing);
            return true;
		}; break;
		case anumShowselected: {
			pGetValue.setBool(mShowSelected ? 2 : 1);
            return true;
		}; break;
		case oDL_filtercalc: {
			pGetValue.setChar((qchar *)mFilter.cString(), mFilter.length());
            return true;
		}; break;
		case oDL_deselNodeClick: {
			pGetValue.setBool(mDeselectOnNodeClick ? 2 : 1);
            return true;
		}; break;
		case oDL_parentCalcs: {
			qstring	parentcalcs;
			bool	hascalcs = false;
			
			for (qulong i = 0; i < mGroupCalculations.size(); i++) {
				qstring * calc = mGroupCalculations[i].mParentCalc;
				if (i!=0) {
					parentcalcs += QTEXT("\r\n");
				};
					
				if (calc != NULL) {
					parentcalcs += *calc;
					hascalcs = true;
				};
			};
			
			// if we have no calculations, clear it, we don't even want our delimiters
			if (!hascalcs) {
				parentcalcs = "";
			};
			
			pGetValue.setChar((qchar *)parentcalcs.cString(), parentcalcs.length());
            return true;
		}; break;
		default:
			return oBaseVisComponent::getProperty(pPropID, pGetValue, pECI);
			
			break;
	};
};	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// $dataname list
//
// We no longer keep a copy of our list data within our component but purely access it by reference.
// This prevents a whole lot of copying of data but does require we redraw the display at set times
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ...

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOparam oDataListIsVisibleParam[] = {
	//	Resource	Type		Flags		ExFlags
	7001,			fftInteger,	0,			0,		// pLineNo
};

ECOparam oDataListLineAtPosParam[] = {
	//	Resource	Type		Flags		ExFlags
	7003,			fftInteger,	0,			0,		// pY
};

// This is our array of methods we support
ECOmethodEvent oDataListMethods[] = {
//	ID				Resource	Return type		Paramcount		Params						Flags		ExFlags
	1,				8010,		fftBoolean,		1,				oDataListIsVisibleParam,	0,			0,			// $isVisible
	2,				8011,		fftInteger,		1,				oDataListLineAtPosParam,	0,			0,			// $lineAtPos
	3,				8012,		fftInteger,		0,				0,							0,			0,			// $lineAtMouse
};

// return an array of method meta data
qMethods * oDataList::methods(void) {
	qMethods * lvMethods = oBaseVisComponent::methods();
	
	lvMethods->addElements(oDataListMethods, sizeof(oDataListMethods) / sizeof(ECOmethodEvent));		
	
	return lvMethods;
};

// invoke a method
int	oDataList::invokeMethod(qlong pMethodId, EXTCompInfo* pECI) {
	switch (pMethodId) {
		case 1: {
			EXTfldval	lvResult;
			qlong		lvLineNo = getLongFromParam(1, pECI);
			qdim		lvAt = mRootNode.findTopForLine(lvLineNo);
			
			if (lvAt==-1) {
				// no top position? = not visible
				lvResult.setBool(1);
			} else {
				// visible!
				lvResult.setBool(2);				
			};
	 
			ECOaddParam(pECI, &lvResult);
			return 1L;							
		}; break;
		case 2: {
			EXTfldval	lvResult;
			qlong		lvY = getLongFromParam(1, pECI);

			qpoint at(40, lvY);
			sDLHitTest hittest = doHitTest(at);

			if ((hittest.mAbove == oDL_row) || (hittest.mAbove == oDL_node)) {
				lvResult.setLong(hittest.mLineNo);
			} else {
				lvResult.setLong(0);
			};

					  
			ECOaddParam(pECI, &lvResult);
			return 1L;							
		}; break;
		case 3: {
			EXTfldval	lvResult;
			qpoint pt;
			
			// get our mouse position and map it to our control so 0,0 is the left-top of our control
			WNDgetCursorPos(&pt);
			WNDmapWindowPoint(HWND_DESKTOP, mHWnd, &pt);
			
			// should test if pt is above our field...

			sDLHitTest hittest = doHitTest(pt);
			
			if ((hittest.mAbove == oDL_row) || (hittest.mAbove == oDL_node)) {
				lvResult.setLong(hittest.mLineNo);
			} else {
				lvResult.setLong(0);
			};
			
			ECOaddParam(pECI, &lvResult);
			return 1L;							
		}; break;
		default: {
			return oBaseVisComponent::invokeMethod(pMethodId, pECI);
		}; break;
	};
	return 1L;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOparam oDataListevClickParam[] = {
	//	Resource	Type		Flags		ExFlags
	7001,			fftInteger,	0,			0,		// pLineNo
};

ECOmethodEvent oDataListEvents[] = {
	//	ID					Resource	Return type		Paramcount		Params					Flags		ExFlags
	oDL_evClick,			5000,		0,				1,				oDataListevClickParam,	0,			0,
	oDL_evHScrolled,		5001,		0,				0,				0,						0,			0,
	oDL_evVScrolled,		5002,		0,				0,				0,						0,			0,
	oDL_evColumnResized,	5003,		0,				0,				0,						0,			0,
	oDL_evDoubleClick,		5004,		0,				0,				0,						0,			0,
};	

// return an array of events meta data
qEvents *	oDataList::events(void) {
	qEvents *	lvEvents = oBaseNVComponent::events();
	
	// Add the event definition for our visual component here...
	lvEvents->addElements(oDataListEvents, sizeof(oDataListEvents) / sizeof(ECOmethodEvent));
	
	return lvEvents;
};
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mouse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// get our vertical step size
qdim oDataList::getVertStepSize(void) {
	return 4;
};

// window was scrolled
void oDataList::evWindowScrolled(qdim pNewX, qdim pNewY) {
	if (!mInWindowScrolled) {
		mInWindowScrolled = true;
		qdim pWasX = mHorzScrollPos;
		qdim pWasY = mVertScrollPos;

		// call base class
		oBaseVisComponent::evWindowScrolled(pNewX, pNewY);

		// According to Omnis tech support instantiating this object will prevent escape testing and solve a recursive issue leading to a crash...
		ECOdisableTestesc disable_test();

		// This should become part of our base class if we can use Omnis' internal values
		if (mHorzScrollPos != pWasX) {
			ECOsendEvent(mHWnd, oDL_evHScrolled, 0, 0, EEN_EXEC_LATER);
		};
		if (mVertScrollPos != pWasY) {
			ECOsendEvent(mHWnd, oDL_evVScrolled, 0, 0, EEN_EXEC_LATER);
		};

		mInWindowScrolled = false;
	} else {
		addToTraceLog("evWindowScrolled recurse call");
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// mouse
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// clear our hitttest info
void	oDataList::clearHitTest(void) {
	// reset this... 
	mMouseHitTest.mAbove	= oDL_none;
	mMouseHitTest.mColNo	= 0;
	mMouseHitTest.mNode		= NULL;
	mMouseHitTest.mLineNo	= 0;	
};

// find what we are above
sDLHitTest	oDataList::doHitTest(qpoint pAt) {
	sDLHitTest	above;
	
	// adjust our point by our horizontal scroll position
	pAt.h += mHorzScrollPos;
	
	// first check if we're above one of our vertical dividers
	qdim	left = 0;
	for (unsigned int i = 0; i < mColumnCount; i++) {
		// as we've drawn our display this should be trustworthy..
		if (i < mColumnWidths.numberOfElements()) {
			left += mColumnWidths[i];
			
			if ((left - 1 <= pAt.h) && (left + 1 >= pAt.h)) {
				above.mAbove	= oDL_horzSplitter;
				above.mColNo	= i;
				above.mNode		= NULL;
				above.mLineNo	= 0;
				
				return above;
			};
		};
	};
		
	// Now check if we're above a node
	above.mColNo	= 0;
	above.mNode		= mRootNode.findChildByPoint(pAt);
	if (above.mNode != NULL) {
		if (above.mNode->aboveTreeIcon(pAt)) {
			above.mAbove	= oDL_treeIcon;
			above.mLineNo	= 0;
		} else {
			above.mLineNo	= above.mNode->lineNo();
			above.mAbove	= above.mLineNo==0 ? oDL_node : oDL_row;
		};
		return above;
	};
	
	// nothing...
	above.mAbove	= oDL_none;
	above.mNode		= NULL;
	above.mLineNo	= 0;
	return above;
};

// return the mouse cursor we should show
HCURSOR	oDataList::getCursor(qpoint pAt, qword2 pHitTest) {
	if (pHitTest==HTCLIENT) {
		sDLHitTest above = this->doHitTest(pAt);
		switch (above.mAbove) {
			case oDL_horzSplitter:
				return WND_CURS_SPLITTER_HORZ;
			case oDL_treeIcon:
				return WND_CURS_ARROW;
			default:
				// nothing
				break;
		};
	};
	
	// return our default
	return oBaseVisComponent::getCursor(pAt, pHitTest);		
};

// mouse left button pressed down (return true if we finished handling this, false if we want Omnis internal logic)
bool	oDataList::evMouseLDown(qpoint pDownAt) {
	mMouseHitTest = this->doHitTest(pDownAt); // where did our mouse come down?
	mMouseLast = pDownAt; // remember this to calculate deltas
	
	return true;
};

// mouse left button released (return true if we finished handling this, false if we want Omnis internal logic)
bool	oDataList::evMouseLUp(qpoint pDownAt) {
	clearHitTest();

	return true;
};

// mouse moved to this location while mouse button is not down
void	oDataList::evMouseMoved(qpoint pMovedTo) {
	if (mMouseHitTest.mAbove == oDL_horzSplitter) {
		qdim	deltaX = pMovedTo.h - mMouseLast.h;
		if (deltaX != 0) {
			qdim newWidth = mColumnWidths[mMouseHitTest.mColNo] + deltaX;
			if (newWidth<10) {
				newWidth = 10;
			};
			
			mColumnWidths.setElementAtIndex(mMouseHitTest.mColNo, newWidth);
			WNDinvalidateRect(mHWnd, NULL);
			
			ECOsendEvent(mHWnd, oDL_evColumnResized, 0, 0, EEN_EXEC_LATER);
		};
		mMouseLast = pMovedTo;
	};
};

// mouse click at this location
void	oDataList::evClick(qpoint pAt, EXTCompInfo* pECI) {
	/*
	 Even if our field is disabled we still get these events. 
    */
	if (isActive() && ECOisOMNISinTrueRuntime(mHWnd)) {
		mMouseHitTest = this->doHitTest(pAt); // redo our hittest just in case
		
		// we must also update our list pointed to by $dataname. Note that if we're dealing with an item reference we'll be updating the same list twice..
		mOmnisList = getNamedList(mListName, pECI);
		
		switch (mMouseHitTest.mAbove) {
			case oDL_none:
				//			addToTraceLog("Nothing at %li, %li",pAt.h,pAt.v);
				break;
			case oDL_horzSplitter:
				break;
			case oDL_treeIcon: {
				// toggle our node
				bool isExpanded = mMouseHitTest.mNode->expanded();
				mMouseHitTest.mNode->setExpanded(isExpanded==false);
				WNDinvalidateRect(mHWnd, NULL);
				
				// maybe send a click event back to Omnis?
			}; break;
			case oDL_node: {
                if (isEnabled()) {
                    if ((mOmnisList != NULL) && mDeselectOnNodeClick) {
                        if (mShowSelected) {
                            // deselect all
                            qlong	rowCnt = mOmnisList->rowCnt();
						
                            for (qlong row = 1; row<=rowCnt; row++) {
                                mOmnisList->selectRow(row, qfalse, qfalse);
                            };
                        } else {
                            // deselect the current row
                            qlong	row = mOmnisList->getCurRow();
                            if (row != 0) {
                                mOmnisList->selectRow(row, qfalse, qfalse);
                            };
                        };
					
                        mOmnisList->setCurRow(0);
					
                        // and redraw
                        WNDinvalidateRect(mHWnd, NULL);
                    };
				
                    // let user know we clicked outside of our line
                    EXTfldval	evParam[1];
                    evParam[0].setLong(0);
                    ECOsendEvent(mHWnd, oDL_evClick, evParam, 1, EEN_EXEC_LATER);
                };
			}; break;
			case oDL_row: {
                if (isEnabled()) {
                    //			addToTraceLog("Clicked on row %li at %li, %li",mMouseHitTest.mLineNo, pAt.h,pAt.v);
				
                    if (mOmnisList != NULL) {
                        qlong	currentRow = mOmnisList->getCurRow();
                        qlong	rowCnt = mOmnisList->rowCnt();
					
                        if ((isShift() == 1) && (currentRow!=0) && (mShowSelected)) {
                            // select from the current line to the line we clicked on, then make that line current
                            qlong	isSelected = mOmnisList->isRowSelected(currentRow, qfalse);
						
                            while (currentRow!=mMouseHitTest.mLineNo) {
                                if (currentRow > mMouseHitTest.mLineNo) {
                                    currentRow--;
                                } else {
                                    currentRow++;
                                };
							
                                qdim		lvAt = mRootNode.findTopForLine(currentRow);
			
                                if (lvAt==-1) {
                                    // no top position? = not visible, don't select it
                                    mOmnisList->selectRow(currentRow, qfalse, qfalse);
                                } else {
                                    mOmnisList->selectRow(currentRow, isSelected, qfalse);
                                };
                            };
						
                            // and make this our new current row
                            mOmnisList->setCurRow(mMouseHitTest.mLineNo);
                        } else {
                            if ((isControl() != 0) && (mShowSelected)) { /* note, control on windows, cmnd on mac */
                                // toggle the line we clicked on
                                qlong	isSelected = mOmnisList->isRowSelected(mMouseHitTest.mLineNo, qfalse);
                                mOmnisList->selectRow(mMouseHitTest.mLineNo, !isSelected, qfalse);
                            } else {
                                // deselect all lines...
                                for (qlong row = 1; row<=rowCnt; row++) {
                                    mOmnisList->selectRow(row, qfalse, qfalse);
                                };
							
                                // select the line we clicked on and make it current
                                mOmnisList->selectRow(mMouseHitTest.mLineNo, qtrue, qfalse);
                            };
						
                            // do make it the current row
                            mOmnisList->setCurRow(mMouseHitTest.mLineNo);
                        };
                    };
				
                    // and redraw
                    WNDinvalidateRect(mHWnd, NULL);
				
                    // let user know we clicked on a line
                    EXTfldval	evParam[1];
                    evParam[0].setLong(mMouseHitTest.mLineNo);
                    ECOsendEvent(mHWnd, oDL_evClick, evParam, 1, EEN_EXEC_LATER);
                };
			};	break;
			default:
				break;
		};
		
		if (mOmnisList != NULL) {
			delete mOmnisList;
			mOmnisList = 0;
		};		
	};
};

// mouse left button double clicked (return true if we finished handling this, false if we want Omnis internal logic)
bool	oDataList::evDoubleClick(qpoint pAt, EXTCompInfo* pECI) {
	bool enabled = (isEnabled() && isActive() && ECOisOMNISinTrueRuntime(mHWnd));
	if (enabled) {
		mMouseHitTest = this->doHitTest(pAt); // redo our hittest just in case
		
		switch (mMouseHitTest.mAbove) {
			case oDL_node: {		
				// double click on our icon we ignore, but double click on our node we treat like we clicked on our icon
				
				// toggle our node
				bool isExpanded = mMouseHitTest.mNode->expanded();
				mMouseHitTest.mNode->setExpanded(isExpanded==false);
				WNDinvalidateRect(mHWnd, NULL);
				
				// maybe send a click event back to Omnis?
			};	break;
			case oDL_row: {
				ECOsendEvent(mHWnd, oDL_evDoubleClick, 0, 0, EEN_EXEC_LATER);				
			};	break;
			default:
				break;
		};
	};
	
	return false; // let omnis do its own thing...
};


// mouse right button pressed down (return true if we finished handling this, false if we want Omnis internal logic)
bool	oDataList::evMouseRDown(qpoint pDownAt, EXTCompInfo* pECI) {
	bool enabled = (isEnabled() && isActive() && ECOisOMNISinTrueRuntime(mHWnd));
	if (enabled) {
		// make sure the line we're over is selected!
		mMouseHitTest = this->doHitTest(pDownAt); // redo our hittest just in case
		
		// we must also update our list pointed to by $dataname. Note that if we're dealing with an item reference we'll be updating the same list twice..
		mOmnisList = getNamedList(mListName, pECI);
		
		qlong	currentLine = mOmnisList->getCurRow();
		qlong	rowCnt = mOmnisList->rowCnt();
		qlong	newCurrentLine = 0;
		bool	selectionChanged = false;
		
		if (mMouseHitTest.mAbove == oDL_row) {
			newCurrentLine = mMouseHitTest.mLineNo;
		};
		
		if (newCurrentLine > 0 && mOmnisList->isRowSelected(newCurrentLine, qfalse)) {
			// The line we clicked on is marked as selected? Then we keep the selection as is.
		} else {
			for (qlong row = 1; row<=rowCnt; row++) {
				if (row == newCurrentLine) {
					if (!mOmnisList->isRowSelected(row, qfalse)) {
						selectionChanged = true;
						mOmnisList->selectRow(row, qtrue, qfalse);
					};
				} else if (mOmnisList->isRowSelected(row, qfalse)) {
					selectionChanged = true;
					mOmnisList->selectRow(row, qfalse, qfalse);
				};
			};		
		}
		
		// We do make sure regardless that the line we clicked on is the current line
		if (newCurrentLine != currentLine) {
			selectionChanged = true;
			mOmnisList->setCurRow(newCurrentLine);
		}		
		
		if (mOmnisList != NULL) {
			delete mOmnisList;
			mOmnisList = 0;
		};
		
		if (selectionChanged) {
			// Redraw
			WNDinvalidateRect(mHWnd, NULL);
			
			// let user know we changed our selection by simulating a click on a line
			EXTfldval	evParam[1];
			evParam[0].setLong(newCurrentLine);
			ECOsendEvent(mHWnd, oDL_evClick, evParam, 1, EEN_EXEC_LATER);				
		};
	};

	// we still want omnis' internal logic to handle our context menu!
	return false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// keyboard
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// let us know a key was pressed. Return true if Omnis should not do anything with this keypress
bool	oDataList::evKeyPressed(qkey *pKey, bool pDown, EXTCompInfo* pECI) {
	bool enabled = (isEnabled() && isActive() && ECOisOMNISinTrueRuntime(mHWnd));
	if (enabled && ECOhasFocus(mHWnd)) {
		pchar	testchar = pKey->getPChar();
		
		if ((testchar=='a' || testchar=='A') && (pKey->isControl()) && (!pKey->isShift()) && (!pKey->isAlt()) && mShowSelected && pDown) {
			// we must make selection changes to both our internal and the external list!
            mOmnisList = getNamedList(mListName, pECI);

			
			if (mOmnisList != NULL) {
				qlong	rowCnt = mOmnisList->rowCnt();
				
				for (qlong row = 1; row<=rowCnt; row++) {
					mOmnisList->selectRow(row, qtrue, qfalse);
				};				
				
                // and select our first line
                mOmnisList->setCurRow(1);

				delete mOmnisList;
				mOmnisList = 0;
			};
			
			// and redraw
			WNDinvalidateRect(mHWnd, NULL);
            
            // and finally treat this as a click
			EXTfldval	evParam[1];
			evParam[0].setLong(1);
			ECOsendEvent(mHWnd, oDL_evClick, evParam, 1, EEN_EXEC_LATER);
			
			return true;
		} else {
			return false;
		};
	} else {
		return false;
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// drag and drop
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Can we drag from this location? Return false if we can't
bool	oDataList::canDrag(qpoint pFrom) {
	bool enabled = (isEnabled() && isActive() && ECOisOMNISinTrueRuntime(mHWnd));
	
	if (enabled) {
		switch (mMouseHitTest.mAbove) {
			case oDL_none:
				return false;
				break;
			case oDL_horzSplitter:
				return false;
				break;
			case oDL_treeIcon:
				return false;
				break;
			case oDL_node:
				return false;
				break;
			case oDL_row:
				// !BAS! Shouldn't return true until we've moved more then a couple of pixels sideways..
				return true;
				break;
			default:
				return false;
				break;
		};
	} else {
		return false;
	};
};

// Set drag value, update the pDragInfo structure with information about what we are dragging, return -1 if we leave it up to Omnis to handle this
qlong	oDataList::evSetDragValue(FLDdragDrop *pDragInfo, EXTCompInfo* pECI) {
	// !BAS! see if we can use a bitmap of the first line we're dragging instead of a shape... but for now we keep a shape..
	// also need to see what we can do about showing some text...

	mOmnisList = getNamedList(mListName, pECI);
    if (mOmnisList != NULL) {
        qdim width		= 0;
        qdim height		= 32;
        for (unsigned int i = 0; i < mColumnWidths.numberOfElements(); i++) {
            width += mColumnWidths[i];
        };
        if (width < 100) width = 100;

        /*
        qdim left	= pDragInfo->mStartPoint.h - (width/2);
        qdim top	= pDragInfo->mStartPoint.v - (height/2);
	
        // this crashes, there is a fix in 10.2 coming
        qrgn *region = GDIallocQRGN();
        GDIsetRectRgn(region, left, top, left+width, top+32);

        // free any shape Omnis may have set, we're replacing it with our own
        if (pDragInfo->mDragShape!=0) {
            delete pDragInfo->mDragShape;
            pDragInfo->mDragShape = 0;
        };

        pDragInfo->mDragShape = region;
        pDragInfo->mAllowsBitmapDragging = 0;

        */

		HDC tmpDC;
		GDIcreateAlphaDC(&tmpDC, width, height);
		qrect r(0, 0, width - 1, height - 1);

		// Draw the alpha bitmap - for now we do a simple shape, we should really draw our row in here.
		HPEN pen = GDIcreatePen(1, 0x8800FF00, patFill);
		HPEN oldPen = GDIselectObject(tmpDC, pen);
		GDIframeRect(tmpDC, &r);
		GDIselectObject(tmpDC, oldPen);
		GDIdeleteObject(pen);

		pDragInfo->mBitmapBounds = r;
		GDIdeleteAlphaDC(tmpDC, &pDragInfo->mDragBitmap);      // This deletes HDC and creates an alpha bitmap containing what was drawn into HDC
		pDragInfo->mDragBitmapMask =  (HBITMAPMASK)0x02; // In 10.2 (HBITMAPMASK)0x02 will be a constant HBITMAP_ISALPHA
		pDragInfo->mAllowsBitmapDragging = qtrue;
	
        EXTfldval	dragType(pDragInfo->mDragType);
        dragType.setLong(cFLDdragDrop_dragData);
	
        EXTfldval dragValue(pDragInfo->mDragValue);
        dragValue.setList(mOmnisList, qfalse, qfalse);
        
		delete mOmnisList;
        mOmnisList = 0;
	};
	return qtrue;
};

// Ended dragging, return -1 if we leave it up to Omnis to handle this
qlong	oDataList::evEndDrag(FLDdragDrop *pDragInfo) {
	// Let Omnis free this up, it requires the info to clean it up....
//	if (pDragInfo->mDragShape!=0) {
//		delete pDragInfo->mDragShape;
//		pDragInfo->mDragShape = 0;
//	};
	
	// rest Omnis can do...
	return -1; 
};


