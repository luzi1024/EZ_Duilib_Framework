#ifndef UI_CORE_WINDOWHELPER_H_
#define UI_CORE_WINDOWHELPER_H_

#pragma once

#include "Image.h"
#include "Window.h"

namespace ui 
{
class SkinBox;
/**
* @brief 全局属性管理工具类
* 用于管理一些全局属性的工具类，包含全局样式（global.xml）和语言设置等
*/
class UILIB_API GlobalManager
{
public:
	/**
	 * @brief 初始化全局设置函数
	 * @param[in] strResourcePath 资源路径位置
	 * @param[in] callback 创建自定义控件时的全局回调函数
	 * @param[in] bAdaptDpi 是否启用 DPI 适配
	 * @param[in] theme 主题目录名，默认为 themes\\default
	 * @param[in] language 使用语言，默认为 lang\\zh_CN
	 * @return 无
	 */
	static void Startup(const ui::string& strResourcePath, const CreateControlCallback& callback, bool bAdaptDpi, const ui::string& theme = _T("themes\\default"), const ui::string& language = _T("lang\\zh_CN"));

	/**
	 * @brief 释放资源
	 * @return 无
	 */
	static void Shutdown();

	/**
	 * @brief 获取当前程序所在目录
	 * @return 返回当前程序所在目录
	 */
	static ui::string GetCurrentPath();

	/**
	 * @brief 获取当前资源所在目录
	 * @return 返回当前资源所在目录
	 */
	static ui::string GetResourcePath();

	/**
	 * @brief 获取当前svg资源文件路径
	 * @return 返回当前svg资源文件路径
	 */
	static ui::string GetResourceSvgFile();
	/**
	 * @brief 设置程序当前目录
	 * @param[in] strPath 要设置的路径
	 * @return 无
	 */
	static void SetCurrentPath(const ui::string& strPath);

	/**
	 * @brief 设置皮肤资源所在目录
	 * @param[in] strPath 要设置的路径
	 * @return 无
	 */
	static void SetResourcePath(const ui::string& strPath);

	/**
	 * @brief 重新加载皮肤资源
	 * @param[in] resourcePath 资源路径
	 * @return 无
	 */
	static void ReloadSkin(const ui::string& resourcePath);

	/**
	 * @brief 获取绘制接口类对象
	 * @return 返回接口类对象指针
	 */
	static IRenderFactory* GetRenderFactory();

	/**
	 * @brief 创建全局的绘制上下文区域
	 * @return 返回绘制区域对象
	 */
	static std::unique_ptr<IRenderContext> CreateRenderContext();

	/**
	 * @brief 创建一个画笔
	 * @param[in] color 画笔颜色
	 * @param[in] width 画笔宽度
	 * @return 返回画笔对象
	 */
	static std::unique_ptr<IPen> CreatePen(DWORD color, int width = 1);

	/**
	 * @brief 创建一个画刷
	 * @param[in] color 画刷颜色
	 * @return 返回画刷对象
	 */
	static std::unique_ptr<IBrush> CreateBrush(DWORD color);

	/**
	 * @brief 创建一个矩阵
	 * @return 返回矩阵对象
	 */
	static std::unique_ptr<IMatrix> CreateMatrix();

	/**
	 * @brief 创建一个绘制路径
	 * @return 返回绘制路径对象
	 */
	static std::unique_ptr<IPath> CreatePath();

	/**
	 * @brief 添加一个全局 class 属性
	 * @param[in] strClassName 全局 class 名称
	 * @param[in] strControlAttrList 属性列表，需要做 XML 转义
	 * @return 无
	 */
	static void AddClass(const ui::string& strClassName, const ui::string& strControlAttrList);

	/**
	 * @brief 获取一个全局 class 属性的值
	 * @param[in] strClassName 全局 class 名称
	 * @return 返回字符串形式的 class 属性值
	 */
	static ui::string GetClassAttributes(const ui::string& strClassName);

	/**
	 * @brief 从全局属性中删除所有 class 属性
	 * @return 返回绘制区域对象
	 */
	static void RemoveAllClasss();

	/**
	 * @brief (NEW)添加一个全局 skin 属性
	 * @param[in] strSkinName 全局 skin 名称
	 * @param[in] strControlAttrList 属性列表，需要做 XML 转义
	 * @return 无
	 */
	static void AddSkin(SkinBox* pSkinBox);

	/**
	 * @brief (NEW)获取图片 StateSkin 对象
	 * @param[in] bitmap 图片路径
	 * @return 返回图片 ImageInfo 对象的智能指针
	 */
	static std::shared_ptr<StateSkin> GetStateSkin(const ui::string& strSkinName, const UiRect& rc);

	/**
	 * @brief (NEW)通过SkinBox生成适配的图片 StateSkin 对象
	 * @param[in] skinBox 模板对象
	 * @param[in] rc 适配尺寸
	 * @return 返回图片 StateSkin 对象的智能指针
	 */
	static std::unique_ptr<StateSkin> ProductSkin(std::unique_ptr<SkinBox>& skinBox, const UiRect& rc);

	/**
	 * @brief (NEW)添加一个全局窗口定义
	 * @param[in] sName 全局 窗口 名称
	 * @param[in] sData  XML 数据
	 * @return 无
	 */
	static void AddWindowDefine(const ui::string& sName, const ui::string& sData);

	/**
	 * @brief (NEW)添加一个全局窗口定义
	 * @param[in] sName 全局 窗口 名称
	 * @param[in] sData  XML 数据
	 * @return 无
	 */
	static bool GetWindowDefine(const ui::string& sName, ui::string& sData);

	/**
	 * @brief (NEW)附着一个全局窗口定义
	 * @param[in] sName 全局 窗口 名称
	 * @param[in] pParentBox  父节点
	 * @return 成功/失败
	 */
	static bool IncludeWindowControl(Box* pParentBox, const ui::string& sName, CreateControlCallback callback = CreateControlCallback());

	/**
	 * @brief (NEW)附着一个全局窗口定义
	 * @param[in] sName 全局 窗口 名称
	 * @param[in] pParentBox  父节点
	 * @return 成功/失败
	 */
	static Box* CreateWindowControl(const ui::string& sName, CreateControlCallback callback = CreateControlCallback());

	/**
	 * @brief 从缓存中一处一个Skin
	 * @param[in] imageFullPath 图片路径
	 * @return 无
	 */
	static void RemoveFromSkinCache(const ui::string& skey);

	/**
	 * @brief 添加一个全局颜色值提供程序使用
	 * @param[in] strName 颜色名称（如 white）
	 * @param[in] strValue 颜色具体数值（如 #FFFFFFFF）
	 * @return 无
	 */
	static void AddTextColor(const ui::string& strName, const ui::string& strValue);

	/**
	 * @brief 根据名称获取一个颜色的具体数值
	 * @param[in] strName 要获取的颜色名称
	 * @return 返回 DWORD 格式的颜色描述值
	 */
	static DWORD GetTextColor(const ui::string& strName);

	/**
	 * @brief 删除所有全局颜色属性
	 * @return 无
	 */
	static void RemoveAllTextColors();

	/**
	 * @brief 检查指定图片是否已经被缓存
	 * @param[in] bitmap 图片路径
	 * @return 如果已经被缓存，则返回 ImageInfo 的智能指针对象
	 */
	static std::shared_ptr<ImageInfo> IsImageCached(const ui::string& bitmap, const ui::string& sGroupID);

	/**
	 * @brief 添加一个图片到缓存中
	 * @param[in] shared_image
	 * @return 返回被缓存的 ImageInfo 智能指针对象
	 */
	static std::shared_ptr<ImageInfo> AddImageCached(const std::shared_ptr<ImageInfo>& shared_image);

	/**
	 * @brief 从缓存中一处一个图片
	 * @param[in] imageFullPath 图片路径
	 * @return 无
	 */
	static void RemoveFromImageCache(const ui::string& imageFullPath);

	/**
	 * @brief 图片被销毁的回调
	 * @param[in] pImageInfo 图片对应的 ImageInfo 对象
	 * @return 无
	 */
	static void OnImageInfoDestroy(ImageInfo *pImageInfo);

	/**
	 * @brief 获取图片 ImageInfo 对象
	 * @param[in] bitmap 图片路径
	 * @return 返回图片 ImageInfo 对象的智能指针
	 */
	static std::shared_ptr<ImageInfo> GetImage(const ui::string& bitmap, const ui::string& sGroupID = _T(""), double dScale = 1.0);

	/**
	 * @brief 从缓存中删除所有图片
	 * @return 无
	 */
	static void RemoveAllImages();

	/**
	 * @brief 获取默认字体名称
	 * @return 字体名称
	 */
	static ui::string GetDefaultFontName();

	/**
	 * @brief 添加一个字体
	 * @param[in] strFontId 指定字体的ID标记
	 * @param[in] strFontName 字体名称
	 * @param[in] nSize 字体大小
	 * @param[in] bBold 是否粗体
	 * @param[in] bUnderline 是否有下划线
	 * @param[in] bItalic 是否倾斜
	 * @param[in] bDefault 是否默认
	 * @return 返回字体的 HFONT 句柄
	 */
	static HFONT AddFont(const ui::string& strFontId, const ui::string& strFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bDefault);

	/**
	 * @brief 根据索引返回一个字体信息
	 * @param[in] strFontId 字体ID
	 * @return 返回字体的 TFontInfo 信息
	 */
	static TFontInfo* GetTFontInfo(const ui::string& strFontId);

	/**
	 * @brief 根据字体ID返回一个字体对象
	 * @param[in] strFontId 字体ID
	 * @return 返回字体的 HFONT 句柄
	 */
	static HFONT GetFont(const ui::string& strFontId);
	/**
	 * @brief 根据字体属性获取字体对象
	 * @param[in] strFontName 字体名称
	 * @param[in] nSize 字体大小
	 * @param[in] bBold 是否粗体
	 * @param[in] bUnderline 是否有下划线
	 * @param[in] bItalic 是否倾斜
	 * @return 返回字体的 HFONT 句柄
	 */
	static HFONT GetFont(const ui::string& strFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);

	/**
	 * @brief 获取字体信息
	 * @param[in] strFontId 字体ID
	 * @param[in] hDcPaint 设备句柄
	 * @return 返回字体的 TFontInfo 信息
	 */
	static TFontInfo* GetFontInfo(const ui::string& strFontId, HDC hDcPaint);

	/**
	 * @brief 获取字体信息
	 * @param[in] hFont 字体对象句柄
	 * @param[in] hDcPaint 设备句柄
	 * @return 返回字体的 TFontInfo 信息
	 */
	static TFontInfo* GetFontInfo(HFONT hFont, HDC hDcPaint);

	/**
	 * @brief 根据字体对象查找指定字体是否存在
	 * @param[in] hFont 字体对象句柄
	 * @return 返回是否存在
	 *     @retval true 存在
	 *     @retval false 不存在
	 */
	static bool FindFont(HFONT hFont);

	/**
	 * @brief 根据字体信息查找字体是否存在
	 * @param[in] strFontName 字体名称
	 * @param[in] nSize 字体大小
	 * @param[in] bBold 是否粗体
	 * @param[in] bUnderline 是否有下划线
	 * @param[in] bItalic 是否倾斜
	 * @return 返回是否存在
	 *     @retval true 存在
	 *     @retval false 不存在
	 */
	static bool FindFont(const ui::string& strFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);

	/**
	 * @brief 根据字体索引删除字体
	 * @param[in] strFontId 字体ID
	 * @return 返回是否删除成功
	 *     @retval true 删除成功
	 *     @retval false 字体不存在或删除失败
	 */
	static bool RemoveFontAt(const ui::string& strFontId);

	/**
	 * @brief 删除所有字体
	 * @return 无
	 */
	static void RemoveAllFonts();

	/**
	 * @brief 获取默认禁用状态下字体颜色
	 * @return 默认禁用状态颜色的字符串表示，对应 global.xml 中指定颜色值
	 */
	static ui::string GetDefaultDisabledTextColor();

	/**
	 * @brief 设置默认禁用状态下的字体颜色
	 * @param[in] dwColor 字体颜色字符串值（如 white）
	 * @return 无
	 */
	static void SetDefaultDisabledTextColor(const ui::string& strColor);

	/**
	 * @brief 获取默认字体颜色
	 * @return 默认字体颜色字符串表示，对应 global.xml 中指定颜色值
	 */
	static ui::string GetDefaultTextColor();

	/**
	 * @brief 设置默认字体颜色
	 * @param[in] 字体颜色的字符串值，对应 global.xml 中指定颜色值
	 * @return 无
	 */
	static void SetDefaultTextColor(const ui::string& strColor);

	/**
	 * @brief 获取默认链接字体颜色
	 * @return 链接字体颜色 DWORD 格式的色值
	 */
	static DWORD GetDefaultLinkFontColor();

	/**
	 * @brief 设置默认链接字体颜色
	 * @param[in] dwColor 默认链接字体颜色
	 * @return 无
	 */
	static void SetDefaultLinkFontColor(DWORD dwColor);

	/**
	 * @brief 获取焦点链接的默认字体颜色
	 * @return 返回焦点链接的默认字体颜色
	 */
	static DWORD GetDefaultLinkHoverFontColor();

	/**
	 * @brief 设置焦点链接的默认字体颜色
	 * @param[in] dwColor 焦点链接的默认字体颜色
	 * @return 无
	 */
	static void SetDefaultLinkHoverFontColor(DWORD dwColor);

	/**
	 * @brief 获取默认选择状态字体颜色
	 * @return 返回默认选择状态的字体颜色
	 */
	static DWORD GetDefaultSelectedBkColor();

	/**
	 * @brief 设置默认选择状态的字体颜色
	 * @param[in] dwColor 字体颜色值
	 * @return 无
	 */
	static void SetDefaultSelectedBkColor(DWORD dwColor);

	/**
	 * @brief 根据 XML 创建一个 Box
	 * @param[in] strXmlPath XML 文件路径
	 * @param[in] callback 自定义控件的回调处理函数
	 * @return 指定布局模块的对象指针
	 */
	static Box* CreateBox(const ui::string& strXmlPath, CreateControlCallback callback = CreateControlCallback());

	/**
	 * @brief 根据 XML 在缓存中查找指定 Box，如果没有则创建
	 * @param[in] strXmlPath XML 文件路径
	 * @param[in] callback 自定义控件的回调处理函数
	 * @return 指定布局模块的对象指针
	 */
	static Box* CreateBoxWithCache(const ui::string& strXmlPath, CreateControlCallback callback = CreateControlCallback());

	/**
	 * @brief 使用 XML 填充指定 Box
	 * @param[in] pUserDefinedBox 要填充的 box 指针
	 * @param[in] strXmlPath XML 文件路径
	 * @param[in] callback 自定义控件的回调处理函数
	 * @return 无
	 */
	static void FillBox(Box* pUserDefinedBox, const ui::string& strXmlPath, CreateControlCallback callback = CreateControlCallback());

	/**
	 * @brief 使用构建过的缓存填充指定 Box，如果没有则重新构建
	 * @param[in] pUserDefinedBox 要填充的 box 指针
	 * @param[in] strXmlPath XML 文件路径
	 * @param[in] callback 自定义控件的回调处理函数
	 * @return 无
	 */
	static void FillBoxWithCache(Box* pUserDefinedBox, const ui::string& strXmlPath, CreateControlCallback callback = CreateControlCallback());

	/**
	 * @brief 自定义控件创建后的全局回调函数
	 * @param[in] strControlName 自定义控件名称
	 * @return 返回一个自定义控件的对象指针
	 */
	static Control* CreateControl(const ui::string& strControlName);

	/**
	 * @brief 判断当前是否使用了 zip 压缩包
	 * @return 返回 true 表示使用了 zip 压缩包作为资源，false 为普通目录模式
	 */
	static bool IsUseZip();

	/**
	 * @brief 打开一个内存压缩包资源
	 * @param[in] resource_name 资源名称，由 MAKEINTRESOURCE 宏生成
	 * @param[in] resource_type 资源类型，自定义导入的资源类型名称
	 * @param[in] password 压缩包密码
	 * @return 返回 true 打开成功，返回 false 为打开失败
	 */
	static bool OpenResZip(LPCTSTR  resource_name, LPCTSTR  resource_type, const ui::string& password);

	/**
	 * @brief 打开一个本地文件压缩包资源
	 * @param[in] path 压缩包文件路径
	 * @param[in] password 压缩包密码
	 * @return 返回 true 打开成功，返回 false 为打开失败
	 */
	static bool OpenResZip(const ui::string& path, const ui::string& password);

	/**
	 * @brief 获取压缩包中的内容到内存
	 * @param[in] path 要获取的文件的路径
	 * @return 返回文件的内存地址
	 */
	static HGLOBAL GetData(const ui::string& path);

	/**
	 * @brief 获取文件在压缩包中的位置
	 * @param[in] path 要获取的文件路径
	 * @return 返回在压缩包中的文件位置
	 */
	static ui::string GetZipFilePath(const ui::string& path);


	static std::unique_ptr<lunasvg::ITreeBuilder>& GetGlobalSvgTreeBuilder();
private:
	/**
	 * @brief 加载全局资源
	 * @return 无
	 */
	static void LoadGlobalResource();

private:
	static std::unique_ptr<IRenderFactory> m_renderFactory;
	class ImageCacheKeyCompare
	{
	public:
		// 这个比较函数不是比较字典序的，而是按照以下法则：
		// 首先比较，长度小的更小
		// 再逆向比较，可以认为是逆向字典序
		bool operator()(const ui::string& key1, const ui::string& key2) const;
	};
	typedef std::map<ui::string, std::weak_ptr<ImageInfo>, ImageCacheKeyCompare> MapStringToImagePtr;

	static ui::string m_pStrResourcePath; //全局的资源路径，换肤的时候修改这个变量
	static std::vector<Window*> m_aPreMessages;
	static std::map<ui::string, std::unique_ptr<WindowBuilder>> m_builderMap;
	static CreateControlCallback m_createControlCallback;

	static MapStringToImagePtr m_mImageHash;
	static std::map<ui::string, DWORD> m_mapTextColor;
	static std::map<ui::string, ui::string> m_mGlobalClass;
	static std::map<ui::string, TFontInfo*> m_mCustomFonts;
	static std::map<ui::string, ui::string> m_mGlobalWindow;

	static ui::string m_sDefaultFontId;

	static short m_H;
	static short m_S;
	static short m_L;

	static ui::string m_strDefaultFontName;
	static ui::string m_strDefaultDisabledColor;
	static ui::string m_strDefaultFontColor;
	static DWORD m_dwDefaultLinkFontColor;
	static DWORD m_dwDefaultLinkHoverFontColor;
	static DWORD m_dwDefaultSelectedBkColor;
	//
	static std::map<ui::string, std::unique_ptr<SkinBox>> m_mSkinBox;
	static std::map<ui::string, std::weak_ptr<StateSkin>> m_mSkinHash;
};

} // namespace ui

#endif // UI_CORE_WINDOWHELPER_H_
