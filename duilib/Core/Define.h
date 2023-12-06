#ifndef UI_CORE_DEFINE_H_
#define UI_CORE_DEFINE_H_

#pragma once

//#define DUI_STAND

namespace ui
{

class Control;

#define DUI_ZERO_VALUE   0
#define DUI_NOSET_VALUE  -1
#define DUI_LENGTH_STRETCH  -1
#define DUI_LENGTH_AUTO  -2

enum HorAlignType
{
	kHorAlignLeft,		// 靠左对齐
	kHorAlignCenter,	// 水平居中
	kHorAlignRight,		// 靠右对齐
};

enum VerAlignType
{
	kVerAlignTop,		// 顶端对齐
	kVerAlignCenter,	// 垂直居中
	kVerAlignBottom,	// 底部对齐
};

enum StateImageType
{
	kStateImageBk,
	kStateImageFore,
	kStateImageSelectedBk,
	kStateImageSelectedFore,
};

// Flags used for controlling the paint
enum ControlStateType
{
	kControlStateNormal,	// 普通状态
	kControlStateHot,		// 焦点状态
	kControlStatePushed,	// 按下状态
	kControlStateDisabled	// 禁用状态
};

enum AnimationType
{
	kAnimationNone,
	kAnimationAlpha,
	kAnimationHeight,
	kAnimationWidth,
	kAnimationHot,
	kAnimationInoutXFromLeft,
	kAnimationInoutXFromRight,
	kAnimationInoutYFromTop,
	kAnimationInoutYFromBottom,
};

enum GifStopType
{
	kGifStopCurrent = -1,	// 当前帧
	kGifStopFirst = 0,		// GIF 停止时定位到第一帧	
	kGifStopLast = 0xFFFF,	// 最后一帧
};

enum CursorType
{
	kCursorArrow,		// 箭头
	kCursorHand,		// 手柄
	kCursorHandIbeam	// 
};

//定义所有消息类型
enum EventType
{
	kEventInternalDoubleClick,
	kEventInternalMenu,
	kEventInternalSetFocus,
	kEventInternalKillFocus ,

	kEventNone,

	kEventFirst,

	kEventAll,

	kEventKeyBegin,
	kEventKeyDown,
	kEventKeyUp,
	kEventChar,
	kEventSystemKey,
	kEventKeyEnd,

	kEventMouseBegin,
	kEventMouseMove,
	kEventMouseEnter,
	kEventMouseLeave,
	kEventMouseHover,
	kEventMouseButtonDown,
	kEventMouseButtonUp,
	kEventMouseRightButtonDown,
	kEventMouseRightButtonUp,
	kEventMouseDoubleClick,
	kEventMouseMenu,
	kEventMouseScrollWheel,
	kEventMouseEnd,

	kEventTouchBegin,
	kEventTouchDown,
	kEventTouchMove,
	kEventTouchUp,
	kEventTouchEnd,

	kEventPointBegin,
	kEventPointDown,
	kEventPointMove,
	kEventPointUp,
	kEventPointEnd,

	kEventSetFocus,
	kEventKillFocus,
	kEventWindowSize,
	kEventWindowClose,
	kEventSetCursor,

	kEventClick,
	kEventSelect,
	kEventUnSelect,

	kEventTextChange,
	kEventReturn,
	kEventTab,
	kEventCustomLinkClick,

	kEventImeStartComposition,
	kEventImeEndComposition,

	kEventScrollChange,
	kEventValueChange,
	kEventResize,

	kEventNotify,	//仅作简单的通知，有复杂数据请通过其他方式实现

	kEventLast,
};


#define	EVENTSTR_ALL				(_T("all"))
#define	EVENTSTR_KEYDOWN			(_T("keydown"))
#define	EVENTSTR_KEYUP				(_T("keyup"))
#define	EVENTSTR_CHAR				(_T("char"))
#define	EVENTSTR_SYSKEY				(_T("syskey"))

#define EVENTSTR_SETFOCUS			(_T("setfocus"))
#define EVENTSTR_KILLFOCUS			(_T("killfocus"))
#define	EVENTSTR_SETCURSOR			(_T("setcursor"))

#define EVENTSTR_MOUSEMOVE			(_T("mousemove"))
#define	EVENTSTR_MOUSEENTER			(_T("mouseenter"))
#define	EVENTSTR_MOUSELEAVE			(_T("mouseleave"))
#define	EVENTSTR_MOUSEHOVER			(_T("mousehover"))

#define	EVENTSTR_BUTTONDOWN			(_T("buttondown"))
#define	EVENTSTR_BUTTONUP			(_T("buttonup"))
#define	EVENTSTR_RBUTTONDOWN		(_T("rbuttondown"))
#define	EVENTSTR_DOUBLECLICK		(_T("doubleclick"))

#define EVENTSTR_SELECT				(_T("select"))
#define EVENTSTR_UNSELECT			(_T("unselect"))
#define	EVENTSTR_MENU				(_T("menu"))

#define	EVENTSTR_SCROLLWHEEL		(_T("scrollwheel"))
#define EVENTSTR_SCROLLCHANGE		(_T("scrollchange"))

#define EVENTSTR_VALUECHANGE		(_T("valuechange"))
#define EVENTSTR_RETURN				(_T("return"))
#define EVENTSTR_TAB				(_T("tab"))
#define EVENTSTR_WINDOWCLOSE		(_T("windowclose"))


struct EventArgs
{
	EventArgs():
	 Type(kEventNone),
     pSender(nullptr),
	 dwTimestamp(0),
	 chKey(0),
	 wParam(0),
	 lParam(0),
	 pressure(0.0f)
	{
		ptMouse.x = ptMouse.y = 0;
	}

	EventType Type;
	Control* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	TCHAR chKey;
	WPARAM wParam;
	LPARAM lParam;
	FLOAT pressure;
};

EventType StringToEnum(const std::wstring& messageType);


//定义所有控件类型 ///////////////////////////////////////////////////////////////////////////
#define  DUI_CTR_CONTROL                         (_T("Control"))
#define  DUI_CTR_LABEL                           (_T("Label"))
#define  DUI_CTR_BUTTON                          (_T("Button"))
#define  DUI_CTR_TEXT                            (_T("Text"))
#define  DUI_CTR_OPTION                          (_T("Option"))
#define  DUI_CTR_CHECKBOX                        (_T("CheckBox"))

#define  DUI_CTR_LABELBOX                        (_T("LabelBox"))
#define  DUI_CTR_BUTTONBOX                       (_T("ButtonBox"))
#define  DUI_CTR_OPTIONBOX                       (_T("OptionBox"))
#define  DUI_CTR_CHECKBOXBOX                     (_T("CheckBoxBox"))

#define  DUI_CTR_BOX							 (_T("Box"))
#ifdef DUI_STAND
#define  DUI_CTR_HBOX							 (_T("HorizontalLayout"))
#define  DUI_CTR_VBOX                            (_T("VerticalLayout"))
#else
#define  DUI_CTR_HBOX							 (_T("HBox"))
#define  DUI_CTR_VBOX                            (_T("VBox"))
#endif // DUI_STAND

#define  DUI_CTR_TABBOX                          (_T("TabBox"))
#define  DUI_CTR_TILEBOX						 (_T("TileBox"))
#define  DUI_CTR_CHILDBOX                        (_T("ChildBox"))

#define  DUI_CTR_LISTCONTAINERELEMENT            (_T("ListContainerElement"))
#define  DUI_CTR_LISTBOX						 (_T("ListBox"))
#define  DUI_CTR_HLISTBOX						 (_T("HListBox"))
#define  DUI_CTR_VLISTBOX                        (_T("VListBox"))
#define  DUI_CTR_VIRTUALLISTBOX                  (_T("VirtualListBox"))
#define  DUI_CTR_TILELISTBOX                     (_T("TileListBox"))

#define  DUI_CTR_TREENODE                        (_T("TreeNode"))
#define  DUI_CTR_TREEVIEW                        (_T("TreeView"))

#define  DUI_CTR_RICHEDIT                        (_T("RichEdit"))
#define  DUI_CTR_COMBO                           (_T("Combo"))
#define  DUI_CTR_SLIDER                          (_T("Slider"))
#define  DUI_CTR_PROGRESS                        (_T("Progress"))
#define  DUI_CTR_CIRCLEPROGRESS                  (_T("CircleProgress"))
#define  DUI_CTR_SCROLLBAR                       (_T("ScrollBar"))
//定义Window及Global属性 ///////////////////////////////////////////////////////////////////////////
// Window
#define ATTR_WINDOW_size					(_T("size"))
#define ATTR_WINDOW_heightpercent			(_T("heightpercent"))
#define ATTR_WINDOW_sizebox					(_T("sizebox"))
#define ATTR_WINDOW_caption					(_T("caption"))
#define ATTR_WINDOW_textid					(_T("textid"))
#define ATTR_WINDOW_roundcorner				(_T("roundcorner"))		// *仅在阴影关闭shadowattached=false时有效
#define ATTR_WINDOW_mininfo					(_T("mininfo"))
#define ATTR_WINDOW_maxinfo					(_T("maxinfo"))
#define ATTR_WINDOW_shadowattached			(_T("shadowattached"))
#define ATTR_WINDOW_shadowimage				(_T("shadowimage"))
#define ATTR_WINDOW_shadowcorner			(_T("shadowcorner"))
#define ATTR_WINDOW_alphafixcorner			(_T("alphafixcorner"))
#define ATTR_WINDOW_custom_shadow			(_T("custom_shadow"))

//定义所有控件属性 ///////////////////////////////////////////////////////////////////////////
// Control
#define ATTR_CTR_class						(_T("class"))			// 设置为default将使用默认样式
#define ATTR_CTR_halign						(_T("halign"))
#define ATTR_CTR_valign						(_T("valign"))
#define ATTR_CTR_margin						(_T("margin"))
#define ATTR_CTR_bkcolor					(_T("bkcolor"))
#define ATTR_CTR_bkcolor1					(_T("bkcolor1"))
#define ATTR_CTR_bordersize					(_T("bordersize"))
#define ATTR_CTR_borderround				(_T("borderround"))
#define ATTR_CTR_width						(_T("width"))
#define ATTR_CTR_height						(_T("height"))
#define ATTR_CTR_maxwidth					(_T("maxwidth"))
#define ATTR_CTR_maxheight					(_T("maxheight"))
#define ATTR_CTR_state						(_T("state"))
#define ATTR_CTR_cursortype					(_T("cursortype"))
#define ATTR_CTR_renderoffset				(_T("renderoffset"))
#define ATTR_CTR_normalcolor				(_T("normalcolor"))
#define ATTR_CTR_hotcolor					(_T("hotcolor"))
#define ATTR_CTR_pushedcolor				(_T("pushedcolor"))
#define ATTR_CTR_disabledcolor				(_T("disabledcolor"))
#define ATTR_CTR_bordercolor				(_T("bordercolor"))
#define ATTR_CTR_leftbordersize				(_T("leftbordersize"))
#define ATTR_CTR_topbordersize				(_T("topbordersize"))
#define ATTR_CTR_rightbordersize			(_T("rightbordersize"))
#define ATTR_CTR_bottombordersize			(_T("bottombordersize"))
#define ATTR_CTR_bkimage					(_T("bkimage"))
#define ATTR_CTR_minwidth					(_T("minwidth"))
#define ATTR_CTR_minheight					(_T("minheight"))
#define ATTR_CTR_name						(_T("name"))
#define ATTR_CTR_tooltiptext				(_T("tooltiptext"))
#define ATTR_CTR_tooltiptextid				(_T("tooltiptextid"))
#define ATTR_CTR_dataid						(_T("dataid"))
#define ATTR_CTR_enabled					(_T("enabled"))
#define ATTR_CTR_mouse						(_T("mouse"))
#define ATTR_CTR_keyboard					(_T("keyboard"))
#define ATTR_CTR_visible					(_T("visible"))
#define ATTR_CTR_fadevisible				(_T("fadevisible"))
#define ATTR_CTR_float						(_T("float"))
#define ATTR_CTR_menu						(_T("menu"))
#define ATTR_CTR_cache						(_T("cache"))
#define ATTR_CTR_nofocus					(_T("nofocus"))
#define ATTR_CTR_alpha						(_T("alpha"))
#define ATTR_CTR_normalimage				(_T("normalimage"))
#define ATTR_CTR_hotimage					(_T("hotimage"))
#define ATTR_CTR_pushedimage				(_T("pushedimage"))
#define ATTR_CTR_disabledimage				(_T("disabledimage"))
#define ATTR_CTR_forenormalimage			(_T("forenormalimage"))
#define ATTR_CTR_forehotimage				(_T("forehotimage"))
#define ATTR_CTR_forepushedimage			(_T("forepushedimage"))
#define ATTR_CTR_foredisabledimage			(_T("foredisabledimage"))
#define ATTR_CTR_fadealpha					(_T("fadealpha"))
#define ATTR_CTR_fadehot					(_T("fadehot"))
#define ATTR_CTR_fadewidth					(_T("fadewidth"))
#define ATTR_CTR_fadeheight					(_T("fadeheight"))
#define ATTR_CTR_fadeinoutxfromleft			(_T("fadeinoutxfromleft"))
#define ATTR_CTR_fadeinoutxfromright		(_T("fadeinoutxfromright"))
#define ATTR_CTR_fadeinoutyfromtop			(_T("fadeinoutyfromtop"))
#define ATTR_CTR_fadeinoutyfrombottom		(_T("fadeinoutyfrombottom"))
#define ATTR_CTR_receivepointer				(_T("receivepointer"))
// Layout (BOX持有)
#define ATTR_LAYOUT_padding					(_T("padding"))
#define ATTR_LAYOUT_childmargin				(_T("childmargin"))
// TileLayout 继承自 Layout
#define ATTR_TILELAYOUT_itemsize			(_T("itemsize"))
#define ATTR_TILELAYOUT_columns				(_T("columns"))
// VirtualTileLayout 继承自 TileLayout
#define ATTR_VTILELAYOUT_column				(_T("column"))
// Box
#define ATTR_BOX_mousechild					(_T("mousechild"))
// TabBox 继承自 Box
#define ATTR_TABBOX_selectedid				(_T("selectedid"))
#define ATTR_TABBOX_fadeswitch				(_T("fadeswitch"))
// ScrollableBox 继承自 Box
#define ATTR_SLBOX_vscrollbar				(_T("vscrollbar"))
#define ATTR_SLBOX_vscrollbarstyle			(_T("vscrollbarstyle"))
#define ATTR_SLBOX_hscrollbar				(_T("hscrollbar"))
#define ATTR_SLBOX_hscrollbarstyle			(_T("hscrollbarstyle"))
#define ATTR_SLBOX_scrollbarpadding			(_T("scrollbarpadding"))
#define ATTR_SLBOX_vscrollunit				(_T("vscrollunit"))
#define ATTR_SLBOX_hscrollunit				(_T("hscrollunit"))
#define ATTR_SLBOX_scrollbarfloat			(_T("scrollbarfloat"))
#define ATTR_SLBOX_defaultdisplayscrollbar	(_T("defaultdisplayscrollbar"))
#define ATTR_SLBOX_holdend					(_T("holdend"))
// ListBox 继承自 ScrollableBox
#define ATTR_LISTBOX_scrollselect			(_T("scrollselect"))
// TreeView 继承自 ListBox
#define ATTR_TREEVIEW_indent				(_T("indent"))
// RichEdit 继承自 ScrollableBox
#define ATTR_RICHEDIT_vscrollbar			(_T("vscrollbar"))
#define ATTR_RICHEDIT_autovscroll			(_T("autovscroll"))
#define ATTR_RICHEDIT_hscrollbar			(_T("hscrollbar"))
#define ATTR_RICHEDIT_autohscroll			(_T("autohscroll"))
#define ATTR_RICHEDIT_multiline				(_T("multiline"))
#define ATTR_RICHEDIT_readonly				(_T("readonly"))
#define ATTR_RICHEDIT_password				(_T("password"))
#define ATTR_RICHEDIT_number				(_T("number"))
#define ATTR_RICHEDIT_align					(_T("align"))
#define ATTR_RICHEDIT_normaltextcolor		(_T("normaltextcolor"))
#define ATTR_RICHEDIT_disabledtextcolor		(_T("disabledtextcolor"))
#define ATTR_RICHEDIT_caretcolor			(_T("caretcolor"))
#define ATTR_RICHEDIT_promptmode			(_T("promptmode"))
#define ATTR_RICHEDIT_promptcolor			(_T("promptcolor"))
#define ATTR_RICHEDIT_prompttext			(_T("prompttext"))
#define ATTR_RICHEDIT_prompttextid			(_T("prompttextid"))
#define ATTR_RICHEDIT_focusedimage			(_T("focusedimage"))
#define ATTR_RICHEDIT_font					(_T("font"))
#define ATTR_RICHEDIT_text					(_T("text"))
#define ATTR_RICHEDIT_textid				(_T("textid"))
#define ATTR_RICHEDIT_wanttab				(_T("wanttab"))
#define ATTR_RICHEDIT_wantreturnmsg			(_T("wantreturnmsg"))
#define ATTR_RICHEDIT_returnmsgwantctrl		(_T("returnmsgwantctrl"))
#define ATTR_RICHEDIT_rich					(_T("rich"))
// ChildBox 继承自Box
#define ATTR_CHILDBOX_xmlfile				(_T("xmlfile"))
// Combo 继承自Box
#define ATTR_COMBO_dropbox					(_T("dropbox"))
#define ATTR_COMBO_vscrollbar				(_T("vscrollbar")) // 未处理?
#define ATTR_COMBO_dropboxsize				(_T("dropboxsize"))
#define ATTR_COMBO_popuptop					(_T("popuptop"))
// LabelTemplate 默认继承自 Control
#define ATTR_LABEL_align					(_T("align"))
#define ATTR_LABEL_endellipsis				(_T("endellipsis"))
#define ATTR_LABEL_linelimit				(_T("linelimit"))
#define ATTR_LABEL_singleline				(_T("singleline"))
#define ATTR_LABEL_text						(_T("text"))
#define ATTR_LABEL_textid					(_T("textid"))
#define ATTR_LABEL_font						(_T("font"))
#define ATTR_LABEL_normaltextcolor			(_T("normaltextcolor"))
#define ATTR_LABEL_hottextcolor				(_T("hottextcolor"))
#define ATTR_LABEL_pushedtextcolor			(_T("pushedtextcolor"))
#define ATTR_LABEL_disabledtextcolor		(_T("disabledtextcolor"))
#define ATTR_LABEL_textpadding				(_T("textpadding"))
// ButtonTemplate 继承自 LabelTemplate 无自有属性
// CheckBoxTemplate 继承自ButtonTemplate
#define ATTR_CHECK_selected					(_T("selected"))
#define ATTR_CHECK_selectednormalimage		(_T("selectednormalimage"))
#define ATTR_CHECK_selectedhotimage			(_T("selectedhotimage"))
#define ATTR_CHECK_selectedpushedimage		(_T("selectedpushedimage"))
#define ATTR_CHECK_selecteddisabledimage	(_T("selecteddisabledimage"))
#define ATTR_CHECK_selectedtextcolor		(_T("selectedtextcolor"))
#define ATTR_CHECK_selectednormalcolor		(_T("selectednormalcolor"))
#define ATTR_CHECK_selectedhotcolor			(_T("selectedhotcolor"))
#define ATTR_CHECK_selectedpushedcolor		(_T("selectedpushedcolor"))
#define ATTR_CHECK_selecteddisabledcolor	(_T("selecteddisabledcolor"))
#define ATTR_CHECK_selectedforenormalimage	(_T("selectedforenormalimage"))
#define ATTR_CHECK_selectedforehotimage		(_T("selectedforehotimage"))
#define ATTR_CHECK_selectedforepushedimage	(_T("selectedforepushedimage"))
#define ATTR_CHECK_selectedforedisabledimage (_T("selectedforedisabledimage"))
// OptionTemplate 继承自 CheckBoxTemplate
#define ATTR_OPTION_group					(_T("group"))
// Progress 继承自 LabelTemplate
#define ATTR_PROGRESS_hor					(_T("hor"))
#define ATTR_PROGRESS_min					(_T("min"))
#define ATTR_PROGRESS_max					(_T("max"))
#define ATTR_PROGRESS_value					(_T("value"))
#define ATTR_PROGRESS_progressimage			(_T("progressimage"))
#define ATTR_PROGRESS_isstretchfore			(_T("isstretchfore"))
#define ATTR_PROGRESS_progresscolor			(_T("progresscolor"))
// CircleProgress 继承自 Progress
#define ATTR_CPROGRESS_circular				(_T("circular"))
#define ATTR_CPROGRESS_circlewidth			(_T("circlewidth"))
#define ATTR_CPROGRESS_indicator			(_T("indicator"))
#define ATTR_CPROGRESS_clockwise			(_T("clockwise"))
#define ATTR_CPROGRESS_bgcolor				(_T("bgcolor"))
#define ATTR_CPROGRESS_fgcolor				(_T("fgcolor"))
#define ATTR_CPROGRESS_gradientcolor		(_T("gradientcolor"))
// Slider 继承自 Progress
#define ATTR_SLIDER_step					(_T("step"))
#define ATTR_SLIDER_thumbnormalimage		(_T("thumbnormalimage"))
#define ATTR_SLIDER_thumbhotimage			(_T("thumbhotimage"))
#define ATTR_SLIDER_thumbpushedimage		(_T("thumbpushedimage"))
#define ATTR_SLIDER_thumbdisabledimage		(_T("thumbdisabledimage"))
#define ATTR_SLIDER_thumbsize				(_T("thumbsize"))
#define ATTR_SLIDER_progressbarpadding		(_T("progressbarpadding"))
// ScrollBar 继承自 Control
#define ATTR_SCROLBAR_button1normalimage	(_T("button1normalimage"))
#define ATTR_SCROLBAR_button1hotimage		(_T("button1hotimage"))
#define ATTR_SCROLBAR_button1pushedimage	(_T("button1pushedimage"))
#define ATTR_SCROLBAR_button1disabledimage	(_T("button1disabledimage"))
#define ATTR_SCROLBAR_button2normalimage	(_T("button2normalimage"))
#define ATTR_SCROLBAR_button2hotimage		(_T("button2hotimage"))
#define ATTR_SCROLBAR_button2pushedimage	(_T("button2pushedimage"))
#define ATTR_SCROLBAR_button2disabledimage	(_T("button2disabledimage"))
#define ATTR_SCROLBAR_thumbnormalimage		(_T("thumbnormalimage"))
#define ATTR_SCROLBAR_thumbhotimage			(_T("thumbhotimage"))
#define ATTR_SCROLBAR_thumbpushedimage		(_T("thumbpushedimage"))
#define ATTR_SCROLBAR_thumbdisabledimage	(_T("thumbdisabledimage"))
#define ATTR_SCROLBAR_railnormalimage		(_T("railnormalimage"))
#define ATTR_SCROLBAR_railhotimage			(_T("railhotimage"))
#define ATTR_SCROLBAR_railpushedimage		(_T("railpushedimage"))
#define ATTR_SCROLBAR_raildisabledimage		(_T("raildisabledimage"))
#define ATTR_SCROLBAR_bknormalimage			(_T("bknormalimage"))
#define ATTR_SCROLBAR_bkhotimage			(_T("bkhotimage"))
#define ATTR_SCROLBAR_bkpushedimage			(_T("bkpushedimage"))
#define ATTR_SCROLBAR_bkdisabledimage		(_T("bkdisabledimage"))
#define ATTR_SCROLBAR_hor					(_T("hor"))
#define ATTR_SCROLBAR_linesize				(_T("linesize"))
#define ATTR_SCROLBAR_thumbminlength		(_T("thumbminlength"))
#define ATTR_SCROLBAR_range					(_T("range"))
#define ATTR_SCROLBAR_value					(_T("value"))
#define ATTR_SCROLBAR_showbutton1			(_T("showbutton1"))
#define ATTR_SCROLBAR_showbutton2			(_T("showbutton2"))
#define ATTR_SCROLBAR_autohidescroll		(_T("autohidescroll"))
//子属性 ///////////////////////////////////////////////////////////////////////////
// Image
#define ATTR__IMAGE_file					(_T("file"))
#define ATTR__IMAGE_res						(_T("res"))
#define ATTR__IMAGE_destscale				(_T("destscale"))
#define ATTR__IMAGE_dest					(_T("dest"))
#define ATTR__IMAGE_source					(_T("source"))
#define ATTR__IMAGE_corner					(_T("corner"))
#define ATTR__IMAGE_fade					(_T("fade"))
#define ATTR__IMAGE_xtiled					(_T("xtiled"))
#define ATTR__IMAGE_ytiled					(_T("ytiled"))
#define ATTR__IMAGE_playcount				(_T("playcount"))
#define ATTR__IMAGE_svggroupid				(_T("svggroupid"))		// (NEW)增加svg组ID属性
#define ATTR__IMAGE_svgscale				(_T("svgscale"))		// (NEW)增加svg缩放比例, 0表示自动缩放到目标尺寸的分辨率
#define ATTR__IMAGE_fillcolor				(_T("fillcolor"))		// (NEW)增加使用颜色填充
#define ATTR__IMAGE_destpadding				(_T("destpadding"))		// (NEW)增加padding
}// namespace ui

#endif // UI_CORE_DEFINE_H_