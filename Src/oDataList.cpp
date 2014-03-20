/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oDataList.cpp
 *  Implementation of our datalist object
 *
 *  Bastiaan Olij
 */


#include "oDataList.h"

oDataList::oDataList(void) {
	mColumnCount = 1;
	mShowSelected = false;
};

oDataList::~oDataList(void) {
	// clean up!

	clearGroupCalcs();
	clearColumnCalcs();
};

// Clear our group calculations
void	oDataList::clearGroupCalcs(void) {
	while (mGroupCalculations.numberOfElements()>0) {
		qstring *calc = mGroupCalculations.pop();
		delete calc;
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

// Draw this node
qdim	oDataList::drawNode(EXTCompInfo* pECI, oDLNode &pNode, EXTqlist* pList, qdim pIndent, qdim pTop) {
	qdim	headerHeight = 0;
	
	if (pIndent == -1) {
		// our root node, ignore this...
		pIndent = 0;
		
		addToTraceLog("Drawing root node");
	} else {
		qdim	iconwidth = 32;

		if (pNode.lineNo()!=0) {
			// draw as a full line
			headerHeight = drawRow(pECI, pNode.lineNo(), pList, pIndent+iconwidth, pTop);
			headerHeight -= pTop;
		} else {
			// Draw our description
			qrect	columnRect;

			columnRect.left		= pIndent - mOffsetX + iconwidth + 1;
			columnRect.top		= pTop;
			columnRect.right	= mColumnWidths[0] - mOffsetX - 1;
			columnRect.bottom	= pTop + 150;
			
			headerHeight = drawText(pNode.description(), columnRect, mTextColor, jstLeft, true, true);
			headerHeight -= pTop;
		};
		
		// now draw expanded/collapsed icon
		
		
		pIndent += iconwidth;
	};
	
	if (pNode.expanded()) {
		qlong i;
		pTop += headerHeight;
		
		// draw child nodes
		for (i = 0; i<pNode.childNodeCount(); i++) {
			oDLNode *child = pNode.getChildByIndex(i);
			pTop = drawNode(pECI, *child, pList, pIndent, pTop);
		};
		
		// draw line nodes
		for (i = 0; i<pNode.lineNodeCount(); i++) {
			pTop = drawRow(pECI, pNode.getLineNoAtIndex(i), pList, pIndent, pTop);
		};
		
		// draw totals?
	} else {
		// see if we need to draw totals...

		
		// now we can add our headerheight...
		pTop += headerHeight;
	};
	
	return pTop;
};

// Draw this line
qdim	oDataList::drawRow(EXTCompInfo* pECI, qlong pLineNo, EXTqlist* pList, qdim pIndent, qdim pTop) {
	qdim				left			= 0;
	qdim				lineheight		= 14; // minimum line height...
	qlong				oldCurRow		= pList->getCurRow();
	bool				isSelected		= ((mShowSelected && pList->isRowSelected(pLineNo)) || (!mShowSelected && (pLineNo == oldCurRow)));
	qArray<qstring *>	columndata;
	
	pList->setCurRow(pLineNo);

	// 1) find the line height of each text to find the highest line
	for (qlong i = 0; i < mColumnCount; i++) {
		qstring *		calcstr = mColumnCalculations[i];
		qstring *		newdata;
		
		if (calcstr->length()==0) {
			EXTfldval colFld;
			
			// just get the column...
			pList->getColValRef(pLineNo, i+1, colFld, qfalse);
			newdata = new qstring(colFld);
		} else {
			// should fine a relyable way to cache our calculations....
			// also should move this code into something more central
			qlong		error1,error2;
			EXTfldval	calcFld;
			
			qret calcret = calcFld.setCalculation(pECI->mLocLocp, ctyCalculation, (qchar *)calcstr->cString(), calcstr->length(), &error1, &error2);
			if (calcret != 0) {
				// error1 => error2 will be the substring of the part of the calculation that is wrong. 

				char errorstr[255];
				strcpy(errorstr, calcstr->c_str());
				errorstr[error2+1]=0x00;
				addToTraceLog("Error in calculation : %s",&errorstr[error1]);
				
				newdata = new qstring();
				*newdata += QTEXT("???");
			} else {
				EXTfldval	result;
				calcFld.evalCalculation(result, pECI->mLocLocp, pList, qtrue);
				newdata = new qstring(result);
			};			
		};
				
		columndata.push(newdata);

		qdim width = mColumnWidths[i] - 2 - (i==0 ? pIndent : 0);
		qdim columnHeight = getTextHeight(newdata->cString(), width > 10 ? width : 10, true, true);
		if (columnHeight>lineheight) {				
			lineheight = columnHeight;
		};
	};
	
	if (isSelected) {		
		// 2) draw our blue background 

		// 3) redraw our divider lines as we'll have drawn over those
		
	};
	
	// 4) draw our text
	left = -mOffsetX;
	for (i = 0; i < mColumnCount; i++) {
		qstring *	data = columndata[i];
		qrect		columnRect;
		
		if (i==0) {
			columnRect.left	= left + pIndent + 1;
		} else {
			columnRect.left	= left + 1;
		}
		columnRect.right	= left + mColumnWidths[i] - 1;
		columnRect.top		= pTop;
		columnRect.bottom	= pTop + lineheight;
		
		drawText(data->cString(), columnRect, mTextColor, mColumnAligns[i], true, true);

		left += mColumnWidths[i];
	};

	// cleanup...
	while(columndata.numberOfElements()>0) {
		qstring * olddata = columndata.pop();
		delete olddata;
	};
	
	pList->setCurRow(oldCurRow);
	return pTop + lineheight;
};


// Do our drawing in here
void oDataList::doPaint(EXTCompInfo* pECI) {
	// need to get this from our scrollbars, also it would be better to set the offset through the SDK but we need to make sure that our clipping behaves accordingly..
	
	// call base class to draw background
	oBaseVisComponent::doPaint(pECI);
	
	// draw our divider lines
	qdim	left = -mOffsetX;
	for (qlong i = 0; i<mColumnCount; i++) {
		qdim	width = 100;
		
		if (i<mColumnWidths.numberOfElements()) {
			width = mColumnWidths[i];
			if (width<10) {
				// column to small? change it and update it so we can trust it later
				width = 10;
				mColumnWidths.setElementAtIndex(i, width);
			};
		} else {
			// missing a width? add it now so we can trust it later...
			mColumnWidths.push(width);
		};
		
		if (i>=mColumnAligns.numberOfElements()) {
			// make sure we've got all our aligns as well so we can trust it later..
			mColumnAligns.push(jstLeft);
		};

		if (i>=mColumnCalculations.numberOfElements()) {
			// make sure we've got all our column calculations as well so we can trust it later..
			mColumnCalculations.push(new qstring(QTEXT("")));
		};
		
		left += width;
		drawLine(qpoint(left,mClientRect.top), qpoint(left,mClientRect.bottom), 1, GDI_COLOR_QGRAY, patDash); // should make the color and linestyle configurable
	};
	qdim	maxScroll = left + mOffsetX - mClientRect.width() + 32;
	WNDsetScrollRange(mHWnd, SB_HORZ, 0, maxScroll > 0 ? maxScroll : 0, 1, qtrue); // update our horizontal scroll range, this may trigger another redraw..
	
	if ( ECOisDesign(mHWnd) ) {
		// Don't draw anything else..
	} else {
		// we should get our list..
		EXTfldval	listFld;
		str255		listName;
		
		ECOgetProperty(mHWnd, anumFieldname, listFld);
		listFld.getChar(listName);		
		EXTfldval	dataField(listName, qfalse, pECI->mLocLocp);
		EXTqlist*	dataList = dataField.getList(qfalse);
		if (dataList==0) {
			return;
		} else {
			qlong		rowCount = dataList->rowCnt();
			qlong		oldCurRow = dataList->getCurRow();

			// Update our nodes
			mRootNode.unTouchChildren(); // untouch children
			
			if (rowCount!=0) {
				// loop through our list
				
				for (qlong lineno = 1; lineno <= rowCount; lineno++) {
					oDLNode *node = &mRootNode;
					dataList->setCurRow(lineno);
						
					// need to parse grouping to find child node...
					for (int group = 0; group < mGroupCalculations.numberOfElements(); group++) {
						// should fine a relyable way to cache our calculations....
						// also should move this code into something more central
						qlong		error1,error2;
						EXTfldval	calcFld;
						qstring *	calcstr = mGroupCalculations[group];
						
						qret calcret = calcFld.setCalculation(pECI->mLocLocp, ctyCalculation, (qchar *)calcstr->cString(), calcstr->length(), &error1, &error2);
						if (calcret != 0) {
							// error1 => error2 will be the substring of the part of the calculation that is wrong. 
							
							char errorstr[255];
							strcpy(errorstr, calcstr->c_str());
							errorstr[error2+1]=0x00;
							addToTraceLog("Error in calculation : %s",&errorstr[error1]);
						} else {
							EXTfldval	result;
							calcFld.evalCalculation(result, pECI->mLocLocp, dataList, qtrue);
							qstring		groupdesc(result);
														
							oDLNode *childnode = node->findChildByDescription(groupdesc);
							if (childnode == NULL) {
								addToTraceLog("New grouping for line %li: %s", lineno, groupdesc.c_str());

								childnode = new oDLNode(groupdesc, 0);
								node->addNode(childnode);
							} else {
								childnode->setTouched(true);
							};
							
							node = childnode;
						};
					};
						
					// now add this line to our node
					node->addLineNo(lineno);
				};					

				// remove untouched children
				mRootNode.removeUntouched();		

				// make sure our current row is current again, we need this later!
				dataList->setCurRow(oldCurRow);
			} else {
				mRootNode.clearChildNodes();
			};
						
			// Now draw our stuff...		
			qdim top	= -mOffsetY;
			top = drawNode(pECI, mRootNode, dataList, -1, top);
			
			// and we are done 
			dataList->setCurRow(oldCurRow);
			delete dataList; // we're done with it!

		};		
	};	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOproperty oDataListProperties[] = { 
	//	ID						ResID	Type			Flags					ExFlags	EnumStart	EnumEnd
	anumFieldname,				0,		fftList,		EXTD_FLAG_PRIMEDATA+EXTD_FLAG_FAR_SRCH+EXTD_FLAG_PROPGENERAL,	0,		0,			0,		// $dataname
	anumHScroll,				0,		fftInteger,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $hscroll
	anumVScroll,				0,		fftInteger,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $vscroll
	anumHorzscroll,				0,		fftBoolean,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $horzscroll
	anumVertscroll,				0,		fftBoolean,		EXTD_FLAG_PROPAPP,		0,		0,			0,		// $vortscroll
	
	oDL_columncount,			4110,	fftInteger,		EXTD_FLAG_PROPDATA,		0,		0,			0,		// $columncount
	oDL_columncalcs,			4111,	fftCharacter,	EXTD_FLAG_PROPDATA,		0,		0,			0,		// $columncalcs
	oDL_columnwidths,			4112,	fftCharacter,	EXTD_FLAG_PROPDATA,		0,		0,			0,		// $columnwidths
	oDL_columnaligns,			4113,	fftCharacter,	EXTD_FLAG_PROPDATA,		0,		0,			0,		// $columnaligns

	oDL_groupcalcs,				4114,	fftCharacter,	EXTD_FLAG_PROPDATA,		0,		0,			0,		// $groupcalcs	
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
		case oDL_columncount: {
			mColumnCount = pNewValue.getLong();
			if (mColumnCount < 1) mColumnCount = 1;
			
			ECOupdatePropInsp(mHWnd,oDL_columncalcs);
			ECOupdatePropInsp(mHWnd,oDL_columnwidths);
			ECOupdatePropInsp(mHWnd,oDL_columnaligns);
			
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		case oDL_columncalcs: {
			qstring		newcalc(pNewValue);
			qstring *	calc = new qstring();
			
			clearColumnCalcs();
			
			for (int i = 0; i < newcalc.length(); i++) {
				qchar	digit = *newcalc[i];
				
				if (digit == '\t') {
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
			
			for (int i = 0; i<widths.length(); i++) {
				qchar	digit = *widths[i];
				
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
			
			for (int i = 1; i<aligns.length(); i++) {
				qchar	digit = *aligns[i];
				
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
		case oDL_groupcalcs: {
			qstring		newcalc(pNewValue);
			qstring *	calc = new qstring();
			
			clearGroupCalcs();
			
			for (int i = 0; i < newcalc.length(); i++) {
				qchar	digit = *newcalc[i];
				
				if (digit == '\t') {
					mGroupCalculations.push(calc);
					calc = new qstring();
				} else {
					*calc += digit;
				};
			};
			
			if (calc->length()>0) {
				mGroupCalculations.push(calc);
			} else {
				delete calc;
			};
			
			WNDinvalidateRect(mHWnd, NULL);
			return qtrue;
		}; break;
		default:
			return oBaseVisComponent::setProperty(pPropID, pNewValue, pECI);
			break;
	};
};

// get the value of a property
void oDataList::getProperty(qlong pPropID,EXTfldval &pGetValue,EXTCompInfo* pECI) {
	// most anum properties are managed by Omnis but some we need to do ourselves...
	
	switch (pPropID) {
		case oDL_columncount: {
			pGetValue.setLong(mColumnCount);
		}; break;
		case oDL_columncalcs: {
			qstring	columncalcs;
			
			for (qlong i = 0; i<mColumnCount; i++) {
				if (i!=0) {
					columncalcs += QTEXT("\t");
				};
				if (i<mColumnCalculations.numberOfElements()) {
					qstring *	calcstr = mColumnCalculations[i];
					columncalcs += *calcstr;
				};
			};
			
			pGetValue.setChar((qchar *)columncalcs.cString(), columncalcs.length());
			
		}; break;
		case oDL_columnwidths: {
			qstring		widths = QTEXT("");;
			for (qlong i = 0; i<mColumnCount; i++) {
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
		}; break;
		case oDL_columnaligns: {
			qstring		aligns = QTEXT("");
			for (qlong i = 0; i<mColumnCount; i++) {				
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
		}; break;
		case oDL_groupcalcs: {
			qstring	groupcalcs;
			
			for (int i = 0; i < mGroupCalculations.numberOfElements(); i++) {
				if (i!=0) {
					groupcalcs += QTEXT(",");
				};
				
				groupcalcs += *mGroupCalculations[i];
			};
			
			pGetValue.setChar((qchar *)groupcalcs.cString(), groupcalcs.length());
		}; break;
		default:
			oBaseVisComponent::getProperty(pPropID, pGetValue, pECI);
			
			break;
	};
};	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// list
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Changes our primary data
qbool	oDataList::setPrimaryData(EXTfldval &pNewValue) {
	// we are not keeping a copy of our list

	WNDinvalidateRect(mHWnd, NULL);	
	
	return 1L;
};

// Retrieves our primary data
void	oDataList::getPrimaryData(EXTfldval &pGetValue) {
	// we are not keeping a copy of our list	
};

// Compare with our primary data
qbool	oDataList::cmpPrimaryData(EXTfldval &pWithValue) {
	// we are not keeping a copy of our list	
	return qtrue;
};

// Get our primary data size
qlong	oDataList::getPrimaryDataLen() {
	return 0L;
};

// Omnis is just letting us know our primary data has changed, this is especially handy if we do not keep a copy ourselves and thus ignore the other functions
void	oDataList::primaryDataHasChanged() {
	WNDinvalidateRect(mHWnd, NULL);	
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is our array of methods we support
// ECOmethodEvent oDataListMethods[] = {
//	ID				Resource	Return type		Paramcount		Params					Flags		ExFlags
//	1,					8000,		fftCharacter,	0,				NULL,					0,			0,			// $testMethod	
// };

// return an array of method meta data
qMethods * oDataList::methods(void) {
	qMethods * lvMethods = oBaseVisComponent::methods();
	
	//	lvMethods->addElements(oCountButtonMethods, sizeof(oCountButtonMethods) / sizeof(ECOmethodEvent));		
	
	return lvMethods;
};

// invoke a method
int	oDataList::invokeMethod(qlong pMethodId, EXTCompInfo* pECI) {
	/*	switch (pMethodId) {
	 case 1: {
	 EXTfldval	lvResult;
	 str255		lvString(QTEXT("Hello world from our visual object!"));
	 
	 lvResult.setChar(lvString);
	 
	 ECOaddParam(pECI, &lvResult);
	 return 1L;							
	 }; break;
	 default: {
	 return oBaseVisComponent::invokeMethod(pMethodId, pECI);
	 }; break;
	 }*/
	return 1L;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ECOmethodEvent oDataListEvents[] = {
	//	ID					Resource	Return type		Paramcount		Params					Flags		ExFlags
	oDL_evClick,			5000,		0,				0,				0,						0,			0,
};	

// return an array of events meta data
qEvents *	oDataList::events(void) {
	qEvents *	lvEvents = oBaseNVComponent::events();
	
	// Add the event definition for our visual component here...
	lvEvents->addElements(oDataListEvents, sizeof(oDataListEvents) / sizeof(ECOmethodEvent));
	
	return lvEvents;
};

void	oDataList::evClick(qpoint pAt) {
	// Need to change this so we select the line we clicked on.
	ECOsendEvent(mHWnd, oDL_evClick, 0, 0, EEN_EXEC_IMMEDIATE);	
};	

