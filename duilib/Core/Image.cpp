#include "StdAfx.h"
#include "Image.h"
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#ifndef __STDC_LIB_EXT1__
#define __STDC_LIB_EXT1__
#endif
#include "lulusvg\3rdparty\stb\stb_image_write.h"
#endif

namespace ui 
{

ImageInfo::ImageInfo() :
	nX(0),
	nY(0),
	sImageFullPath(_T("")),
	sImageGroupID(_T("")),
	m_bAlphaChannel(false),
	m_bCached(false),
	m_propertyItem(),
	m_vecBitmap()
{

}

ImageInfo::~ImageInfo()
{
	for (auto it = m_vecBitmap.begin(); it != m_vecBitmap.end(); it++) {
		::DeleteObject(*it);
	}
}

void ImageInfo::SetPropertyItem(Gdiplus::PropertyItem* pPropertyItem)
{
	m_propertyItem.reset(pPropertyItem);
}

void ImageInfo::PushBackHBitmap(HBITMAP hBitmap)
{
	m_vecBitmap.push_back(hBitmap);
}

HBITMAP ImageInfo::GetHBitmap(int nIndex)
{
	return m_vecBitmap[nIndex];
}

int ImageInfo::GetFrameCount()
{
	return (int)m_vecBitmap.size();
}

bool ImageInfo::IsGif()
{
	return m_vecBitmap.size() > 1;
}

int ImageInfo::GetInterval(int nIndex)
{
	if (m_propertyItem == nullptr)
		return 0;

	if (nIndex >= (int)m_vecBitmap.size())
		return 0;

	int interval = ((long*)(m_propertyItem->value))[nIndex] * 10;
	if (interval < 30) {
		interval = 100;
	}
	else if (interval < 50)	{
		interval = 50;
	}
	return interval;
}

std::unique_ptr<ImageInfo> ImageInfo::LoadImage(const std::wstring& strImageFullPath, std::wstring sGroupID, double dScale)
{
	if (StringHelper::EndWith(strImageFullPath, _T(".svg")))
	{
		std::string st;
		StringHelper::UnicodeToMBCS(strImageFullPath, st);
		if (sGroupID.empty())
		{
			auto document = lunasvg::Document::loadFromFile(st);
			auto pIStream = RenderImageGroup(document, dScale);
			if (pIStream == nullptr)
				return nullptr;
			std::unique_ptr<Gdiplus::Bitmap> gdiplusBitmap(Gdiplus::Bitmap::FromStream(pIStream));
			return LoadImageByBitmap(gdiplusBitmap, strImageFullPath, sGroupID);
		}
		else
		{
			auto tb = lunasvg::ITreeBuilder::parseFromFile(st);
			return LoadImageGroup(tb, sGroupID, dScale);
		}
	}
	std::unique_ptr<Gdiplus::Bitmap> gdiplusBitmap(Gdiplus::Bitmap::FromFile(strImageFullPath.c_str()));
	return LoadImageByBitmap(gdiplusBitmap, strImageFullPath, sGroupID);
}

std::unique_ptr<ImageInfo> ImageInfo::LoadImage(HGLOBAL hGlobal, const std::wstring& imageFullPath, std::wstring sGroupID, double dScale)
{
	if (hGlobal == NULL)
	{
		return nullptr;
	}
	IStream* stream = NULL;
	GlobalLock(hGlobal);
	CreateStreamOnHGlobal(hGlobal, true, &stream);
	if (stream == NULL)
	{
		GlobalUnlock(hGlobal);
		return nullptr;
	}
	std::unique_ptr<Gdiplus::Bitmap> gdiplusBitmap(Gdiplus::Bitmap::FromStream(stream));
	stream->Release();
	GlobalUnlock(hGlobal);
	return LoadImageByBitmap(gdiplusBitmap, imageFullPath, sGroupID);
}

std::unique_ptr<ui::ImageInfo> ImageInfo::LoadImageGroup(std::unique_ptr<lunasvg::ITreeBuilder>& tb, const std::wstring& sGroupID, double dScale)
{
	if (!tb)
	{
		ASSERT(FALSE);
		return nullptr;
	}
	std::string sgid;
	StringHelper::UnicodeToMBCS(sGroupID, sgid);
	auto document = tb->buildClip(sgid);
	auto pIStream = RenderImageGroup(document, dScale);
	if (pIStream == nullptr)
		return nullptr;
	// 显示
	std::unique_ptr<Gdiplus::Bitmap> gdiplusBitmap(Gdiplus::Bitmap::FromStream(pIStream));
	return LoadImageByBitmap(gdiplusBitmap, _T(""), sGroupID);
}

std::unique_ptr<ImageInfo> ImageInfo::LoadImageByBitmap(std::unique_ptr<Gdiplus::Bitmap>& pGdiplusBitmap, const std::wstring& strImageFullPath, std::wstring  sGroup)
{
	Gdiplus::Status status;
	status = pGdiplusBitmap->GetLastStatus();
	ASSERT(status == Gdiplus::Ok);
	if (status != Gdiplus::Ok) {
		return nullptr;
	}

	UINT nCount = pGdiplusBitmap->GetFrameDimensionsCount();
	std::unique_ptr<GUID[]> pDimensionIDs(new GUID[nCount]);
	pGdiplusBitmap->GetFrameDimensionsList(pDimensionIDs.get(), nCount);
	int iFrameCount = pGdiplusBitmap->GetFrameCount(&pDimensionIDs.get()[0]);

	std::unique_ptr<ImageInfo> imageInfo(new ImageInfo);
	if (iFrameCount > 1) {
		int iSize = pGdiplusBitmap->GetPropertyItemSize(PropertyTagFrameDelay);
		Gdiplus::PropertyItem* pPropertyItem = (Gdiplus::PropertyItem*)malloc(iSize);
		status = pGdiplusBitmap->GetPropertyItem(PropertyTagFrameDelay, iSize, pPropertyItem);
		ASSERT(status == Gdiplus::Ok);
		if (status != Gdiplus::Ok) {
			return nullptr;
		}
		imageInfo->SetPropertyItem(pPropertyItem);
	}

	for (int i = 0; i < iFrameCount; i++) {
		status = pGdiplusBitmap->SelectActiveFrame(&Gdiplus::FrameDimensionTime, i);
		ASSERT(status == Gdiplus::Ok);
		if (status != Gdiplus::Ok) {
			return nullptr;
		}

		HBITMAP hBitmap;
		status = pGdiplusBitmap->GetHBITMAP(Gdiplus::Color(), &hBitmap);
		ASSERT(status == Gdiplus::Ok);
		if (status != Gdiplus::Ok) {
			return nullptr;
		}
		imageInfo->PushBackHBitmap(hBitmap);
	}

	imageInfo->nX = pGdiplusBitmap->GetWidth();
	imageInfo->nY = pGdiplusBitmap->GetHeight();
	imageInfo->sImageFullPath = strImageFullPath;
	imageInfo->sImageGroupID = sGroup;
	Gdiplus::PixelFormat format = pGdiplusBitmap->GetPixelFormat();
	imageInfo->SetAlpha((format & PixelFormatAlpha) != 0);

	if ((format & PixelFormatIndexed) != 0) {
		int nPalSize = pGdiplusBitmap->GetPaletteSize();
		if (nPalSize > 0) {
			Gdiplus::ColorPalette *palette = (Gdiplus::ColorPalette*)malloc(nPalSize);;
			status = pGdiplusBitmap->GetPalette(palette, nPalSize);
			if (status == Gdiplus::Ok) {
				imageInfo->SetAlpha((palette->Flags & Gdiplus::PaletteFlagsHasAlpha) != 0);
			}
			free(palette);
		}
	}

	if (format == PixelFormat32bppARGB) {
		for (int nFrameIndex = 0; nFrameIndex < iFrameCount; nFrameIndex++) {
			HBITMAP hBitmap = imageInfo->GetHBitmap(nFrameIndex);
			BITMAP bm;
			::GetObject(hBitmap, sizeof(bm), &bm);
			LPBYTE imageBits = (LPBYTE)bm.bmBits;
			for (int i = 0; i < bm.bmHeight; ++i) {
				for (int j = 0; j < bm.bmWidthBytes; j += 4) {
					int x = i * bm.bmWidthBytes + j;
					if (imageBits[x + 3] != 255) {
						imageInfo->SetAlpha(true);
						return imageInfo;
					}
				}
			}
		}

		imageInfo->SetAlpha(false);
		return imageInfo;
	}

	return imageInfo;
}

IStream* ImageInfo::RenderImageGroup(std::unique_ptr<lunasvg::Document>& document, double dScale /*= 1.0*/)
{
	if (!document)
	{
		ASSERT(FALSE);
		return nullptr;
	}
	if (dScale < 1e-4)
	{
		// #待处理,需要传入控件尺寸后计算比例系数.
		dScale = 1.0;
	}
	int  nScrWidth = round(document->width() * dScale);
	int  nScrHeight = round(document->height() * dScale);
	auto bitmap = document->renderToBitmap(nScrWidth, nScrHeight);
	if (!bitmap.valid())
	{
		ASSERT(FALSE);
		return nullptr;
	}
	bitmap.convertToRGBA();
	IStream* pIStream = 0;
	stbi_write_png_to_func([](void* context, void* data, int size) {
		IStream** ps = (IStream**)context;
		HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, size);
		BYTE* pMem = (BYTE*)::GlobalLock(hMem);
		memcpy(pMem, data, size);
		::CreateStreamOnHGlobal(hMem, FALSE, ps);
		}, &pIStream, bitmap.width(), bitmap.height(), 4, bitmap.data(), 0);
	return pIStream;
}

ImageAttribute::ImageAttribute()
{
	Init();
}

void ImageAttribute::Init()
{
	simageString.clear();
	sImageName.clear();
	bFade = 0xFF;
	bTiledX = false;
	bTiledY = false;
	rcDest.left = rcDest.top = rcDest.right = rcDest.bottom = DUI_NOSET_VALUE;
	rcSource.left = rcSource.top = rcSource.right = rcSource.bottom = DUI_NOSET_VALUE;
	rcCorner.left = rcCorner.top = rcCorner.right = rcCorner.bottom = 0;
	nPlayCount = -1;
	sSvgGroupID.clear();
	dSvgScale = 1.0;
}

void ImageAttribute::SetImageString(const std::wstring& strImageString)
{
	Init();
	simageString = strImageString;
	sImageName = strImageString;
	ModifyAttribute(*this, strImageString);
}

void ImageAttribute::ModifyAttribute(ImageAttribute& imageAttribute, const std::wstring& strImageString)
{
	std::wstring sItem;
	std::wstring sValue;
	LPTSTR pstr = NULL;
	bool bScaleDest = true;
	bool bHasFile = false;
	LPCTSTR pStrImage = strImageString.c_str();
	while (*pStrImage != _T('\0')) {
		sItem.clear();
		sValue.clear();
		while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
		while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) {
			LPTSTR pstrTemp = ::CharNext(pStrImage);
			while (pStrImage < pstrTemp) {
				sItem += *pStrImage++;
			}
		}
		while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
		if (*pStrImage++ != _T('=')) break;
		while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
		if (*pStrImage++ != _T('\'')) break;
		while (*pStrImage != _T('\0') && *pStrImage != _T('\'')) {
			LPTSTR pstrTemp = ::CharNext(pStrImage);
			while (pStrImage < pstrTemp) {
				sValue += *pStrImage++;
			}
		}
		if (*pStrImage++ != _T('\'')) break;
		if (!sValue.empty()) {
			if (sItem == ATTR__IMAGE_file || sItem == ATTR__IMAGE_res) {
				imageAttribute.sImageName = sValue;
				bHasFile = true;
			}
			else if (sItem == ATTR__IMAGE_destscale) {
				bScaleDest = (_tcscmp(sValue.c_str(), _T("true")) == 0);
			}
			else if (sItem == ATTR__IMAGE_dest) {
				imageAttribute.rcDest.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
				imageAttribute.rcDest.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
				imageAttribute.rcDest.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
				imageAttribute.rcDest.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

				if (bScaleDest)
					DpiManager::GetInstance()->ScaleRect(imageAttribute.rcDest);
			}
			else if (sItem == ATTR__IMAGE_source) {
				imageAttribute.rcSource.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
				imageAttribute.rcSource.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
				imageAttribute.rcSource.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
				imageAttribute.rcSource.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			}
			else if (sItem == ATTR__IMAGE_corner) {
				imageAttribute.rcCorner.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
				imageAttribute.rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
				imageAttribute.rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
				imageAttribute.rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			}
			else if (sItem == ATTR__IMAGE_fade) {
				imageAttribute.bFade = (BYTE)_tcstoul(sValue.c_str(), &pstr, 10);
			}
			else if (sItem == ATTR__IMAGE_xtiled) {
				imageAttribute.bTiledX = (_tcscmp(sValue.c_str(), _T("true")) == 0);
			}
			else if (sItem == ATTR__IMAGE_ytiled) {
				imageAttribute.bTiledY = (_tcscmp(sValue.c_str(), _T("true")) == 0);
			}
			else if (sItem == ATTR__IMAGE_playcount)
			{
				imageAttribute.nPlayCount = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);
			}
			else if (sItem == ATTR__IMAGE_svggroupid)
			{
				imageAttribute.sSvgGroupID = sValue;
			}
			else if (sItem == ATTR__IMAGE_svgscale)
			{
				imageAttribute.dSvgScale = _tcstod(sValue.c_str(), &pstr);  ASSERT(pstr);
			}
		}
		if (*pStrImage++ != _T(' ')) break;
	}
	// svg允许缺省文件名
	if (!bHasFile && !imageAttribute.sSvgGroupID.empty())
	{
		imageAttribute.sImageName = GLOBAL_VSGFILE_KEY;
	}
}


Image::Image() :
	imageAttribute(),
	imageCache(),
	m_nCurrentFrame(0),
	m_bPlaying(false),
	m_nCycledCount(0)
{

}

void Image::SetImageString(const std::wstring& strImageString)
{
	ClearCache();
	imageAttribute.SetImageString(strImageString);
}

void Image::ClearCache()
{
	m_nCurrentFrame = 0;
	m_bPlaying = false;
	imageCache.reset();
	m_nCycledCount = 0;
}

bool Image::IncrementCurrentFrame()
{
	if (!imageCache) {
		return false;
	}
	m_nCurrentFrame++;
	if (m_nCurrentFrame == imageCache->GetFrameCount()) {
		m_nCurrentFrame = 0;
		m_nCycledCount += 1;
	}
	return true;
}

void Image::SetCurrentFrame(int nCurrentFrame)
{
	m_nCurrentFrame = nCurrentFrame;
}

HBITMAP Image::GetCurrentHBitmap()
{
	if (!imageCache) {
		return NULL;
	}
	return imageCache->GetHBitmap(m_nCurrentFrame);
}

int Image::GetCurrentInterval()
{
	if (!imageCache) {
		return 0;
	}
	return imageCache->GetInterval(m_nCurrentFrame);
}
int Image::GetCurrentFrameIndex()
{
	return m_nCurrentFrame;
}
int Image::GetCycledCount()
{
	return m_nCycledCount;
}
void Image::ClearCycledCount()
{
	m_nCycledCount = 0;
}
bool Image::ContinuePlay()
{
	if (imageAttribute.nPlayCount < 0)
		return true;
	else if (imageAttribute.nPlayCount == 0)
		return m_bPlaying;
	else
		return m_nCycledCount < imageAttribute.nPlayCount;
}
StateImage::StateImage() :
	m_pControl(nullptr),
	m_stateImageMap()
{

}

bool StateImage::HasHotImage()
{
	return !m_stateImageMap[kControlStateHot].imageAttribute.simageString.empty();
}

bool StateImage::PaintStatusImage(IRenderContext* pRender, ControlStateType stateType, const std::wstring& sImageModify /*= L""*/)
{
	if (m_pControl) {
		bool bFadeHot = m_pControl->GetAnimationManager().GetAnimationPlayer(kAnimationHot) != nullptr;
		int nHotAlpha = m_pControl->GetHotAlpha();
		if (bFadeHot) {
			if (stateType == kControlStateNormal || stateType == kControlStateHot) {
				std::wstring strNormalImageKey = m_stateImageMap[kControlStateNormal].imageAttribute.ImgKey();
				std::wstring strHotImageKey = m_stateImageMap[kControlStateHot].imageAttribute.ImgKey();

				if (strNormalImageKey.empty() || strHotImageKey.empty()
					|| strNormalImageKey != strHotImageKey
					|| !m_stateImageMap[kControlStateNormal].imageAttribute.rcSource.Equal(m_stateImageMap[kControlStateHot].imageAttribute.rcSource)) {

					m_pControl->DrawImage(pRender, m_stateImageMap[kControlStateNormal], sImageModify);
					int nHotFade = m_stateImageMap[kControlStateHot].imageAttribute.bFade;
					nHotFade = int(nHotFade * (double)nHotAlpha / 255);
					return m_pControl->DrawImage(pRender, m_stateImageMap[kControlStateHot], sImageModify, nHotFade);
				}
				else {
					int nNormalFade = m_stateImageMap[kControlStateNormal].imageAttribute.bFade;
					int nHotFade = m_stateImageMap[kControlStateHot].imageAttribute.bFade;
					int nBlendFade = int((1 - (double)nHotAlpha / 255) * nNormalFade + (double)nHotAlpha / 255 * nHotFade);
					return m_pControl->DrawImage(pRender, m_stateImageMap[kControlStateHot], sImageModify, nBlendFade);
				}
			}
		}
	}

	if (stateType == kControlStatePushed && m_stateImageMap[kControlStatePushed].imageAttribute.simageString.empty()) {
		stateType = kControlStateHot;
		m_stateImageMap[kControlStateHot].imageAttribute.bFade = 255;
	}
	if (stateType == kControlStateHot && m_stateImageMap[kControlStateHot].imageAttribute.simageString.empty()) {
		stateType = kControlStateNormal;
	}
	if (stateType == kControlStateDisabled && m_stateImageMap[kControlStateDisabled].imageAttribute.simageString.empty()) {
		stateType = kControlStateNormal;
	}

	return m_pControl->DrawImage(pRender, m_stateImageMap[stateType], sImageModify);
}

Image* StateImage::GetEstimateImage()
{
	Image* pEstimateImage = nullptr;
	if (!m_stateImageMap[kControlStateNormal].imageAttribute.sImageName.empty()){
		pEstimateImage = &m_stateImageMap[kControlStateNormal];
	}
	else if (!m_stateImageMap[kControlStateHot].imageAttribute.sImageName.empty()) {
		pEstimateImage = &m_stateImageMap[kControlStateHot];
	}
	else if (!m_stateImageMap[kControlStatePushed].imageAttribute.sImageName.empty()) {
		pEstimateImage = &m_stateImageMap[kControlStatePushed];
	}
	else if (!m_stateImageMap[kControlStateDisabled].imageAttribute.sImageName.empty()) {
		pEstimateImage = &m_stateImageMap[kControlStateDisabled];
	}

	return pEstimateImage;
}

void StateImage::ClearCache()
{
	auto it = m_stateImageMap.find(kControlStateNormal);
	if (it != m_stateImageMap.end())
	{
		it->second.ClearCache();
	}
	it = m_stateImageMap.find(kControlStateHot);
	if (it != m_stateImageMap.end())
	{
		it->second.ClearCache();
	}
	it = m_stateImageMap.find(kControlStatePushed);
	if (it != m_stateImageMap.end())
	{
		it->second.ClearCache();
	}
	it = m_stateImageMap.find(kControlStateDisabled);
	if (it != m_stateImageMap.end())
	{
		it->second.ClearCache();
	}
}


void StateImageMap::SetControl(Control* control)
{
	m_stateImageMap[kStateImageBk].SetControl(control);
	m_stateImageMap[kStateImageFore].SetControl(control);
	m_stateImageMap[kStateImageSelectedBk].SetControl(control);
	m_stateImageMap[kStateImageSelectedFore].SetControl(control);
}

void StateImageMap::SetImage(StateImageType stateImageType, ControlStateType stateType, const std::wstring& strImagePath)
{
	m_stateImageMap[stateImageType][stateType].SetImageString(strImagePath);
}

std::wstring StateImageMap::GetImagePath(StateImageType stateImageType, ControlStateType stateType)
{
	return m_stateImageMap[stateImageType][stateType].imageAttribute.simageString;
}

bool StateImageMap::HasHotImage()
{
	for (auto& it : m_stateImageMap) {
		if (it.second.HasHotImage()) {
			return true;
		}
	}
	return false;
}

bool StateImageMap::PaintStatusImage(IRenderContext* pRender, StateImageType stateImageType, ControlStateType stateType, const std::wstring& sImageModify /*= L""*/)
{
	auto it = m_stateImageMap.find(stateImageType);
	if (it != m_stateImageMap.end()) {
		return it->second.PaintStatusImage(pRender, stateType, sImageModify);
	}
	return false;
}

Image* StateImageMap::GetEstimateImage(StateImageType stateImageType)
{
	auto it = m_stateImageMap.find(stateImageType);
	if (it != m_stateImageMap.end()) {
		return it->second.GetEstimateImage();
	}
	return nullptr;
}

void StateImageMap::ClearCache()
{
	m_stateImageMap[kStateImageBk].ClearCache();
	m_stateImageMap[kStateImageFore].ClearCache();
	m_stateImageMap[kStateImageSelectedBk].ClearCache();
	m_stateImageMap[kStateImageSelectedFore].ClearCache();
}


StateColorMap::StateColorMap() :
	m_pControl(nullptr),
	m_stateColorMap()
{
}

void StateColorMap::SetControl(Control* control)
{
	m_pControl = control;
}

bool StateColorMap::HasHotColor()
{
	return !m_stateColorMap[kControlStateHot].empty();
}

void StateColorMap::PaintStatusColor(IRenderContext* pRender, UiRect rcPaint, ControlStateType stateType)
{
	if (m_pControl) {
		bool bFadeHot = m_pControl->GetAnimationManager().GetAnimationPlayer(kAnimationHot) != nullptr;
		int nHotAlpha = m_pControl->GetHotAlpha();
		if (bFadeHot) {
			if ((stateType == kControlStateNormal || stateType == kControlStateHot)
				&& !m_stateColorMap[kControlStateHot].empty()) {

				pRender->DrawColor(rcPaint, m_stateColorMap[kControlStateNormal]);
				if (nHotAlpha > 0) {
					pRender->DrawColor(rcPaint, m_stateColorMap[kControlStateHot], nHotAlpha);
				}
				return;
			}
		}
	}

	if (stateType == kControlStatePushed && m_stateColorMap[kControlStatePushed].empty()) {
		stateType = kControlStateHot;
	}
	if (stateType == kControlStateHot && m_stateColorMap[kControlStateHot].empty()) {
		stateType = kControlStateNormal;
	}
	if (stateType == kControlStateDisabled && m_stateColorMap[kControlStateDisabled].empty()) {
		stateType = kControlStateNormal;
	}

	pRender->DrawColor(rcPaint, m_stateColorMap[stateType]);
}

}