#include "StdAfx.h"
#include <zmouse.h>
#include <shlwapi.h>
#include "Utils/UnZip.h"

namespace ui
{

ui::string GlobalManager::m_pStrResourcePath;
std::vector<Window*> GlobalManager::m_aPreMessages;
std::map<ui::string, std::unique_ptr<WindowBuilder>> GlobalManager::m_builderMap;
CreateControlCallback GlobalManager::m_createControlCallback;

GlobalManager::MapStringToImagePtr GlobalManager::m_mImageHash;
std::map<ui::string, DWORD> GlobalManager::m_mapTextColor;
std::map<ui::string, ui::string> GlobalManager::m_mGlobalClass;
std::map<ui::string, TFontInfo*> GlobalManager::m_mCustomFonts;
ui::string GlobalManager::m_sDefaultFontId;

short GlobalManager::m_H = 180;
short GlobalManager::m_S = 100;
short GlobalManager::m_L = 100;

ui::string GlobalManager::m_strDefaultFontName;
ui::string GlobalManager::m_strDefaultDisabledColor = _T("textdefaultdisablecolor");
ui::string GlobalManager::m_strDefaultFontColor = _T("textdefaultcolor");
DWORD GlobalManager::m_dwDefaultLinkFontColor = 0xFF0000FF;
DWORD GlobalManager::m_dwDefaultLinkHoverFontColor = 0xFFD3215F;
DWORD GlobalManager::m_dwDefaultSelectedBkColor = 0xFFBAE4FF;

std::unique_ptr<IRenderFactory> GlobalManager::m_renderFactory;

static ULONG_PTR g_gdiplusToken;
static Gdiplus::GdiplusStartupInput g_gdiplusStartupInput;
static HZIP g_hzip = NULL;
static std::unique_ptr<lunasvg::ITreeBuilder> g_svgTreeBuilder;

void GlobalManager::Startup(const ui::string& strResourcePath, const CreateControlCallback& callback, bool bAdaptDpi, const ui::string& theme, const ui::string& language)
{
	m_renderFactory = std::make_unique<RenderFactory_GdiPlus>();
	GlobalManager::SetResourcePath(strResourcePath + theme);
	m_createControlCallback = callback;

	// 适配DPI
	if (bAdaptDpi) {
		DpiManager::GetInstance()->SetAdaptDPI();
		DpiManager::GetInstance()->SetScale(DpiManager::GetMainMonitorDPI());
	}

	// 解析全局资源信息
	LoadGlobalResource();

	// 加载多语言文件，如果使用了资源压缩包则从内存中加载语言文件
	if (g_hzip) {
		HGLOBAL hGlobal = GetData(strResourcePath + language + _T("\\gdstrings.ini"));
		if (hGlobal) {
			ui::MutiLanSupport::GetInstance()->LoadStringTable(hGlobal);
			GlobalFree(hGlobal);
		}
	}
	else {
		MutiLanSupport::GetInstance()->LoadStringTable(strResourcePath + language + _T("\\gdstrings.ini"));
	}

	GdiplusStartup(&g_gdiplusToken, &g_gdiplusStartupInput, NULL);
	// Boot Windows Common Controls (for the ToolTip control)
	::InitCommonControls();
}

void GlobalManager::Shutdown()
{
	if (g_hzip)
	{
		CloseZip(g_hzip);
		g_hzip = NULL;
	}
	m_renderFactory.reset();
	RemoveAllFonts();
	Gdiplus::GdiplusShutdown(g_gdiplusToken);
}

ui::string GlobalManager::GetCurrentPath()
{
	TCHAR tszModule[MAX_PATH + 1] = { 0 };
	::GetCurrentDirectory(MAX_PATH, tszModule);
	return tszModule;
}

ui::string GlobalManager::GetResourcePath()
{
	return m_pStrResourcePath;
}

ui::string GlobalManager::GetResourceSvgFile()
{
	return GetResourcePath() + _T("style.svg");
}

void GlobalManager::SetCurrentPath(const ui::string& strPath)
{
	::SetCurrentDirectory(strPath.c_str());
}

void GlobalManager::SetResourcePath(const ui::string& strPath)
{
	m_pStrResourcePath = strPath;
	if (m_pStrResourcePath.empty()) return;
	TCHAR cEnd = m_pStrResourcePath.at(m_pStrResourcePath.length() - 1);
	if (cEnd != _T('\\') && cEnd != _T('/')) m_pStrResourcePath += _T('\\');
}

void GlobalManager::LoadGlobalResource()
{
	ui::WindowBuilder dialog_builder;
	ui::Window paint_manager;
	dialog_builder.Create(_T("global.xml"), CreateControlCallback(), &paint_manager);
	//
	ui::string sFile = GetResourceSvgFile();
#ifdef _UNICODE	
	std::string sFileA;
	StringHelper::UnicodeToMBCS(sFile, sFileA);
	g_svgTreeBuilder = lunasvg::ITreeBuilder::parseFromFile(sFileA);
#else
	g_svgTreeBuilder = lunasvg::ITreeBuilder::parseFromFile(sFile);
#endif
	if (!g_svgTreeBuilder)
		::MessageBox(NULL, _T("加载svg样式表失败."), _T("错误"), MB_ICONERROR | MB_OK);
}

void GlobalManager::ReloadSkin(const ui::string& resourcePath)
{
	RemoveAllFonts();
	RemoveAllTextColors();
	RemoveAllClasss();
	RemoveAllImages();

	SetResourcePath(resourcePath);
	LoadGlobalResource();

	for (auto it = m_aPreMessages.begin(); it != m_aPreMessages.end(); it++) {
		(*it)->GetRoot()->Invalidate();
	}
}

ui::IRenderFactory* GlobalManager::GetRenderFactory()
{
	return m_renderFactory.get();
}

std::unique_ptr<ui::IRenderContext> GlobalManager::CreateRenderContext()
{
	std::unique_ptr<ui::IRenderContext> p;
	p.reset(m_renderFactory->CreateRenderContext());
	return p;
}

std::unique_ptr<ui::IPen> GlobalManager::CreatePen(DWORD color, int width /*= 1*/)
{
	std::unique_ptr<ui::IPen> p;
	p.reset(m_renderFactory->CreatePen(color, width));
	return p;
}

std::unique_ptr<ui::IBrush> GlobalManager::CreateBrush(DWORD color)
{
	std::unique_ptr<ui::IBrush> p;
	p.reset(m_renderFactory->CreateBrush(color));
	return p;
}

std::unique_ptr<ui::IMatrix> GlobalManager::CreateMatrix()
{
	std::unique_ptr<ui::IMatrix> p;
	p.reset(m_renderFactory->CreateMatrix());
	return p;
}

std::unique_ptr<ui::IPath> GlobalManager::CreatePath()
{
	std::unique_ptr<ui::IPath> p;
	p.reset(m_renderFactory->CreatePath());
	return p;
}

void GlobalManager::AddClass(const ui::string& strClassName, const ui::string& strControlAttrList)
{
	ASSERT(!strClassName.empty());
	ASSERT(!strControlAttrList.empty());
	m_mGlobalClass[strClassName] = strControlAttrList;
}

ui::string GlobalManager::GetClassAttributes(const ui::string& strClassName)
{
	auto it = m_mGlobalClass.find(strClassName);
	if (it != m_mGlobalClass.end()) {
		return it->second;
	}

	return _T("");
}

void GlobalManager::RemoveAllClasss()
{
	m_mGlobalClass.clear();
}

void GlobalManager::AddTextColor(const ui::string& strName, const ui::string& strValue)
{
	ui::string strColor = strValue.substr(1);
	LPTSTR pstr = NULL;
	DWORD dwBackColor = _tcstoul(strColor.c_str(), &pstr, 16);

	m_mapTextColor[strName] = dwBackColor;
}

DWORD GlobalManager::GetTextColor(const ui::string& strName)
{
	// 可以使用global.xml中提定义的颜色值或直接输入颜色码
	const auto& itDef = m_mapTextColor.find(strName);
	if (itDef != m_mapTextColor.end())
		return itDef->second;
	else if (!strName.empty() && strName.at(0) == _T('#'))
	{
		LPTSTR pstr = NULL;
		return _tcstoul(strName.substr(1).c_str(), &pstr, 16);
	}
	ASSERT(FALSE);
	return 0;
}

void GlobalManager::RemoveAllTextColors()
{
	m_mapTextColor.clear();
}

std::shared_ptr<ImageInfo> GlobalManager::IsImageCached(const ui::string& strImagePath, const ui::string& sGroupID)
{
	ui::string imageFullPath = StringHelper::ReparsePath(strImagePath) + sGroupID;
	std::shared_ptr<ImageInfo> sharedImage;
	auto it = m_mImageHash.find(imageFullPath);
	if (it != m_mImageHash.end()) {
		sharedImage = it->second.lock();
	}

	return sharedImage;
}

std::shared_ptr<ImageInfo> GlobalManager::AddImageCached(const std::shared_ptr<ImageInfo>& sharedImage)
{
	ASSERT(FALSE); // 暂未使用,应考虑GroupID在处理
	ASSERT(m_mImageHash[sharedImage->sImageFullPath].expired() == true);
	m_mImageHash[sharedImage->sImageFullPath] = sharedImage;
	sharedImage->SetCached(true);
	return sharedImage;
}

void GlobalManager::RemoveFromImageCache(const ui::string& imageFullPath)
{
	auto it = m_mImageHash.find(imageFullPath);
	if (it != m_mImageHash.end()) {
		m_mImageHash.erase(it);
	}
	else {
		ASSERT(FALSE);
	}
}

void GlobalManager::OnImageInfoDestroy(ImageInfo* pImageInfo)
{
	ASSERT(pImageInfo);
	if (pImageInfo && pImageInfo->IsCached()) {
		if (!pImageInfo->sImageFullPath.empty()) {
			GlobalManager::RemoveFromImageCache(pImageInfo->sImageFullPath + pImageInfo->sImageGroupID);
		}
	}
	delete pImageInfo;
}

std::shared_ptr<ImageInfo> GlobalManager::GetImage(const ui::string& bitmap, const ui::string& sGroupID, double dScale)
{
	ui::string imageFullPath = StringHelper::ReparsePath(bitmap);
	if (IsUseZip())
	{
		imageFullPath = GetZipFilePath(imageFullPath);
	}
	static ui::string sGlobal = GlobalManager::GetResourceSvgFile();
	std::shared_ptr<ImageInfo> sharedImage;
	auto strImageKey = imageFullPath + sGroupID;
	auto it = m_mImageHash.find(strImageKey);
	if (it == m_mImageHash.end()) {
		std::unique_ptr<ImageInfo> data;
		if (IsUseZip())
		{
			data = ImageInfo::LoadImage(GetData(imageFullPath), imageFullPath, sGroupID, dScale);
		}
		if (!data)
		{
			if (sGroupID.empty() || sGlobal.compare(imageFullPath) != 0)
				data = ImageInfo::LoadImage(imageFullPath, sGroupID, dScale);
			else
				data = ImageInfo::LoadImageGroup(g_svgTreeBuilder, sGroupID, dScale);
		}
		if (!data) return sharedImage;
		sharedImage.reset(data.release(), &OnImageInfoDestroy);
		m_mImageHash[strImageKey] = sharedImage;
		sharedImage->SetCached(true);
	}
	else {
		sharedImage = it->second.lock();
	}

	return sharedImage;
}

ui::string GlobalManager::GetDefaultFontName()
{
	return m_strDefaultFontName;
}

void GlobalManager::RemoveAllImages()
{
	for (auto it = m_aPreMessages.begin(); it != m_aPreMessages.end(); it++) {
		(*it)->GetRoot()->ClearImageCache();
	}

	m_mImageHash.clear();
}

HFONT GlobalManager::AddFont(const ui::string& strFontId, const ui::string& strFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bDefault)
{
	ui::string strNewFontId = strFontId;
	if (strNewFontId.empty())
	{
#ifdef _UNICODE	
		strNewFontId = std::to_wstring(m_mCustomFonts.size());
#else
		strNewFontId = std::to_string(m_mCustomFonts.size());
#endif
	}

	auto iter = m_mCustomFonts.find(strNewFontId);
	ASSERT(iter == m_mCustomFonts.end());

	static bool bOsOverXp = IsWindowsVistaOrGreater();
	ui::string fontName = strFontName;
	if (fontName == _T("system")) {
		fontName = bOsOverXp ? _T("微软雅黑") : _T("新宋体");
	}

	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcscpy(lf.lfFaceName, fontName.c_str());
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -DpiManager::GetInstance()->ScaleInt(nSize);
	if (bBold) lf.lfWeight += FW_BOLD;
	if (bUnderline) lf.lfUnderline = TRUE;
	if (bItalic) lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	if (hFont == NULL) return NULL;

	TFontInfo* pFontInfo = new TFontInfo;
	if (!pFontInfo) return false;
	pFontInfo->hFont = hFont;
	pFontInfo->sFontName = fontName;
	pFontInfo->iSize = nSize;
	pFontInfo->bBold = bBold;
	pFontInfo->bUnderline = bUnderline;
	pFontInfo->bItalic = bItalic;
	::ZeroMemory(&pFontInfo->tm, sizeof(pFontInfo->tm));

	m_mCustomFonts.insert(std::make_pair(strNewFontId, pFontInfo));

	if (bDefault) m_sDefaultFontId = strNewFontId;

	return hFont;
}

TFontInfo* GlobalManager::GetTFontInfo(const ui::string& strFontId)
{
	ui::string strFindId = strFontId;
	if (strFindId.empty())
	{
		ASSERT(!m_sDefaultFontId.empty());
		strFindId = m_sDefaultFontId;
	}

	auto iter = m_mCustomFonts.find(strFindId);
	ASSERT(iter != m_mCustomFonts.end());

	TFontInfo* pFontInfo = static_cast<TFontInfo*>(iter->second);
	return pFontInfo;
}

HFONT GlobalManager::GetFont(const ui::string& strFontId)
{
	TFontInfo* pFontInfo = GetTFontInfo(strFontId);
	if (pFontInfo)
		return pFontInfo->hFont;
	return nullptr;
}

HFONT GlobalManager::GetFont(const ui::string& strFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	for (auto it = m_mCustomFonts.begin(); it != m_mCustomFonts.end(); it++) {
		auto pFontInfo = it->second;
		if (pFontInfo->sFontName == strFontName && pFontInfo->iSize == nSize &&
			pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic)
			return pFontInfo->hFont;
	}
	return NULL;
}

TFontInfo* GlobalManager::GetFontInfo(const ui::string& strFontId, HDC hDcPaint)
{
	TFontInfo* pFontInfo = GetTFontInfo(strFontId);
	if (pFontInfo->tm.tmHeight == 0) {
		HFONT hOldFont = (HFONT) ::SelectObject(hDcPaint, pFontInfo->hFont);
		::GetTextMetrics(hDcPaint, &pFontInfo->tm);
		::SelectObject(hDcPaint, hOldFont);
	}
	return pFontInfo;
}

TFontInfo* GlobalManager::GetFontInfo(HFONT hFont, HDC hDcPaint)
{
	for (auto it = m_mCustomFonts.begin(); it != m_mCustomFonts.end(); it++) {
		auto pFontInfo = it->second;
		if (pFontInfo->hFont == hFont) {
			if (pFontInfo->tm.tmHeight == 0) {
				HFONT hOldFont = (HFONT) ::SelectObject(hDcPaint, pFontInfo->hFont);
				::GetTextMetrics(hDcPaint, &pFontInfo->tm);
				::SelectObject(hDcPaint, hOldFont);
			}
			return pFontInfo;
		}
	}

	ASSERT(FALSE);
	return NULL;
}

bool GlobalManager::FindFont(HFONT hFont)
{
	for (auto it = m_mCustomFonts.begin(); it != m_mCustomFonts.end(); it++) {
		auto pFontInfo = it->second;
		if (pFontInfo->hFont == hFont)
			return true;
	}
	return false;
}

bool GlobalManager::FindFont(const ui::string& strFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
	for (auto it = m_mCustomFonts.begin(); it != m_mCustomFonts.end(); it++) {
		auto pFontInfo = it->second;
		if (pFontInfo->sFontName == strFontName && pFontInfo->iSize == nSize &&
			pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic)
			return true;
	}
	return false;
}

bool GlobalManager::RemoveFontAt(const ui::string& strFontId)
{
	auto iter = m_mCustomFonts.find(strFontId);
	if (iter == m_mCustomFonts.end()) return false;

	TFontInfo* pFontInfo = static_cast<TFontInfo*>(iter->second);
	::DeleteObject(pFontInfo->hFont);
	delete pFontInfo;

	m_mCustomFonts.erase(iter);

	return true;
}

void GlobalManager::RemoveAllFonts()
{
	for (auto it = m_mCustomFonts.begin(); it != m_mCustomFonts.end(); it++) {
		auto pFontInfo = it->second;
		::DeleteObject(pFontInfo->hFont);
		delete pFontInfo;
	}
	m_mCustomFonts.clear();
}

ui::string GlobalManager::GetDefaultDisabledTextColor()
{
	return m_strDefaultDisabledColor;
}

void GlobalManager::SetDefaultDisabledTextColor(const ui::string& strColor)
{
	m_strDefaultDisabledColor = strColor;
}

ui::string GlobalManager::GetDefaultTextColor()
{
	return m_strDefaultFontColor;
}

void GlobalManager::SetDefaultTextColor(const ui::string& strColor)
{
	m_strDefaultFontColor = strColor;
}

DWORD GlobalManager::GetDefaultLinkFontColor()
{
	return m_dwDefaultLinkFontColor;
}

void GlobalManager::SetDefaultLinkFontColor(DWORD strColor)
{
	m_dwDefaultLinkFontColor = strColor;
}

DWORD GlobalManager::GetDefaultLinkHoverFontColor()
{
	return m_dwDefaultLinkHoverFontColor;
}

void GlobalManager::SetDefaultLinkHoverFontColor(DWORD dwColor)
{
	m_dwDefaultLinkHoverFontColor = dwColor;
}

DWORD GlobalManager::GetDefaultSelectedBkColor()
{
	return m_dwDefaultSelectedBkColor;
}

void GlobalManager::SetDefaultSelectedBkColor(DWORD dwColor)
{
	m_dwDefaultSelectedBkColor = dwColor;
}

Box* GlobalManager::CreateBox(const ui::string& strXmlPath, CreateControlCallback callback)
{
	WindowBuilder builder;
	Box* box = builder.Create(strXmlPath.c_str(), callback);
	ASSERT(box);

	return box;
}

Box* GlobalManager::CreateBoxWithCache(const ui::string& strXmlPath, CreateControlCallback callback)
{
	Box* box = nullptr;
	auto it = m_builderMap.find(strXmlPath);
	if (it == m_builderMap.end()) {
		WindowBuilder* builder = new WindowBuilder();
		box = builder->Create(strXmlPath.c_str(), callback);
		if (box) {
			m_builderMap[strXmlPath].reset(builder);
		}
		else {
			ASSERT(FALSE);
		}
	}
	else {
		box = it->second->Create(callback);
	}

	return box;
}

void GlobalManager::FillBox(Box* pUserDefinedBox, const ui::string& strXmlPath, CreateControlCallback callback)
{
	WindowBuilder winBuilder;
	Box* box = winBuilder.Create(strXmlPath.c_str(), callback, pUserDefinedBox->GetWindow(), nullptr, pUserDefinedBox);
	ASSERT(box);

	return;
}

void GlobalManager::FillBoxWithCache(Box* pUserDefinedBox, const ui::string& strXmlPath, CreateControlCallback callback)
{
	Box* box = nullptr;
	auto it = m_builderMap.find(strXmlPath);
	if (it == m_builderMap.end()) {
		WindowBuilder* winBuilder = new WindowBuilder();
		box = winBuilder->Create(strXmlPath.c_str(), callback, pUserDefinedBox->GetWindow(), nullptr, pUserDefinedBox);
		if (box) {
			m_builderMap[strXmlPath].reset(winBuilder);
		}
		else {
			ASSERT(FALSE);
		}
	}
	else {
		box = it->second->Create(callback, pUserDefinedBox->GetWindow(), nullptr, pUserDefinedBox);
	}

	return;
}

Control* GlobalManager::CreateControl(const ui::string& strControlName)
{
	if (m_createControlCallback) {
		return m_createControlCallback(strControlName);
	}

	return nullptr;
}

bool GlobalManager::IsUseZip()
{
	return g_hzip != NULL;
}

bool GlobalManager::OpenResZip(LPCTSTR  resource_name, LPCTSTR  resource_type, const ui::string& password)
{
	HRSRC   rsc = FindResource(NULL, resource_name, resource_type);
	HGLOBAL resource = LoadResource(NULL, rsc);
	DWORD   size = SizeofResource(NULL, rsc);
	if (resource && size > 0)
	{

	}

	if (g_hzip)
	{
		CloseZip(g_hzip);
		g_hzip = NULL;
	}
#ifdef _UNICODE	
	std::string strPsd;
	StringHelper::UnicodeToMBCS(password, strPsd);
	g_hzip = OpenZip(resource, size, strPsd.c_str());
#else
	g_hzip = OpenZip(resource, size, password.c_str());
#endif
	return g_hzip != NULL;
}

bool GlobalManager::OpenResZip(const ui::string& path, const ui::string& password)
{
	if (g_hzip)
	{
		CloseZip(g_hzip);
		g_hzip = NULL;
	}
#ifdef _UNICODE	
	std::string strPsd;
	StringHelper::UnicodeToMBCS(password, strPsd);
	g_hzip = OpenZip(path.c_str(), strPsd.c_str());
#else
	g_hzip = OpenZip(path.c_str(), password.c_str());
#endif
	return g_hzip != NULL;
}

HGLOBAL GlobalManager::GetData(const ui::string& path)
{
	HGLOBAL hGlobal = NULL;
	ui::string file_path = GetZipFilePath(path);
	if (g_hzip && !file_path.empty())
	{
		ZIPENTRY ze;
		int i = 0;
		if (FindZipItem(g_hzip, file_path.c_str(), true, &i, &ze) == ZR_OK)
		{
			if (ze.index >= 0)
			{
				hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, ze.unc_size);
				if (hGlobal)
				{
					TCHAR *pData = (TCHAR*)GlobalLock(hGlobal);
					if (pData)
					{
						ZRESULT res = UnzipItem(g_hzip, ze.index, pData, ze.unc_size);
						GlobalUnlock(hGlobal);
						if (res != ZR_OK)
						{
							GlobalFree(hGlobal);
							hGlobal = NULL;
						}
					}
					else
					{
						GlobalFree(hGlobal);
						hGlobal = NULL;
					}
				}
			}
		}
	}

	return hGlobal;
}

ui::string GlobalManager::GetZipFilePath(const ui::string& path)
{
	ui::string file_path = path;
	StringHelper::ReplaceAll(_T("\\"), _T("/"), file_path);
	StringHelper::ReplaceAll(_T("//"), _T("/"), file_path);
	for (unsigned int i = 0; i < file_path.size();)
	{
		bool start_node = false;
		if (i == 0 || file_path.at(i - 1) == _T('/'))
		{
			start_node = true;
		}
		TCHAR wch = file_path.at(i);
		if (start_node && wch == _T('/'))//"//"
		{
			file_path.erase(i, 1);
			continue;
		}
		if (start_node && wch == _T('.'))
		{
			if (i + 1 < file_path.size() && file_path.at(i + 1) == _T('/'))// "./"
			{
				file_path.erase(i, 2);
				continue;
			}
			else if (i + 2 < file_path.size() && file_path.at(i + 1) == _T('.') && file_path.at(i + 2) == _T('/'))// "../"
			{
				file_path.erase(i, 2);
				int i_erase = i - 2;
				if (i_erase < 0)
				{
					ASSERT(0);
				}
				while (i_erase > 0 && file_path.at(i_erase) != _T('/'))
					i_erase--;
				file_path.erase(i_erase, i - i_erase);
				i = i_erase;
				continue;
			}
		}
		i++;
	}
	return file_path;
}

bool GlobalManager::ImageCacheKeyCompare::operator()(const ui::string& key1, const ui::string& key2) const
{
	int nLen1 = (int)key1.length();
	int nLen2 = (int)key2.length();
	if (nLen1 != nLen2)
		return nLen1 < nLen2;

	LPCTSTR pStr1Begin = key1.c_str();
	LPCTSTR pStr2Begin = key2.c_str();
	LPCTSTR pStr1End = pStr1Begin + nLen1;
	LPCTSTR pStr2End = pStr2Begin + nLen2;

	// 逆向比较
	while (--pStr1End >= pStr1Begin && --pStr2End >= pStr2Begin && *pStr1End == *pStr2End);

	// 两个串都已经比光了，那么肯定相等，返回false
	if (pStr1End < pStr1Begin) {
		return false;
	}
	return *pStr1End < *pStr2End;
}

} // namespace ui
