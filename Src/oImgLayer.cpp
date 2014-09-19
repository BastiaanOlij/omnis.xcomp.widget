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
void	oImgLayer::drawLayer(oRGBAImage & pOnto, bool pMix) {
	// nothing to do here..
};	


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// oImgBitmap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// init as empty bitmap
oImgBitmap::oImgBitmap(qlong pWidth, qlong pHeight, qcol pColor) {
	mImage = new oRGBAImage(pWidth, pHeight, pColor);
};

oImgBitmap::~oImgBitmap() {
	if (mImage != NULL) {
		delete mImage;
		mImage = NULL;
	};
};

const char * oImgBitmap::layerType() {
	return "Bitmap";
};

qlong	oImgBitmap::width() {
	if (mImage == NULL) {
		return 0;
	} else if (mWidth == 0) {
		return mImage->width();
	} else {
		return mWidth;
	};
};

void	oImgBitmap::setWidth(qlong pNewValue) {
	if (mImage == NULL) {
		mWidth = pNewValue;				
	} else if (mImage->width() == pNewValue) {
		mWidth = 0;
	} else {
		mWidth = pNewValue;		
	};
};

qlong	oImgBitmap::height() {
	if (mImage == NULL) {
		return 0;
	} else if (mHeight == 0) {
		return mImage->height();
	} else {
		return mHeight;
	};	
};

void	oImgBitmap::setHeight(qlong pNewValue) {
	if (mImage == NULL) {
		mHeight = pNewValue;
	} else if (mImage->height() == pNewValue) {
		mHeight = 0;		
	} else {		
		mHeight = pNewValue;
	};
};

void	oImgBitmap::getContents(EXTfldval &pContents) {
	if (mImage == NULL) {
		pContents.setEmpty(fftBinary, 0);		
	} else {
		int				len = 0;
		unsigned char *	pngdata = mImage->asPNG(len);

		if (pngdata == NULL) {
			oBaseComponent::addToTraceLog("getContents: No image");
			pContents.setEmpty(fftBinary, 0);		
		} else {
			pContents.setBinary(fftBinary, pngdata, len);

			free(pngdata);
		};
	};
};

void	oImgBitmap::setContents(EXTfldval &pContents) {
	if (mImage == NULL) {
		// really?
	} else {
		// We get the binary as is, we assume the contents is raw picture data in any format supported by STB
		qlong bitmapLen = pContents.getBinLen();
		if (bitmapLen > 0) {
			qbyte * bitmapData = (qbyte *)MEMmalloc(sizeof(qbyte) * (bitmapLen+1));
			if (bitmapData != NULL) {
				qlong	reallen;
				pContents.getBinary(bitmapLen, bitmapData, reallen);

				// and assign it to our image
				mImage->copy(bitmapData, reallen);

				// and free our memory
				MEMfree(bitmapData);
			} else {
				oBaseComponent::addToTraceLog("setContents: Couldn't allocate memory for pixel buffer");
			};
		} else {
			oBaseComponent::addToTraceLog("setContents: No image specified");
		};
	};
};

// draw our layer onto the pixmap..
void	oImgBitmap::drawLayer(oRGBAImage & pOnto, bool pMix) {
	if (mImage == NULL) {
		// nothing to draw..
		oBaseComponent::addToTraceLog("drawLayer: No source image");
		return;
	};

	// get the size of our destination bitmap
	qlong	destwidth	= pOnto.width();
	qlong	destheight	= pOnto.height();

	if ((destwidth == 0) || (destheight == 0)) {
		// nothing to draw..
		oBaseComponent::addToTraceLog("drawLayer: Empty destination image");
		return;
	};

	// get the size we want our bitmap to be
	qlong	sourcewidth		= mImage->width();
	qlong	sourceheight	= mImage->height();
	qlong	scaledwidth		= width();
	qlong	scaledheight	= height();

	if ((sourcewidth==0) || (sourceheight==0)) {
		// nothing to draw..
		oBaseComponent::addToTraceLog("drawLayer: Empty source image");
		return;
	};

	qlong	maxX = (mLeft + scaledwidth);
	if (maxX > destwidth) maxX = destwidth;

	qlong	maxY = (mTop + scaledheight);
	if (maxY > destheight) maxY = destheight;
		
	if ((sourcewidth != scaledwidth) || (sourceheight != scaledheight)) {
		// we're scaling this so we handle this slightly differently
		// this code works well when enlarging our bitmap but is overkill for making things smaller
		// look into using the resample logic available in STB, it may do a much better job, but thats for the next version...
			
		bool	issmaller = (sourcewidth > scaledwidth) && (sourceheight > scaledheight);
			
		float	stepX = (float) sourcewidth;
		float	stepY = (float) sourceheight;
				
		stepX = stepX / scaledwidth;
		stepY = stepY / scaledheight;
				
		float	sY = 0.0;
				
		for (qlong dY = mTop; dY < maxY; dY++) {
			if (dY >= 0) {
				float	sX = 0.0;
						
				for (qlong dX = mLeft; dX < maxX; dX++) {
					if (dX >= 0)  {
						sPixel pixel;
						if (issmaller) {
							// just get it
							pixel = (* mImage)((qlong) floor(sX), (qlong) floor(sY));
						} else {
							// interpolate
							pixel = mImage->getPixel(sX, sY);
						};
						if (pMix) {
							pOnto(dX, dY) = oRGBAImage::mixPixel(pOnto(dX, dY), pixel, mAlpha);
						} else {
							pOnto(dX, dY) = pixel;
						};
					};
							
					sX += stepX;
				};
			};
			sY += stepY;
		}; 
	} else {
		// not scaled? just copy it!
		qlong	sY = 0;
				
		for (qlong dY = mTop; dY < maxY; dY++) {
			if (dY >= 0) {
				qlong	sX = 0;
				
				for (qlong dX = mLeft; dX < maxX; dX++) {
					if (dX >= 0)  {
						sPixel	pixel = (*mImage)(sX, sY);
						if (pMix) {
							pOnto(dX, dY) = oRGBAImage::mixPixel(pOnto(dX, dY), pixel, mAlpha);
						} else {
							pOnto(dX, dY) = pixel;
						};
					};
							
					sX++;
				};
			};
					
			sY++;
		};				
	};			
};	

