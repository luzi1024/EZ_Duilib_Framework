#ifndef UI_CORE_IMAGEDECODE_H_
#define UI_CORE_IMAGEDECODE_H_

#pragma once

#include <GdiPlus.h>
#include "..\..\..\lulusvg\include\lunasvg.h"
#define GLOBAL_VSGFILE_KEY  _T("../style.svg")

namespace ui 
{

class UILIB_API ImageInfo
{
	friend class Image;
public:
	ImageInfo();
	~ImageInfo();

	void SetAlpha(bool bAlphaChannel) {	m_bAlphaChannel = bAlphaChannel; }
	bool IsAlpha() { return m_bAlphaChannel; }
	bool IsCached()	{ return m_bCached; }
	void SetCached(bool bCached) { m_bCached = bCached; }
	bool IsGroup() { return !sImageGroupID.empty(); }

	void SetPropertyItem(Gdiplus::PropertyItem* pPropertyItem);

	void PushBackHBitmap(HBITMAP hBitmap);
	HBITMAP GetHBitmap(int nIndex);
	int GetFrameCount();
	bool IsGif();
	int GetInterval(int nIndex); //毫秒为单位 
	 
	static std::unique_ptr<ImageInfo> LoadImage(const ui::string& strImageFullPath, ui::string sGroupID = _T(""), double dScale = 1.0);
	static std::unique_ptr<ImageInfo> LoadImage(HGLOBAL hGlobal, const ui::string& imageFullPath, ui::string sGroupID = _T(""), double dScale = 1.0);
	static std::unique_ptr<ImageInfo> LoadImageGroup(std::unique_ptr<lunasvg::ITreeBuilder>& tb, const ui::string& sGroupID, double dScale);
private:
	static std::unique_ptr<ImageInfo> LoadImageByBitmap(std::unique_ptr<Gdiplus::Bitmap>& pGdiplusBitmap, const ui::string& imageFullPath, ui::string  sGroup = _T(""));
	static IStream* RenderImageGroup(std::unique_ptr<lunasvg::Document>& document, double dScale = 1.0);
public:
	int nX;
	int nY;
	ui::string sImageFullPath;
	ui::string sImageGroupID;
private:
	bool m_bAlphaChannel;
	bool m_bCached;
	std::unique_ptr<Gdiplus::PropertyItem> m_propertyItem;
	std::vector<HBITMAP> m_vecBitmap;
};

struct UILIB_API ImageAttribute
{
public:
	ImageAttribute();

	void Init();
	void SetImageString(const ui::string& strImageString);
	static void ModifyAttribute(ImageAttribute& imageAttribute, const ui::string& strImageString);
	ui::string ImgKey() { return sImageName + sSvgGroupID; };

	ui::string simageString;
	ui::string sImageName;
	UiRect rcDest;
	UiRect rcSource;
	UiRect rcCorner;
	UiRect rcPadding;
	BYTE bFade;
	bool bTiledX;
	bool bTiledY;
	int nPlayCount;//如果是GIF可以指定播放次数 -1 ：一直播放，缺省值。
	ui::string sSvgGroupID;
	double dSvgScale;
	ui::string sFillcolor;
};

class UILIB_API Image
{
public:
	Image();

	bool Has() const { return !imageAttribute.simageString.empty(); }
	bool IsValid() const { return (bool)imageCache; }
	bool IsPlaying() const { return m_bPlaying; }
	void SetPlaying(bool bPlaying) { m_bPlaying = bPlaying; }

	void SetImageString(const ui::string& strImageString);
	void ClearCache();

	bool IncrementCurrentFrame();
	void SetCurrentFrame(int nCurrentFrame);
	HBITMAP GetCurrentHBitmap();
	int GetCurrentInterval();
	int GetCurrentFrameIndex();
	int GetCycledCount();
	void ClearCycledCount();
	bool ContinuePlay();
	ImageAttribute imageAttribute;
	std::shared_ptr<ImageInfo> imageCache;

private:
	int m_nCurrentFrame;
	bool m_bPlaying;
	int m_nCycledCount;//播放次数
};

class UILIB_API StateImage
{
public:
	StateImage();

	void SetControl(Control* control) {	m_pControl = control; }
	Image& operator[](ControlStateType stateType) {	return m_stateImageMap[stateType]; }

	bool HasHotImage();
	bool PaintStatusImage(IRenderContext* pRender, ControlStateType stateType, const ui::string& sImageModify = _T(""));
	Image* GetEstimateImage();
	const Image* GetStatusImage(ControlStateType stateType) const;
	void ClearCache();

private:
	Control* m_pControl;
	std::map<ControlStateType, Image> m_stateImageMap;
};

class UILIB_API StateImageMap
{
public:
	StateImageMap()	{ }

	void SetControl(Control* control);

	void SetImage(StateImageType stateImageType, ControlStateType stateType, const ui::string& strImagePath);
	ui::string GetImagePath(StateImageType stateImageType, ControlStateType stateType);

	bool HasHotImage();
	bool PaintStatusImage(IRenderContext* pRender, StateImageType stateImageType, ControlStateType stateType, const ui::string& sImageModify = _T(""));
	Image* GetEstimateImage(StateImageType stateImageType);
	const Image* GetStatusImage(StateImageType stateImageType, ControlStateType stateType) const;
	void ClearCache();

private:
	std::map<StateImageType, StateImage> m_stateImageMap;
};

class UILIB_API StateColorMap
{
public:
	StateColorMap();

	void SetControl(Control* control);
	ui::string& operator[](ControlStateType stateType) { return m_stateColorMap[stateType]; }

	bool HasHotColor();
	void PaintStatusColor(IRenderContext* pRender, UiRect rcPaint, ControlStateType stateType);

private:
	Control* m_pControl;
	std::map<ControlStateType, ui::string> m_stateColorMap;
};

class UILIB_API StateSkin
{
public:
	StateSkin() : m_strName(), m_nX(0), m_nY(0), m_bCached(false) {};
	bool IsCached() { return m_bCached; }
	void SetCached(bool bCached) { m_bCached = bCached; }
	bool HasStateImage(ControlStateType st) { return m_stateImageMap.find(st) != m_stateImageMap.end(); };
	Image* GetStatusImage(ControlStateType st);
	ui::string SkinKey();
	bool PaintStatusImage(Control* pControl, IRenderContext* pRender, ControlStateType stateType);
	void ClearCache();
public:
	ui::string m_strName;
	int m_nX;
	int m_nY;
	// 状态缓存
	std::map<ControlStateType, Image> m_stateImageMap;
private:
	bool m_bCached;
};

} // namespace ui

#endif // UI_CORE_IMAGEDECODE_H_
