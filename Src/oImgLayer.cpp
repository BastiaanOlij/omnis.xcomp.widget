/*
 *  omnis.xcomp.widget
 *  ===================
 *
 *  oImgLayer.h
 *  Source for our image layer objects
 *
 *  This object represents a single image layer for our image system
 *  The baseclass image layer defines:
 *  - sizing information for our layer
 *  - scaling information for this layer
 *  - offset information for this layer
 *  - an alpha value to apply to the image (or multiplied with the alpha in the image)
 *
 *  We have a bitmap subclass that defines a layer based on an uncompressed bitmap contained within
 * 
 *  Bastiaan Olij
 */

#include "oImgLayer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// oImgLayer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

oImgLayer::oImgLayer(void) {
	mEnabled	= true;
	mLeft		= 0;
	mTop		= 0;
	mWidth		= 0;	// 0 = autosize
	mHeight		= 0;	// 0 = autosize
	mAlpha		= 255;	// 255 = fully opaque
};

oImgLayer::~oImgLayer(void) {
	// nothing to do here
};

sPixel	oImgLayer::mixPixel(sPixel pBase, sPixel pAdd, qbyte pAlpha) {
	if ((pAlpha == 255) && (pAdd.mA == 255)) {
		// opaque pixel...
		return pAdd;
	} else if ((pAlpha == 0) || (pAdd.mA == 0)) {		
		// fully transparent pixel
		return pBase;
	} else {
		sPixel	result;
		qlong	value, alpha = pAlpha;

		alpha		*= pAdd.mA;
		result.mA	= 255;
		
		// Red
		value		= pBase.mR;
		value		*= (255 * 255) - alpha;
		result.mR	= value >> 16;
		
		value		= pAdd.mR;
		value		*= alpha;
		result.mR	+= (value >> 16);
		
		// Green
		value		= pBase.mG;
		value		*= (255 * 255) - alpha;
		result.mG	= value >> 16;
		
		value		= pAdd.mG;
		value		*= alpha;
		result.mG	+= (value >> 16);

		// blue
		value		= pBase.mB;
		value		*= (255 * 255) - alpha;
		result.mB	= value >> 16;
		
		value		= pAdd.mB;
		value		*= alpha;
		result.mB	+= (value >> 16);
		
		return result;
	};
};


const char * oImgLayer::layerType() {
	return "ImgLayer";
};

bool oImgLayer::enabled() {
	return mEnabled;
};

void	oImgLayer::setEnabled(bool pEnabled) {
	mEnabled = pEnabled;
};

qlong	oImgLayer::left() {
	return mLeft;
};

void	oImgLayer::setLeft(qlong pNewValue) {
	mLeft = pNewValue;
};

qlong	oImgLayer::top() {
	return mTop;
};

void	oImgLayer::setTop(qlong pNewValue) {
	mTop = pNewValue;
};

qlong	oImgLayer::width() {
	return mWidth;
};

void	oImgLayer::setWidth(qlong pNewValue) {
	mWidth = pNewValue;
};

qlong	oImgLayer::height() {
	return mHeight;
};

void	oImgLayer::setHeight(qlong pNewValue) {
	mHeight = pNewValue;
};

void	oImgLayer::getContents(EXTfldval &pContents) {
	pContents.setEmpty(fftBinary, 0);
};

void	oImgLayer::setContents(EXTfldval &pContents) {
	// nothing to do here...
};

// draw our layer onto the pixmap..
void	oImgLayer::drawLayer(HPIXMAP pOnto, bool pMix) {
	// nothing to do here..
};	


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// oImgBitmap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// init as empty bitmap
oImgBitmap::oImgBitmap(qlong pWidth, qlong pHeight, qcol pColor) {
	mImgWidth	= pWidth;
	mImgHeight	= pHeight;
	if ((pWidth == 0) && (pHeight == 0)) {
		mPixmap = 0;
	} else {
		mPixmap = GDIcreateHPIXMAP(pWidth, pHeight, 32, false);
		
		sPixel *	pixels = (sPixel *) GDIlockHPIXMAP(mPixmap);
		sPixel		pixel;
		
		// get real RGB value..
		qlong col = GDIgetRealColor(pColor);

		// split into RGBA components
		// need to check if this is the same on windows..
		pixel.mR = col & 0xFF;
		pixel.mG = (col >> 8) & 0xFF;
		pixel.mB = (col >> 16) & 0xFF;
		// pixel.mA = (col >> 24) & 0xFF; 
		pixel.mA = 255; // note, not supplied by omnis generally so we ignore...
		
//		oBaseComponent::addToTraceLog("RGB: %li, %li, %li, %li, %li",col,pixel.mR, pixel.mB, pixel.mG, pixel.mA);
		
		for (qlong Y = 0;Y<pWidth*pHeight; Y+=pWidth) {
			for (qlong X = 0;X<pWidth;X++) {
				pixels[Y+X] = pixel;
			};			
		}
		
		GDIunlockHPIXMAP(mPixmap);
	};
};

oImgBitmap::~oImgBitmap() {
	if (mPixmap != 0) {
		GDIdeleteHPIXMAP(mPixmap);
		mPixmap		= 0;
		mImgWidth	= 0;
		mImgHeight	= 0;
	};
};

const char * oImgBitmap::layerType() {
	return "Bitmap";
};

qlong	oImgBitmap::width() {
	if (mWidth == 0) {
		return mImgWidth;
	} else {
		return mWidth;
	};
};

void	oImgBitmap::setWidth(qlong pNewValue) {
	if (mImgWidth == pNewValue) {
		mWidth = 0;
	} else {
		mWidth = pNewValue;		
	};
};

qlong	oImgBitmap::height() {
	if (mHeight == 0) {
		return mImgHeight;
	} else {
		return mHeight;
	};	
};

void	oImgBitmap::setHeight(qlong pNewValue) {
	if (mImgHeight == pNewValue) {
		mHeight = 0;		
	} else {		
		mHeight = pNewValue;
	};
};

void	oImgBitmap::getContents(EXTfldval &pContents) {
	if (mPixmap==0) {
		pContents.setEmpty(fftPicture, 0);		
	} else {
		GDIbitmapToColorShared(mPixmap, pContents);
	};
};

void	oImgBitmap::setContents(EXTfldval &pContents) {
	GDIdeleteHPIXMAP(mPixmap);
	mPixmap		= 0;
	mImgWidth	= 0;
	mImgHeight	= 0;
	
	// first convert to shared picture
	GDIconvToSharedPict(pContents);
	
	// now get our shared picture data
	qlong bitmapLen = pContents.getBinLen();
	if (bitmapLen > 0) {
		qbyte * bitmapData = (qbyte *)MEMmalloc(sizeof(qbyte) * (bitmapLen+1));
		if (bitmapData != NULL) {
			qlong	reallen;
			pContents.getBinary(bitmapLen, bitmapData, reallen);
			
			// and decompress it
			mPixmap = GDIHPIXMAPfromSharedPicture(bitmapData, bitmapLen);
			MEMfree(bitmapData);
			
			if (mPixmap != 0) {
				HPIXMAPinfo	pixmapinfo;
				GDIgetHPIXMAPinfo(mPixmap, &pixmapinfo);
				mImgWidth = pixmapinfo.mWidth;
				mImgHeight = pixmapinfo.mHeight;				

				oBaseComponent::addToTraceLog("setContents: loaded layer with image %li, %li",mImgWidth, mImgHeight);
			};
		} else {
			oBaseComponent::addToTraceLog("setContents: Couldn't allocate memory for pixel buffer");
		};
	};
};

// interpolate pixel
sPixel	oImgBitmap::interpolatePixel(sPixel pA, sPixel pB, float pFract) {
	sPixel	result;
	float	delta;
	
	if (pA.mR == pB.mR) {
		result.mR = pA.mR;
	} else {
		delta		= pB.mR - pA.mR;
		delta		*= pFract;
		result.mR	= pA.mR + delta;		
	};

	if (pA.mG == pB.mG) {
		result.mG = pA.mG;
	} else {
		delta		= pB.mG - pA.mG;
		delta		*= pFract;
		result.mG	= pA.mG + delta;
	};
	
	if (pA.mB == pB.mB) {
		result.mB = pA.mB;
	} else {
		delta		= pB.mB - pA.mB;
		delta		*= pFract;
		result.mB	= pA.mB + delta;
	};

	if (pA.mA == pB.mA) {
		result.mA = pA.mA;
	} else {
		delta		= pB.mA - pA.mA;
		delta		*= pFract;
		result.mA	= pA.mA + delta;
	};
	
	return result;
};

// get interpolated pixel
sPixel	oImgBitmap::getPixel(sPixel * pData, float pX, float pY) {
	sPixel col;

	if (pX < 0.0) pX = 0.0; // ???
	if (pY < 0.0) pY = 0.0; // ???	
	
	qlong	intX		= floor(pX);
	qlong	intY		= floor(pY);
	bool	interpolate	= true;
		
	// make sure we stay within our buffer...
	if (intX >= (mImgWidth - 1)) {
		intX			= mImgWidth - 1;
		interpolate		= false;
	};
	
	if (intY >= (mImgHeight - 1)) {
		intY			= mImgHeight - 1;
		interpolate		= false;
	};
	
	// calculate our top/left offset
	qlong	offset		= intY;
	offset *= mImgWidth;
	offset += intX;
	 
	// check if we need to interpolate
	if (interpolate) {
		float	fractX		= pX - intX;
		float	fractY		= pY - intY;
		
		if ((fractX > 0.0) || (fractY > 0.0)) {
			sPixel	topcol		= interpolatePixel(pData[offset], pData[offset+1], fractX);
			offset += mImgWidth;
			sPixel	bottomcol	= interpolatePixel(pData[offset], pData[offset+1], fractX);
			
			col = interpolatePixel(topcol, bottomcol, fractY);
		} else {
			col = pData[offset];			
		};
	} else {
		col = pData[offset];		
	};

	return col;		
};

// draw our layer onto the pixmap..
void	oImgBitmap::drawLayer(HPIXMAP pOnto, bool pMix) {
	if (mPixmap != 0) {
		// get info about the pixmap we're drawing on....
		HPIXMAPinfo	pixmapinfo;
		GDIgetHPIXMAPinfo(pOnto, &pixmapinfo);

		// now get our buffers....
		sPixel *	sourceData	= (sPixel *) GDIlockHPIXMAP(mPixmap);
		sPixel *	destData	= (sPixel *) GDIlockHPIXMAP(pOnto);
		
		if ((sourceData!=NULL) && (destData!=NULL)) {
			// get the size we want our bitmap to be
			qlong	scaledwidth = width();
			qlong	scaledheight = height();
			
			if ((mImgWidth != scaledwidth) || (mImgHeight != scaledheight)) {
				// we're scaling this so we handle this slightly differently
				// this code works well when enlarging our bitmap but is overkill for making things smaller
				float	stepX = mImgWidth;
				float	stepY = mImgHeight;
				
				stepX = stepX / scaledwidth;
				stepY = stepY / scaledheight;
				
				float	sY = 0.0;
				
				for (qlong Y = mTop; Y < (mTop + scaledheight); Y++) {
					if (Y >= pixmapinfo.mHeight) {
						// no need to continue
						Y = (mTop + scaledheight);
					} else if (Y >= 0) {
						qlong	dest = Y * pixmapinfo.mWidth;
						float	sX = 0.0;
						
						for (qlong X = mLeft; X < (mLeft + scaledwidth); X++) {
							if (X >= pixmapinfo.mWidth) {
								// no need to continue
								X = (mLeft + scaledwidth);
							} else if (X >= 0)  {
								sPixel pixel = getPixel(sourceData, sX, sY);
								if (pMix) {
									destData[dest + X] = mixPixel(destData[dest + X], pixel, mAlpha);
								} else {
									destData[dest + X] = pixel;
								}							
							};
							
							sX += stepX;
						};
					};
					sY += stepY;
				}; 
			} else {
				// not scaled? just copy it!
				qlong	sourceY = 0;
				
				for (qlong Y = mTop; Y < (mTop + scaledheight); Y++) {
					if (Y >= pixmapinfo.mHeight) {
						// no need to continue
						Y = (mTop + scaledheight);
					} else if (Y >= 0) {
						qlong	source = sourceY;
						qlong	dest = Y * pixmapinfo.mWidth;
						for (qlong X = mLeft; X < (mLeft + scaledwidth); X++) {
							if (X >= pixmapinfo.mWidth) {
								// no need to continue
								X = (mLeft + scaledwidth);
							} else if (X >= 0)  {
								sPixel	pixel = sourceData[source];
								if (pMix) {
									destData[dest + X] = mixPixel(destData[dest + X], pixel, mAlpha);
								} else {
									destData[dest + X] = pixel;
								}
								
								dest++;
							};
							
							source++;
						};
					};
					
					sourceY += mImgWidth;
				};
				
			};			
		} else {
			oBaseComponent::addToTraceLog("drawLayer: Couldn't access pixel buffers");
		};
		
		// unlock our buffers....
		GDIunlockHPIXMAP(pOnto);
		GDIunlockHPIXMAP(mPixmap);
	};
};	

