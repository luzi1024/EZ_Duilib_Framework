#include "StdAfx.h"
#include "shlwapi.h"
#include "../Animation/AnimationPlayer.h"

namespace ui 
{
	const int Control::m_nVirtualEventGifStop = 1;
Control::Control() :
	OnXmlEvent(),
	OnEvent(),
	m_pUserDataBase(),
	m_bMenuUsed(false),
	m_bEnabled(true),
	m_bMouseEnabled(true),
	m_bKeyboardEnabled(true),
	m_bFocused(false),
	m_bMouseFocused(false),
	m_bSetPos(false),
	m_bNoFocus(false),
	m_bClip(true),
	m_bGifPlay(true),
	m_bReceivePointerMsg(true),
	m_bNeedButtonUpWhenKillFocus(false),
	m_szEstimateSize(),
	m_renderOffset(),
	m_cxyBorderRound(),
	m_rcMargin(),
	m_rcPaint(),
	m_rcBorderSize(),
	m_cursorType(kCursorArrow),
	m_uButtonState(kControlStateNormal),
	m_nBorderSize(0),
	m_nTooltipWidth(300),
	m_nAlpha(255),
	m_nHotAlpha(0),
	m_sToolTipText(),
	m_sToolTipTextId(),
	m_sUserData(),
	m_strBkColor(),
	m_colorMap(),
	m_strBorderColor(),
	m_gifWeakFlag(),
	m_animationManager(),
	m_imageMap(),
	m_bkImage(),
	m_strControlClass(),
	m_strSkinName(),
	m_loadBkImageWeakFlag()
{
	m_colorMap.SetControl(this);
	m_imageMap.SetControl(this);
	m_animationManager.Init(this);
}

Control::Control(const Control& r) :
	PlaceHolder(r),
	OnXmlEvent(),
	OnEvent(),
	m_pUserDataBase(),
	m_bMenuUsed(r.m_bMenuUsed),
	m_bEnabled(r.m_bEnabled),
	m_bMouseEnabled(r.m_bMouseEnabled),
	m_bKeyboardEnabled(r.m_bKeyboardEnabled),
	m_bFocused(r.m_bFocused),
	m_bMouseFocused(r.m_bMouseFocused),
	m_bSetPos(r.m_bSetPos),
	m_bNoFocus(r.m_bNoFocus),
	m_bClip(r.m_bClip),
	m_bGifPlay(r.m_bGifPlay),
	m_bReceivePointerMsg(r.m_bReceivePointerMsg),
	m_bNeedButtonUpWhenKillFocus(r.m_bNeedButtonUpWhenKillFocus),
	m_szEstimateSize(r.m_szEstimateSize),
	m_renderOffset(r.m_renderOffset),
	m_cxyBorderRound(r.m_cxyBorderRound),
	m_rcMargin(r.m_rcMargin),
	m_rcPaint(r.m_rcPaint),
	m_rcBorderSize(r.m_rcBorderSize),
	m_cursorType(r.m_cursorType),
	m_uButtonState(kControlStateNormal),
	m_nBorderSize(r.m_nBorderSize),
	m_nTooltipWidth(r.m_nTooltipWidth),
	m_nAlpha(r.m_nAlpha),
	m_nHotAlpha(r.m_nHotAlpha),
	m_sToolTipText(r.m_sToolTipText),
	m_sToolTipTextId(r.m_sToolTipTextId),
	m_sUserData(r.m_sUserData),
	m_strBkColor(r.m_strBkColor),
	m_colorMap(r.m_colorMap),
	m_strBorderColor(r.m_strBorderColor),
	m_gifWeakFlag(),
	m_animationManager(r.m_animationManager),
	m_imageMap(r.m_imageMap),
	m_bkImage(r.m_bkImage),
	m_strSkinName(r.m_strSkinName),
	m_loadBkImageWeakFlag()
{
	m_colorMap.SetControl(this);
	m_imageMap.SetControl(this);
	m_animationManager.Init(this);
	if (r.m_bGifPlay)
	{
		this->GifPlay();
	}
}

Control::~Control()
{
	if (m_pWindow) {
		m_pWindow->ReapObjects(this);
	}
}

bool Control::RemoveSelf()
{
	ui::Box* pParent = this->GetParent();
	if (pParent == nullptr)
		return false;
	return pParent->Remove(this);
}

ui::string Control::GetBkColor() const
{
	return m_strBkColor;
}

void Control::SetBkColor(const ui::string& strColor)
{
	ASSERT(strColor.empty() || GlobalManager::GetTextColor(strColor) != 0);
	if( m_strBkColor == strColor ) return;

	m_strBkColor = strColor;
	Invalidate();
}

ui::string Control::GetStateColor(ControlStateType stateType)
{
	return m_colorMap[stateType];
}

void Control::SetStateColor(ControlStateType stateType, const ui::string& strColor)
{
	ASSERT(GlobalManager::GetTextColor(strColor) != 0);
	if( m_colorMap[stateType] == strColor ) return;

	if (stateType == kControlStateHot) {
		m_animationManager.SetFadeHot(true);
	}
	m_colorMap[stateType] = strColor;
	Invalidate();
}

ui::string Control::GetBkImage() const
{
	return m_bkImage.imageAttribute.simageString;
}

void Control::SetBkImage(const ui::string& strImage)
{
	StopGifPlay();
	m_bkImage.SetImageString(strImage);
	m_bGifPlay = m_bkImage.imageAttribute.nPlayCount != 0;
	if (GetFixedWidth() == DUI_LENGTH_AUTO || GetFixedHeight() == DUI_LENGTH_AUTO) {
		ArrangeAncestor();
	}
	else {
		Invalidate();
	}
}

ui::string Control::GetStateImage(ControlStateType stateType)
{
	return m_imageMap.GetImagePath(kStateImageBk, stateType);
}

void Control::SetStateImage(ControlStateType stateType, const ui::string& strImage)
{
	if (stateType == kControlStateHot) {
		m_animationManager.SetFadeHot(true);
	}
	m_imageMap.SetImage(kStateImageBk, stateType, strImage);
	if (GetFixedWidth() == DUI_LENGTH_AUTO || GetFixedHeight() == DUI_LENGTH_AUTO) {
		ArrangeAncestor();
	}
	else {
		Invalidate();
	}
}

ui::string Control::GetForeStateImage(ControlStateType stateType)
{
	return m_imageMap.GetImagePath(kStateImageFore, stateType);
}

void Control::SetForeStateImage(ControlStateType stateType, const ui::string& strImage)
{
	if (stateType == kControlStateHot) {
		m_animationManager.SetFadeHot(true);
	}
	m_imageMap.SetImage(kStateImageFore, stateType, strImage);
	Invalidate();
}

ControlStateType Control::GetState() const
{
	return m_uButtonState;
}

void Control::SetState(ControlStateType pStrState) 
{
	if (pStrState == kControlStateNormal) {
		m_nHotAlpha = 0;
	}
	else if (pStrState == kControlStateHot) {
		m_nHotAlpha = 255;
	}

	m_uButtonState = pStrState;
	Invalidate();
}

Image* Control::GetEstimateImage()
{
	Image* estimateImage = nullptr;
	if (!m_bkImage.imageAttribute.sImageName.empty()) {
		estimateImage = &m_bkImage;
	}
	else {
		estimateImage = m_imageMap.GetEstimateImage(kStateImageBk);
	}

	return estimateImage;
}

int Control::GetBorderSize() const
{
	return m_nBorderSize;
}

void Control::SetBorderSize(int nSize)
{
	DpiManager::GetInstance()->ScaleInt(nSize);
	if (m_nBorderSize == nSize) return;

	m_nBorderSize = nSize;
	Invalidate();
}

ui::string Control::GetBorderColor() const
{
    return m_strBorderColor;
}

void Control::SetBorderColor(const ui::string& strBorderColor)
{
    if( m_strBorderColor == strBorderColor ) return;

    m_strBorderColor = strBorderColor;
    Invalidate();
}

void Control::SetBorderSize(UiRect rc)
{
	DpiManager::GetInstance()->ScaleRect(rc);
	m_rcBorderSize = rc;
	Invalidate();
}

int Control::GetLeftBorderSize() const
{
	return m_rcBorderSize.left;
}

void Control::SetLeftBorderSize(int nSize)
{
	DpiManager::GetInstance()->ScaleInt(nSize);
	m_rcBorderSize.left = nSize;
	Invalidate();
}

int Control::GetTopBorderSize() const
{
	return m_rcBorderSize.top;
}

void Control::SetTopBorderSize(int nSize)
{
	DpiManager::GetInstance()->ScaleInt(nSize);
	m_rcBorderSize.top = nSize;
	Invalidate();
}

int Control::GetRightBorderSize() const
{
	return m_rcBorderSize.right;
}

void Control::SetRightBorderSize(int nSize)
{
	DpiManager::GetInstance()->ScaleInt(nSize);
	m_rcBorderSize.right = nSize;
	Invalidate();
}

int Control::GetBottomBorderSize() const
{
	return m_rcBorderSize.bottom;
}

void Control::SetBottomBorderSize(int nSize)
{
	DpiManager::GetInstance()->ScaleInt(nSize);
	m_rcBorderSize.bottom = nSize;
	Invalidate();
}

CSize Control::GetBorderRound() const
{
    return m_cxyBorderRound;
}

void Control::SetBorderRound(CSize cxyRound)
{
	DpiManager::GetInstance()->ScaleSize(cxyRound);
    m_cxyBorderRound = cxyRound;
    Invalidate();
}

CursorType Control::GetCursorType() const
{
	return m_cursorType;
}

void Control::SetCursorType(CursorType flag)
{
	m_cursorType = flag;
}

ui::string Control::GetToolTipText() const
{
	ui::string strText = m_sToolTipText;
	if (strText.empty() && !m_sToolTipTextId.empty()) {
		strText = MutiLanSupport::GetInstance()->GetStringViaID(m_sToolTipTextId);
	}

	return strText;
}

void Control::SetToolTipText(const ui::string& strText)
{
	ui::string strTemp(strText);
	StringHelper::ReplaceAll(_T("<n>"),_T("\r\n"), strTemp);
	m_sToolTipText = strTemp;

	Invalidate();
}


void Control::SetToolTipTextId(const ui::string& strTextId)
{
	if (m_sToolTipTextId == strTextId) return;
	m_sToolTipTextId = strTextId;

	Invalidate();
}

void Control::SetToolTipWidth( int nWidth )
{
	DpiManager::GetInstance()->ScaleInt(nWidth);
	m_nTooltipWidth=nWidth;
}

int Control::GetToolTipWidth(void) const
{
	return m_nTooltipWidth;
}

bool Control::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void Control::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

ui::string Control::GetDataID() const
{
    return m_sUserData;
}

void Control::SetDataID(const ui::string& strText)
{
    m_sUserData = strText;
}

UserDataBase* Control::GetUserDataBase() const
{
	return m_pUserDataBase.get();
}

void Control::SetUserDataBase(UserDataBase* pUserDataBase)
{
	m_pUserDataBase.reset(pUserDataBase);
}

void Control::SetVisible(bool bVisible)
{
	if (bVisible) {
		m_animationManager.Appear();
	}
	else {
		m_animationManager.Disappear();
	}
}

void Control::SetInternVisible(bool bVisible)
{
	m_bInternVisible = bVisible;
	if (!bVisible && m_pWindow && m_pWindow->GetFocus() == this) {
		m_pWindow->SetFocus(NULL) ;
	}

	if (!IsVisible()) {
		StopGifPlay();
	}
}

void Control::SetVisible_(bool bVisible)
{
	if (m_bVisible == bVisible) return;
	bool v = IsVisible();
	m_bVisible = bVisible;
	if (m_bFocused) m_bFocused = false;
	if (!bVisible && m_pWindow && m_pWindow->GetFocus() == this) {
		m_pWindow->SetFocus(NULL);
	}
	if (IsVisible() != v) {
		ArrangeAncestor();
	}

	if (!IsVisible()) {
		StopGifPlay();
	}
}

bool Control::IsEnabled() const
{
    return m_bEnabled;
}

void Control::SetEnabled(bool bEnabled)
{
    if( m_bEnabled == bEnabled ) return;

    m_bEnabled = bEnabled;
	if (m_bEnabled) {
		m_uButtonState = kControlStateNormal;
		m_nHotAlpha = 0;
	}
	else {
		m_uButtonState = kControlStateDisabled;
	}
    Invalidate();
}

bool Control::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void Control::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool Control::IsKeyboardEnabled() const
{
	return m_bKeyboardEnabled ;
}
void Control::SetKeyboardEnabled(bool bEnabled)
{
	m_bKeyboardEnabled = bEnabled ; 
}

bool Control::IsFocused() const
{
    return m_bFocused;
}

void Control::SetFocus()
{
	if( m_bNoFocus )
		return;
    if( m_pWindow != NULL ) m_pWindow->SetFocus(this);
}

UINT Control::GetControlFlags() const
{
	return UIFLAG_TABSTOP;
}

void Control::SetNoFocus()
{
	m_bNoFocus = true;
}

void Control::Activate()
{

}

bool Control::IsActivatable() const
{
	if (!IsVisible()) return false;
	if (!IsEnabled()) return false;
	return true;
}

Control* Control::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags, CPoint scrollPos)
{
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
    return Proc(this, pData);
}

UiRect Control::GetPos(bool bContainShadow) const
{
	UiRect pos = m_rcItem;
	if (m_pWindow && !bContainShadow) {
		UiRect shadowLength = m_pWindow->GetShadowCorner();
		pos.Offset(-shadowLength.left, -shadowLength.top);
	}

	return pos;
}

void Control::SetPos(UiRect rc)
{
	if (rc.right < rc.left) rc.right = rc.left;
	if (rc.bottom < rc.top) rc.bottom = rc.top;

	if (m_rcItem.Equal(rc)) {
		m_bIsArranged = false;
		return;
	}

	UiRect invalidateRc = m_rcItem;
	if (::IsRectEmpty(&invalidateRc)) invalidateRc = rc;

	m_rcItem = rc;
	if (m_pWindow == NULL) return;

	if (!m_bSetPos) {
		m_bSetPos = true;
		m_pWindow->SendNotify(this, kEventResize, NULL, NULL);
		m_bSetPos = false;
	}

	m_bIsArranged = false;
	invalidateRc.Union(m_rcItem);

	Control* pParent = this;
	UiRect rcTemp;
	UiRect rcParent;
	CPoint offset = GetScrollOffset();
	invalidateRc.Offset(-offset.x, -offset.y);
	while ((pParent = pParent->GetParent()) != nullptr)
	{
		rcTemp = invalidateRc;
		rcParent = pParent->GetPos();
		if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent)) {
			return;
		}
	}
	m_pWindow->Invalidate(invalidateRc);
}

UiRect Control::GetMargin() const
{
	return m_rcMargin;
}

void Control::SetMargin(UiRect rcMargin, bool bNeedDpiScale)
{
	if (bNeedDpiScale)
		DpiManager::GetInstance()->ScaleRect(rcMargin);

	if (!m_rcMargin.Equal(rcMargin)) {
		m_rcMargin = rcMargin;
		ArrangeAncestor();
	}
}

CSize Control::EstimateSize(CSize szAvailable)
{
	CSize imageSize = m_cxyFixed;
	if (GetFixedWidth() == DUI_LENGTH_AUTO || GetFixedHeight() == DUI_LENGTH_AUTO) {
		if (!m_bReEstimateSize) {
			return m_szEstimateSize;
		}
		Image* image = GetEstimateImage();
		if (image) {
			auto imageAttribute = image->imageAttribute;
			if (imageAttribute.rcSource.left != DUI_NOSET_VALUE && imageAttribute.rcSource.top != DUI_NOSET_VALUE
				&& imageAttribute.rcSource.right != DUI_NOSET_VALUE && imageAttribute.rcSource.bottom != DUI_NOSET_VALUE) {
				if ((GetFixedWidth() <= 0)) {
					SetFixedWidth(imageAttribute.rcSource.right - imageAttribute.rcSource.left);
				}
				if ((GetFixedHeight() <= 0)) {
					SetFixedHeight(imageAttribute.rcSource.bottom - imageAttribute.rcSource.top);
				}
				return m_cxyFixed;
			}

			GetImage(*image);
			if (image->imageCache) {
				if (GetFixedWidth() == DUI_LENGTH_AUTO) {
					int image_width = image->imageCache->nX;
					DpiManager::GetInstance()->ScaleInt(image_width);
					imageSize.cx = image_width;
				}
				if (GetFixedHeight() == DUI_LENGTH_AUTO) {
					int image_height = image->imageCache->nY;
					DpiManager::GetInstance()->ScaleInt(image_height);
					imageSize.cy = image_height;
				}
			}
		}

		m_bReEstimateSize = false;
		CSize textSize = EstimateText(szAvailable, m_bReEstimateSize);
		if (GetFixedWidth() == DUI_LENGTH_AUTO && imageSize.cx < textSize.cx) {
			imageSize.cx = textSize.cx;
		}
		if (GetFixedHeight() == DUI_LENGTH_AUTO && imageSize.cy < textSize.cy) {
			imageSize.cy = textSize.cy;
		}

		m_szEstimateSize = imageSize;
	}

	return imageSize;
}

CSize Control::EstimateText(CSize szAvailable, bool& bReEstimateSize)
{
	return CSize();
}

bool Control::IsPointInWithScrollOffset(const CPoint& point) const
{
	CPoint scrollOffset = GetScrollOffset();
	CPoint newPoint = point;
	newPoint.Offset(scrollOffset);
	return m_rcItem.IsPointIn(newPoint);
}

void Control::HandleMessageTemplate(EventType eventType, WPARAM wParam, LPARAM lParam, TCHAR tChar, CPoint mousePos, FLOAT pressure)
{
	EventArgs msg;
	msg.pSender = this;
	msg.Type = eventType;
	msg.chKey = tChar;
	msg.wParam = wParam;
	msg.lParam = lParam;
	msg.pressure = pressure;
	if (0 == mousePos.x == mousePos.y)
		msg.ptMouse = m_pWindow->GetLastMousePos();
	else
		msg.ptMouse = mousePos;
	msg.dwTimestamp = ::GetTickCount();

	HandleMessageTemplate(msg);
}

void Control::HandleMessageTemplate(EventArgs& msg)
{
	if (msg.Type == kEventInternalDoubleClick || msg.Type == kEventInternalMenu
		|| msg.Type == kEventInternalSetFocus || msg.Type == kEventInternalKillFocus) {
		HandleMessage(msg);
		return;
	}
	bool bRet = true;

	if (this == msg.pSender) {
		std::weak_ptr<nbase::WeakFlag> weakflag = GetWeakFlag();
		auto callback = OnEvent.find(msg.Type);
		if (callback != OnEvent.end()) {
			bRet = callback->second(&msg);
		}
		if (weakflag.expired()) {
			return;
		}

		callback = OnEvent.find(kEventAll);
		if (callback != OnEvent.end()) {
			bRet = callback->second(&msg);
		}
		if (weakflag.expired()) {
			return;
		}

		if (bRet) {
			auto xmlcallback = OnXmlEvent.find(msg.Type);
			if (xmlcallback != OnXmlEvent.end()) {
				bRet = xmlcallback->second(&msg);
			}
			if (weakflag.expired()) {
				return;
			}

			xmlcallback = OnXmlEvent.find(kEventAll);
			if (xmlcallback != OnXmlEvent.end()) {
				bRet = xmlcallback->second(&msg);
			}
			if (weakflag.expired()) {
				return;
			}
		}
	}
	else {
		ASSERT(FALSE);
	}
	
    if(bRet) {
		HandleMessage(msg);
	}
}

void Control::HandleMessage(EventArgs& msg)
{
	if( !IsMouseEnabled() && msg.Type > kEventMouseBegin && msg.Type < kEventMouseEnd ) {
		if( m_pParent != NULL) m_pParent->HandleMessageTemplate(msg);
		return;
	}
	else if( msg.Type == kEventSetCursor ) {
		if (m_cursorType == kCursorHand) {
			if (IsEnabled()) {
				::SetCursor(::LoadCursor(NULL, IDC_HAND));
			}
			else {
				::SetCursor(::LoadCursor(NULL, IDC_ARROW));
			}
			return;
		}
		else if (m_cursorType == kCursorArrow){
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
			return;
		}
		else if (m_cursorType == kCursorHandIbeam){
			::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
			return;
		}
		else {
			ASSERT(FALSE);
		}
	}
	else if (msg.Type == kEventInternalSetFocus) {
		SetState(kControlStateHot);
        m_bFocused = true;
        Invalidate();
		return;
    }
	else if (msg.Type == kEventInternalKillFocus) {
		SetState(kControlStateNormal);
        m_bFocused = false;
        Invalidate();
		return;
    }
	else if (msg.Type == kEventInternalMenu && IsEnabled()) {
        if( IsContextMenuUsed() ) {
            m_pWindow->SendNotify(this, kEventMouseMenu, msg.wParam, msg.lParam);
            return;
        }
    }
	else if( msg.Type == kEventMouseEnter ) {
		if (msg.pSender != this && m_pWindow) {
			if (!IsChild(this, m_pWindow->GetNewHover())) {
				return;
			}
		}
		MouseEnter(msg);
	}
	else if( msg.Type == kEventMouseLeave ) {
		if (msg.pSender != this && m_pWindow) {
			if (IsChild(this, m_pWindow->GetNewHover())) {
				return;
			}
		}
		MouseLeave(msg);
	}
	else if (msg.Type == kEventMouseButtonDown || msg.Type == kEventInternalDoubleClick) {
		ButtonDown(msg);
		return;
	}
	else if (msg.Type == kEventMouseButtonUp) {
		ButtonUp(msg);
		return;
	}
	else if (msg.Type == kEventPointDown && m_bReceivePointerMsg) {
		ButtonDown(msg);
		return;
	}
	else if (msg.Type == kEventPointUp && m_bReceivePointerMsg) {
		ButtonUp(msg);
		return;
	}

    if( m_pParent != NULL) m_pParent->HandleMessageTemplate(msg);
}

bool Control::HasHotState()
{
	// �жϱ��ؼ��Ƿ���hot״̬
	return m_colorMap.HasHotColor() || m_imageMap.HasHotImage() || (m_skinImage && m_skinImage->HasStateImage(kControlStateHot));
}

bool Control::MouseEnter(EventArgs& msg)
{
	if( IsEnabled() ) {
		if ( m_uButtonState == kControlStateNormal) {
			m_uButtonState = kControlStateHot;
			if (HasHotState()) {
				m_animationManager.MouseEnter();
				Invalidate();
			}
			return true;
		}
	}

	return false;
}

bool Control::MouseLeave(EventArgs& msg)
{
	if( IsEnabled() ) {
		if (m_uButtonState == kControlStateHot) {
			m_uButtonState = kControlStateNormal;
			if (HasHotState()) {
				m_animationManager.MouseLeave();
				Invalidate();
			}
			return true;
		}
	}

	return false;
}

bool Control::ButtonDown(EventArgs& msg)
{
	bool ret = false;
	if( IsEnabled() ) {
		m_uButtonState = kControlStatePushed;
		SetMouseFocused(true);
		Invalidate();
		ret = true;
	}

	return ret;
}

bool Control::ButtonUp(EventArgs& msg)
{
	bool ret = false;
	if( IsMouseFocused() ) {
		SetMouseFocused(false);
		auto player = m_animationManager.GetAnimationPlayer(kAnimationHot);
		if (player)
			player->Stop();

		Invalidate();
		if( IsPointInWithScrollOffset(msg.ptMouse) ) {
			if (msg.Type == kEventPointUp) {
				m_uButtonState = kControlStateNormal;
				m_nHotAlpha = 0;
			}
			else {
				m_uButtonState = kControlStateHot;
				m_nHotAlpha = 255;
			}
			Activate();
			ret = true;
		}
		else {
			m_uButtonState = kControlStateNormal;
			m_nHotAlpha = 0;
		}
	}

	return ret;
}

void Control::SetAttribute(const ui::string& strName, const ui::string& strValue)
{
	if ( strName == ATTR_CTR_class) {
		SetClass(strValue);
	}
	else if ( strName == ATTR_CTR_skin ) {
		SetSkin(strValue);
	}
	else if( strName == ATTR_CTR_halign) {
		if (strValue == _T("left")) {
			SetHorAlignType(kHorAlignLeft);
		}
		else if (strValue == _T("center")) {
			SetHorAlignType(kHorAlignCenter);
		}
		else if (strValue == _T("right")) {
			SetHorAlignType(kHorAlignRight);
		}
		else {
			ASSERT(FALSE);
		}
	}
	else if( strName == ATTR_CTR_valign) {
		if (strValue == _T("top")) {
			SetVerAlignType(kVerAlignTop);
		}
		else if (strValue == _T("center")) {
			SetVerAlignType(kVerAlignCenter);
		}
		else if (strValue == _T("bottom")) {
			SetVerAlignType(kVerAlignBottom);
		}
		else {
			ASSERT(FALSE);
		}
	}
	else if( strName == ATTR_CTR_margin) {
        UiRect rcMargin;
        LPTSTR pstr = NULL;
        rcMargin.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);    
        rcMargin.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcMargin.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcMargin.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetMargin(rcMargin);
    }
    else if( strName == ATTR_CTR_bkcolor || strName == ATTR_CTR_bkcolor1) {
		LPCTSTR pValue = strValue.c_str();
        while( *pValue > _T('\0') && *pValue <= _T(' ') ) pValue = ::CharNext(pValue);
        SetBkColor(pValue);
    }
	else if (strName == ATTR_CTR_bordersize) {
		ui::string nValue = strValue;
		if (nValue.find(_T(',')) == ui::string::npos) {
			SetBorderSize(_ttoi(strValue.c_str()));
			UiRect rcBorder;
			SetBorderSize(rcBorder);
		}
		else {
			UiRect rcBorder;
			LPTSTR pstr = NULL;
			rcBorder.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
			rcBorder.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcBorder.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcBorder.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetBorderSize(rcBorder);
		}
	}
    else if( strName == ATTR_CTR_borderround) {
        CSize cxyRound;
        LPTSTR pstr = NULL;
        cxyRound.cx = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);    
        cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);     
        SetBorderRound(cxyRound);
    }
	else if( strName == ATTR_CTR_width) {
		if ( strValue == _T("stretch") ) {
			SetFixedWidth(DUI_LENGTH_STRETCH);
		}
		else if ( strValue == _T("auto") ) {
			SetFixedWidth(DUI_LENGTH_AUTO);
		}
		else {
			ASSERT(_ttoi(strValue.c_str()) >= 0);
			SetFixedWidth(_ttoi(strValue.c_str()));
		}
	}
	else if( strName == ATTR_CTR_height) {
		if ( strValue == _T("stretch") ) {
			SetFixedHeight(DUI_LENGTH_STRETCH);
		}
		else if ( strValue == _T("auto") ) {
			SetFixedHeight(DUI_LENGTH_AUTO);
		}
		else {
			ASSERT(_ttoi(strValue.c_str()) >= 0);
			SetFixedHeight(_ttoi(strValue.c_str()));
		}
	}
	else if( strName == ATTR_CTR_maxwidth) {
		if ( strValue == _T("stretch") ) {
			SetMaxWidth(DUI_LENGTH_STRETCH);
		}
		else if ( strValue == _T("auto") ) {
			SetMaxWidth(DUI_LENGTH_AUTO);
		}
		else {
			ASSERT(_ttoi(strValue.c_str()) >= 0);
			SetMaxWidth(_ttoi(strValue.c_str()));
		}
	}
	else if( strName == ATTR_CTR_maxheight) {
		if ( strValue == _T("stretch") ) {
			SetMaxHeight(DUI_LENGTH_STRETCH);
		}
		else if ( strValue == _T("auto") ) {
			SetMaxHeight(DUI_LENGTH_AUTO);
		}
		else {
			ASSERT(_ttoi(strValue.c_str()) >= 0);
			SetMaxHeight(_ttoi(strValue.c_str()));
		}
	}
	else if( strName == ATTR_CTR_state) {
		if( strValue == _T("normal") ) SetState(kControlStateNormal);
		else if( strValue == _T("hot") ) SetState(kControlStateHot);
		else if( strValue == _T("pushed") ) SetState(kControlStatePushed);
		else if( strValue == _T("disabled") ) SetState(kControlStateDisabled);
		else ASSERT(FALSE);
	}
	else if( strName == ATTR_CTR_cursortype) {
		if (strValue == _T("arrow")) {
			SetCursorType(kCursorArrow);
		}
		else if ( strValue == _T("hand") ) {
			SetCursorType(kCursorHand);
		}
		else if (strValue == _T("ibeam")) {
			SetCursorType(kCursorHandIbeam);
		}
		else {
			ASSERT(FALSE);
		}
	}
	else if (strName == ATTR_CTR_renderoffset) {
		CPoint renderOffset;
		LPTSTR pstr = NULL;
		renderOffset.x = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
		renderOffset.y = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);

		DpiManager::GetInstance()->ScalePoint(renderOffset);
		SetRenderOffset(renderOffset);
	}
	else if (strName == ATTR_CTR_normalcolor)	SetStateColor(kControlStateNormal, strValue);
	else if (strName == ATTR_CTR_hotcolor)	SetStateColor(kControlStateHot, strValue);
	else if (strName == ATTR_CTR_pushedcolor)	SetStateColor(kControlStatePushed, strValue);
	else if (strName == ATTR_CTR_disabledcolor)	SetStateColor(kControlStateDisabled, strValue);
	else if (strName == ATTR_CTR_bordercolor) SetBorderColor(strValue);
	else if (strName == ATTR_CTR_leftbordersize) SetLeftBorderSize(_ttoi(strValue.c_str()));
	else if (strName == ATTR_CTR_topbordersize) SetTopBorderSize(_ttoi(strValue.c_str()));
	else if (strName == ATTR_CTR_rightbordersize) SetRightBorderSize(_ttoi(strValue.c_str()));
	else if (strName == ATTR_CTR_bottombordersize) SetBottomBorderSize(_ttoi(strValue.c_str()));
	else if (strName == ATTR_CTR_bkimage) SetBkImage(strValue);
	else if (strName == ATTR_CTR_minwidth) SetMinWidth(_ttoi(strValue.c_str()));
	else if (strName == ATTR_CTR_minheight) SetMinHeight(_ttoi(strValue.c_str()));
	else if (strName == ATTR_CTR_name) SetName(strValue);
	else if (strName == ATTR_CTR_tooltiptext) SetToolTipText(strValue);
	else if (strName == ATTR_CTR_tooltiptextid) SetToolTipTextId(strValue);
	else if (strName == ATTR_CTR_dataid) SetDataID(strValue);
	else if (strName == ATTR_CTR_enabled) SetEnabled(strValue == _T("true"));
	else if (strName == ATTR_CTR_mouse) SetMouseEnabled(strValue == _T("true"));
	else if (strName == ATTR_CTR_keyboard) SetKeyboardEnabled(strValue == _T("true"));
	else if (strName == ATTR_CTR_visible) SetVisible_(strValue == _T("true"));
	else if (strName == ATTR_CTR_fadevisible) SetVisible(strValue == _T("true"));
	else if (strName == ATTR_CTR_float) SetFloat(strValue == _T("true"));
	else if (strName == ATTR_CTR_menu) SetContextMenuUsed(strValue == _T("true"));
	else if (strName == ATTR_CTR_cache) SetUseCache(strValue == _T("true"));
	else if (strName == ATTR_CTR_nofocus) SetNoFocus();
	else if (strName == ATTR_CTR_alpha) SetAlpha(_ttoi(strValue.c_str()));
	else if (strName == ATTR_CTR_normalimage) SetStateImage(kControlStateNormal, strValue);
	else if (strName == ATTR_CTR_hotimage) SetStateImage(kControlStateHot, strValue);
	else if (strName == ATTR_CTR_pushedimage) SetStateImage(kControlStatePushed, strValue);
	else if (strName == ATTR_CTR_disabledimage) SetStateImage(kControlStateDisabled, strValue);
	else if (strName == ATTR_CTR_focusedimage) SetStateImage(kControlStateFocused, strValue);
	else if (strName == ATTR_CTR_forenormalimage) SetForeStateImage(kControlStateNormal, strValue);
	else if (strName == ATTR_CTR_forehotimage) SetForeStateImage(kControlStateHot, strValue);
	else if (strName == ATTR_CTR_forepushedimage) SetForeStateImage(kControlStatePushed, strValue);
	else if (strName == ATTR_CTR_foredisabledimage) SetForeStateImage(kControlStateDisabled, strValue);
	else if (strName == ATTR_CTR_fadealpha) m_animationManager.SetFadeAlpha(strValue == _T("true"));
	else if (strName == ATTR_CTR_fadehot) m_animationManager.SetFadeHot(strValue == _T("true"));
	else if (strName == ATTR_CTR_fadewidth) m_animationManager.SetFadeWidth(strValue == _T("true"));
	else if (strName == ATTR_CTR_fadeheight) m_animationManager.SetFadeHeight(strValue == _T("true"));
	else if (strName == ATTR_CTR_fadeinoutxfromleft) m_animationManager.SetFadeInOutX(strValue == _T("true"), false);
	else if (strName == ATTR_CTR_fadeinoutxfromright) m_animationManager.SetFadeInOutX(strValue == _T("true"), true);
	else if (strName == ATTR_CTR_fadeinoutyfromtop) m_animationManager.SetFadeInOutY(strValue == _T("true"), false);
	else if (strName == ATTR_CTR_fadeinoutyfrombottom) m_animationManager.SetFadeInOutY(strValue == _T("true"), true);
	else if (strName == ATTR_CTR_receivepointer) SetReceivePointerMsg(strValue == _T("true"));
    else
    {
		ui::string ws = StringHelper::Printf(_T("��Ч��%s����:%s"), GetControlClass().c_str(), strName.c_str());
		::MessageBox(NULL, ws.c_str(), _T("�������Դ���"), MB_ICONERROR | MB_OK);
        ASSERT(FALSE);
    }
}

void Control::SetClass(const ui::string& strClass)
{
	std::list<ui::string> splitList = StringHelper::Split(strClass, _T(" "));
	for (auto it = splitList.begin(); it != splitList.end(); it++) {
		ui::string pDefaultAttributes;
		if (*it == _T("default"))
		{
			ui::string strDefault = StringHelper::Printf(_T("_%s_%s"), it->c_str(), GetControlClass().c_str());
			pDefaultAttributes = GlobalManager::GetClassAttributes(strDefault);
		}
		else
			pDefaultAttributes = GlobalManager::GetClassAttributes((*it));
		
		if (pDefaultAttributes.empty() && m_pWindow) {
			pDefaultAttributes = m_pWindow->GetClassAttributes(*it);
		}
		if (pDefaultAttributes.empty())
		{
			::MessageBox(nullptr, ui::string(_T("������Class��ʽ:") + strClass).c_str(), _T("����"), MB_ICONERROR);
			continue;
		}
		if( !pDefaultAttributes.empty() ) {
			ApplyAttributeList(pDefaultAttributes);
		}
	}
}

void Control::SetSkin(const ui::string& strSkin)
{
	m_strSkinName = strSkin;
	m_skinImage.reset();
}

void Control::ApplyAttributeList(const ui::string& strList)
{
	ui::string sItem;
	ui::string sValue;
	LPCTSTR pstrList = strList.c_str();
    while( *pstrList != _T('\0') ) {
        sItem.clear();
        sValue.clear();
        while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sItem += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('=') );
        if( *pstrList++ != _T('=') ) return;
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return;
        while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sValue += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return;
        SetAttribute(sItem, sValue);
        if( *pstrList++ != _T(' ') ) return;
    }
    return;
}

bool Control::OnApplyAttributeList(const ui::string& strReceiver, const ui::string& strList, EventArgs* eventArgs)
{
	Control* pReceiverControl;
	if (strReceiver.substr(0, 2) == _T(".\\") || strReceiver.substr(0, 2) == _T("./")) {
		pReceiverControl = ((Box*)this)->FindSubControl(strReceiver.substr(2));
	}
	else {
		pReceiverControl = GetWindow()->FindControl(strReceiver);
	}

	if (pReceiverControl) {
		pReceiverControl->ApplyAttributeList(strList);
	}
	else {
		ASSERT(FALSE);
	}

	return true;
}

void Control::GetImage(Image& duiImage) const
{
	if (duiImage.imageCache) {
		return;
	}
	ui::string sImageName = duiImage.imageAttribute.sImageName;
	ui::string imageFullPath = sImageName;
	if (sImageName.compare(GLOBAL_VSGFILE_KEY) == 0)
		imageFullPath = GlobalManager::GetResourceSvgFile();
	else if (::PathIsRelative(sImageName.c_str())) {
		imageFullPath = GlobalManager::GetResourcePath() + m_pWindow->GetWindowResourcePath() + sImageName;
	}
	imageFullPath = StringHelper::ReparsePath(imageFullPath);

	if (!duiImage.imageCache || 
		duiImage.imageCache->sImageFullPath != imageFullPath || 
		duiImage.imageCache->sImageGroupID != duiImage.imageAttribute.sSvgGroupID) {
		duiImage.imageCache = GlobalManager::GetImage(imageFullPath, duiImage.imageAttribute.sSvgGroupID, duiImage.imageAttribute.dSvgScale);
	}
}

bool Control::DrawImage(IRenderContext* pRender, Image& duiImage, const ui::string& strModify /*= L""*/, int nFade /*= DUI_NOSET_VALUE*/)
{
	// 1. ��ɫ������
	if (!duiImage.imageAttribute.sFillcolor.empty())
	{
		DWORD dwBackColor = GlobalManager::GetTextColor(duiImage.imageAttribute.sFillcolor);
		UiRect rcNewDest = m_rcItem;
		if (duiImage.imageAttribute.rcPadding.left != DUI_NOSET_VALUE)
			rcNewDest.left += duiImage.imageAttribute.rcPadding.left;
		if (duiImage.imageAttribute.rcPadding.right != DUI_NOSET_VALUE)
			rcNewDest.right -= duiImage.imageAttribute.rcPadding.right;
		if (duiImage.imageAttribute.rcPadding.top != DUI_NOSET_VALUE)
			rcNewDest.top += duiImage.imageAttribute.rcPadding.top;
		if (duiImage.imageAttribute.rcPadding.bottom != DUI_NOSET_VALUE)
			rcNewDest.bottom -= duiImage.imageAttribute.rcPadding.bottom;
		if (dwBackColor != 0) {
			if (dwBackColor >= 0xFF000000) pRender->DrawColor(rcNewDest, dwBackColor, nFade);
			else pRender->DrawColor(m_rcItem, dwBackColor, nFade);
		}
	}
	// 2. ͼƬ����
	if (duiImage.imageAttribute.sImageName.empty())
		return false;

	GetImage(duiImage);

	if (!duiImage.imageCache) {
		ASSERT(FALSE);
		duiImage.imageAttribute.Init();
		return false;
	}
	// �ڲ���������
	ImageAttribute newImageAttribute = duiImage.imageAttribute;
	if (!strModify.empty()) {
		ImageAttribute::ModifyAttribute(newImageAttribute, strModify);
	}
	// Dest
	UiRect rcNewDest = m_rcItem;
	if (newImageAttribute.rcDest.left != DUI_NOSET_VALUE && newImageAttribute.rcDest.top != DUI_NOSET_VALUE
		&& newImageAttribute.rcDest.right != DUI_NOSET_VALUE && newImageAttribute.rcDest.bottom != DUI_NOSET_VALUE) {
		rcNewDest.left = m_rcItem.left + newImageAttribute.rcDest.left;
		rcNewDest.right = m_rcItem.left + newImageAttribute.rcDest.right;
		rcNewDest.top = m_rcItem.top + newImageAttribute.rcDest.top;
		rcNewDest.bottom = m_rcItem.top + newImageAttribute.rcDest.bottom;
	}
	// Padding
	if (newImageAttribute.rcPadding.left > DUI_ZERO_VALUE)
		rcNewDest.left += newImageAttribute.rcPadding.left;
	if (newImageAttribute.rcPadding.right > DUI_ZERO_VALUE)
		rcNewDest.right -= newImageAttribute.rcPadding.right;
	if (newImageAttribute.rcPadding.top > DUI_ZERO_VALUE)
		rcNewDest.top += newImageAttribute.rcPadding.top;
	if (newImageAttribute.rcPadding.bottom > DUI_ZERO_VALUE)
		rcNewDest.bottom -= newImageAttribute.rcPadding.bottom;
	// Source
	UiRect rcNewSource = newImageAttribute.rcSource;
	if (rcNewSource.left == DUI_NOSET_VALUE || rcNewSource.top == DUI_NOSET_VALUE
		|| rcNewSource.right == DUI_NOSET_VALUE || rcNewSource.bottom == DUI_NOSET_VALUE) {
		rcNewSource.left = 0;
		rcNewSource.top = 0;
		rcNewSource.right = duiImage.imageCache->nX;
		rcNewSource.bottom = duiImage.imageCache->nY;
	}

	if (m_bkImage.imageCache && m_bkImage.imageCache->IsGif() && m_bGifPlay && !m_bkImage.IsPlaying()) 
	{
		GifPlay();
	}
	else 
	{
		int iFade = nFade == DUI_NOSET_VALUE ? newImageAttribute.bFade : nFade;
		ImageInfo* imageInfo = duiImage.imageCache.get();
		pRender->DrawImage(m_rcPaint, duiImage.GetCurrentHBitmap(), imageInfo->IsAlpha(),
			rcNewDest, rcNewSource, newImageAttribute.rcCorner, iFade, newImageAttribute.bTiledX, newImageAttribute.bTiledY);
	}

	return true;
}

bool Control::DrawSkin(IRenderContext* pRender, Image& duiImage, int nFade /*= DUI_NOSET_VALUE*/)
{
	if (!duiImage.imageCache)
	{
		assert(false);
		return false;
	}
	assert(duiImage.imageCache->nX == m_rcItem.GetWidth());
	assert(duiImage.imageCache->nY == m_rcItem.GetHeight());
	UiRect rcSource = { 0,0,duiImage.imageCache->nX,duiImage.imageCache->nY };
	UiRect rcCorner = { 0,0,0,0 };
	int iFade = nFade == DUI_NOSET_VALUE ? duiImage.imageAttribute.bFade : nFade;
	pRender->DrawImage(m_rcPaint, duiImage.GetCurrentHBitmap(), duiImage.imageCache->IsAlpha(),
		m_rcItem, rcSource, rcCorner, iFade, false, false);
	return true;
}

ui::IRenderContext* Control::GetRenderContext()
{
	if (!m_renderContext) {
		m_renderContext = GlobalManager::CreateRenderContext();
	}
	return m_renderContext.get();
}


void Control::ClearRenderContext()
{
	if (m_renderContext) {
		m_renderContext.reset();
	}
}

void Control::AlphaPaint(IRenderContext* pRender, const UiRect& rcPaint)
{
	if (m_nAlpha == 0) {
		return;
	}

	UiRect rcUnion;
	if( !::IntersectRect(&rcUnion, &rcPaint, &m_rcItem) ) return;

	bool bRoundClip = false;
	if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0) {
		bRoundClip = true;
	}

	if (IsAlpha()) {
		CSize size;
		size.cx = m_rcItem.right - m_rcItem.left;
		size.cy = m_rcItem.bottom - m_rcItem.top;
		auto pCacheRender = GetRenderContext();
		if (pCacheRender) {
			if (pCacheRender->Resize(size.cx, size.cy)) {
				SetCacheDirty(true);
			}

			// IsCacheDirty��m_bCacheDirty���岻һ��
			if (m_bCacheDirty) {
				pCacheRender->Clear();
				UiRect rcClip = { 0, 0, size.cx, size.cy };
				AutoClip alphaClip(pCacheRender, rcClip, m_bClip);
				AutoClip roundAlphaClip(pCacheRender, rcClip, m_cxyBorderRound.cx, m_cxyBorderRound.cy, bRoundClip);

				bool bOldCanvasTrans = m_pWindow->SetRenderTransparent(true);
				CPoint ptOffset(m_rcItem.left + m_renderOffset.x, m_rcItem.top + m_renderOffset.y);
				CPoint ptOldOrg = pCacheRender->OffsetWindowOrg(ptOffset);
				Paint(pCacheRender, m_rcItem);
				PaintChild(pRender, rcPaint);
				pCacheRender->SetWindowOrg(ptOldOrg);
				m_pWindow->SetRenderTransparent(bOldCanvasTrans);
				SetCacheDirty(false);
			}

			pRender->AlphaBlend(rcUnion.left, rcUnion.top, rcUnion.right - rcUnion.left, rcUnion.bottom - rcUnion.top, pCacheRender->GetDC(),
				rcUnion.left - m_rcItem.left, rcUnion.top - m_rcItem.top, rcUnion.right - rcUnion.left, rcUnion.bottom - rcUnion.top, m_nAlpha);

			m_renderContext.reset();
		}
	}
	else if (IsUseCache()) {
		CSize size;
		size.cx = m_rcItem.right - m_rcItem.left;
		size.cy = m_rcItem.bottom - m_rcItem.top;
		auto pCacheRender = GetRenderContext();
		if (pCacheRender) {
			if (pCacheRender->Resize(size.cx, size.cy)) {
				SetCacheDirty(true);
			}

			if (IsCacheDirty()) {
				pCacheRender->Clear();
				UiRect rcClip = { 0, 0, size.cx, size.cy };
				AutoClip alphaClip(pCacheRender, rcClip, m_bClip);
				AutoClip roundAlphaClip(pCacheRender, rcClip, m_cxyBorderRound.cx, m_cxyBorderRound.cy, bRoundClip);

				bool bOldCanvasTrans = m_pWindow->SetRenderTransparent(true);
				CPoint ptOffset(m_rcItem.left + m_renderOffset.x, m_rcItem.top + m_renderOffset.y);
				CPoint ptOldOrg = pCacheRender->OffsetWindowOrg(ptOffset);
				Paint(pCacheRender, m_rcItem);
				pCacheRender->SetWindowOrg(ptOldOrg);
				m_pWindow->SetRenderTransparent(bOldCanvasTrans);
				SetCacheDirty(false);
			}

			pRender->AlphaBlend(rcUnion.left, rcUnion.top, rcUnion.right - rcUnion.left, rcUnion.bottom - rcUnion.top, pCacheRender->GetDC(),
				rcUnion.left - m_rcItem.left, rcUnion.top - m_rcItem.top, rcUnion.right - rcUnion.left, rcUnion.bottom - rcUnion.top, m_nAlpha);
			PaintChild(pRender, rcPaint);
		}
	}
	else {
		AutoClip clip(pRender, m_rcItem, m_bClip);
		AutoClip roundClip(pRender, m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, bRoundClip);
		CPoint ptOldOrg = pRender->OffsetWindowOrg(m_renderOffset);
		Paint(pRender, rcPaint);
		PaintChild(pRender, rcPaint);
		pRender->SetWindowOrg(ptOldOrg);
	}
}

void Control::Paint(IRenderContext* pRender, const UiRect& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	PaintBkColor(pRender);
	PaintBkImage(pRender);
	PaintStatusColor(pRender);
	PaintStatusImage(pRender);
	PaintText(pRender);
	PaintBorder(pRender);
}

void Control::PaintBkColor(IRenderContext* pRender)
{
	if (m_strBkColor.empty()) {
		return;
	}

	DWORD dwBackColor = GlobalManager::GetTextColor(m_strBkColor);
	if(dwBackColor != 0) {
		if (dwBackColor >= 0xFF000000) pRender->DrawColor(m_rcPaint, dwBackColor);
		else pRender->DrawColor(m_rcItem, dwBackColor);
	}
}

void Control::PaintBkImage(IRenderContext* pRender)
{
    DrawImage(pRender, m_bkImage);
}

void Control::PaintStatusColor(IRenderContext* pRender)
{
	m_colorMap.PaintStatusColor(pRender, m_rcPaint, m_uButtonState);
}

void Control::PaintStatusImage(IRenderContext* pRender)
{
	if (m_strSkinName.empty())
	{
		m_imageMap.PaintStatusImage(pRender, kStateImageBk, m_uButtonState);
		m_imageMap.PaintStatusImage(pRender, kStateImageFore, m_uButtonState);
	}
	else
	{
		if (!m_skinImage)
		{
			m_skinImage = GlobalManager::GetStateSkin(m_strSkinName, m_rcItem);
			if (m_skinImage && m_skinImage->HasStateImage(kControlStateHot))
				m_animationManager.SetFadeHot(true);
		}
		if (m_skinImage)
		{
			if (IsFocused() && m_skinImage->HasStateImage(kControlStateFocused))
				m_skinImage->PaintStatusImage(this, pRender, kControlStateFocused);
			else
				m_skinImage->PaintStatusImage(this, pRender, m_uButtonState);
		}	
	}
}

void Control::PaintText(IRenderContext* pRender)
{
    return;
}

void Control::PaintBorder(IRenderContext* pRender)
{
	if (m_strBorderColor.empty()) {
		return;
	}
	DWORD dwBorderColor = 0;
	if (!m_strBorderColor.empty()) {
		dwBorderColor = GlobalManager::GetTextColor(m_strBorderColor);
	}

	if (dwBorderColor != 0) {
		if (m_rcBorderSize.left > 0 || m_rcBorderSize.top > 0 || m_rcBorderSize.right > 0 || m_rcBorderSize.bottom > 0) {
			UiRect rcBorder;
			if (m_rcBorderSize.left > 0) {
				rcBorder = m_rcItem;
				rcBorder.right = rcBorder.left = m_rcItem.left + m_rcBorderSize.left / 2;
				if (m_rcBorderSize.left == 1) {
					rcBorder.bottom -= 1;
				}
				pRender->DrawLine(rcBorder, m_rcBorderSize.left, dwBorderColor);
			}
			if (m_rcBorderSize.top > 0) {
				rcBorder = m_rcItem;
				rcBorder.bottom = rcBorder.top = m_rcItem.top + m_rcBorderSize.top / 2;
				if (m_rcBorderSize.top == 1) {
					rcBorder.right -= 1;
				}
				pRender->DrawLine(rcBorder, m_rcBorderSize.top, dwBorderColor);
			}
			if (m_rcBorderSize.right > 0) {
				rcBorder = m_rcItem;
				rcBorder.left = rcBorder.right = m_rcItem.right - (m_rcBorderSize.right + 1) / 2;
				if (m_rcBorderSize.right == 1) {
					rcBorder.bottom -= 1;
				}
				pRender->DrawLine(rcBorder, m_rcBorderSize.right, dwBorderColor);
			}
			if (m_rcBorderSize.bottom > 0) {
				rcBorder = m_rcItem;
				rcBorder.top = rcBorder.bottom = m_rcItem.bottom - (m_rcBorderSize.bottom + 1) / 2;
				if (m_rcBorderSize.bottom == 1) {
					rcBorder.right -= 1;
				}
				pRender->DrawLine(rcBorder, m_rcBorderSize.bottom, dwBorderColor);
			}
		}
		else if (m_nBorderSize > 0) {
			UiRect rcDraw = m_rcItem;
			int nDeltaValue = m_nBorderSize / 2;
			rcDraw.top += nDeltaValue;
			rcDraw.bottom -= nDeltaValue;
			if (m_nBorderSize % 2 != 0) {
				rcDraw.bottom -= 1;
			}
			rcDraw.left += nDeltaValue;
			rcDraw.right -= nDeltaValue;
			if (m_nBorderSize % 2 != 0) {
				rcDraw.right -= 1;
			}

			if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0)
				pRender->DrawRoundRect(rcDraw, m_cxyBorderRound, m_nBorderSize, dwBorderColor);
			else
				pRender->DrawRect(rcDraw, m_nBorderSize, dwBorderColor);
		}
	}
}

void Control::SetAlpha(int alpha)
{
	ASSERT(alpha >= 0 && alpha <= 255);
	m_nAlpha = alpha;
	Invalidate();
}

void Control::SetHotAlpha(int nHotAlpha)
{
	ASSERT(nHotAlpha >= 0 && nHotAlpha <= 255);
	m_nHotAlpha = nHotAlpha;
	Invalidate();
}

void Control::SetRenderOffset(CPoint renderOffset)
{
	m_renderOffset = renderOffset;
	Invalidate();
}

void Control::SetRenderOffsetX(int renderOffsetX)
{
	m_renderOffset.x = renderOffsetX;
	Invalidate();
}

void Control::SetRenderOffsetY(int renderOffsetY)
{
	m_renderOffset.y = renderOffsetY;
	Invalidate();
}

void Control::GifPlay()
{
	if (!m_bkImage.IsValid() || !m_bkImage.imageCache->IsGif() || !m_bkImage.ContinuePlay()) {
		m_bkImage.SetPlaying(false);
		m_gifWeakFlag.Cancel();
		return;
	}

	if (!m_bkImage.IsPlaying()) {
		m_bkImage.SetCurrentFrame(0);
		m_gifWeakFlag.Cancel();
		int lPause = m_bkImage.GetCurrentInterval();
		if (lPause == 0)
			return;
		m_bkImage.SetPlaying(true);
		auto gifPlayCallback = nbase::Bind(&Control::GifPlay, this);
		TimerManager::GetInstance()->AddCancelableTimer(m_gifWeakFlag.GetWeakFlag(), gifPlayCallback,
			lPause, TimerManager::REPEAT_FOREVER);
	}
	else {
		int lPrePause = m_bkImage.GetCurrentInterval();
		m_bkImage.IncrementCurrentFrame();
		int lPause = m_bkImage.GetCurrentInterval();
		if (!m_bkImage.ContinuePlay())
		{
			StopGifPlayForUI(true, kGifStopLast);
		}
		else
		{
			if (lPrePause == 0 || lPause == 0) {//0��ʾGetCurrentInterval����
				m_bkImage.SetPlaying(false);
				m_gifWeakFlag.Cancel();
				return;
			}

			if (lPrePause != lPause) {
				m_gifWeakFlag.Cancel();
				auto gifPlayCallback = nbase::Bind(&Control::GifPlay, this);
				TimerManager::GetInstance()->AddCancelableTimer(m_gifWeakFlag.GetWeakFlag(), gifPlayCallback,
					lPause, TimerManager::REPEAT_FOREVER);
			}
		}			
	}
	Invalidate();
}

void Control::StopGifPlay(GifStopType frame)
{
	if (m_bkImage.imageCache && m_bkImage.imageCache->IsGif()) {
		m_bkImage.SetPlaying(false);
		m_gifWeakFlag.Cancel();
		int index = GetGifFrameIndex(frame);
		m_bkImage.SetCurrentFrame(index);
		Invalidate();
	}
}

void Control::StartGifPlayForUI(GifStopType frame, int playcount)
{
	GetImage(m_bkImage);
	if (!m_bkImage.IsValid() || !m_bkImage.imageCache->IsGif()) {
		m_bGifPlay = false;
		m_bkImage.SetPlaying(false);
		m_gifWeakFlag.Cancel();
		return;
	}
	if (playcount == 0)
	{
		StopGifPlayForUI(false);
	}		
	else
	{
		m_gifWeakFlag.Cancel();
		m_bGifPlay = true;
		m_bkImage.SetCurrentFrame(GetGifFrameIndex(frame));
		int lPause = m_bkImage.GetCurrentInterval();
		if (lPause == 0) {
			m_bGifPlay = false;
			return;
		}
		m_bkImage.SetPlaying(true);
		m_bkImage.imageAttribute.nPlayCount = playcount;
		m_bkImage.ClearCycledCount();
		auto gifPlayCallback = nbase::Bind(&Control::GifPlay, this);
		TimerManager::GetInstance()->AddCancelableTimer(m_gifWeakFlag.GetWeakFlag(), gifPlayCallback,
			lPause, TimerManager::REPEAT_FOREVER);
		Invalidate();
	}	
}

void Control::StopGifPlayForUI(bool transfer, GifStopType frame)
{
	m_bGifPlay = false;
	StopGifPlay(frame);
	if (transfer)
		BroadcastGifEvent(m_nVirtualEventGifStop);
}
int Control::GetGifFrameIndex(GifStopType frame)
{
	int ret = frame;
	switch (frame)
	{
	case kGifStopCurrent:
		ret = m_bkImage.GetCurrentFrameIndex();
		break;
	case kGifStopFirst:
		ret = 0;
		break;
	case kGifStopLast:
	{
		int nFrameCount = m_bkImage.imageCache->GetFrameCount();
		ret = nFrameCount > 0 ? nFrameCount - 1 : 0;		
	}
	break;
	}
	return ret;
}
void Control::BroadcastGifEvent(int nVirtualEvent)
{
	auto callback = OnGifEvent.find(nVirtualEvent);
	if (callback != OnGifEvent.end()) {
		EventArgs param;
		param.pSender = this;
		callback->second(&param);
	}
}

void Control::InvokeLoadImageCache()
{
	if (m_loadBkImageWeakFlag.HasUsed()) {
		return;
	}
	ui::string sImageName = m_bkImage.imageAttribute.sImageName;
	if (sImageName.empty()) {
		return;
	}
	ui::string imageFullPath = sImageName;
	if (::PathIsRelative(sImageName.c_str())) {
		imageFullPath = GlobalManager::GetResourcePath() + m_pWindow->GetWindowResourcePath() + sImageName;
	}
	imageFullPath = StringHelper::ReparsePath(imageFullPath);

	if (!m_bkImage.imageCache || m_bkImage.imageCache->sImageFullPath != imageFullPath) {
		auto shared_image = GlobalManager::IsImageCached(imageFullPath, m_bkImage.imageAttribute.sSvgGroupID);
		if (shared_image) {
			m_bkImage.imageCache = shared_image;
			return;
		}
	}
}

void Control::UnLoadImageCache()
{
	m_loadBkImageWeakFlag.Cancel();
	m_bkImage.ClearCache();
}

void Control::ClearImageCache()
{
	m_imageMap.ClearCache();
	m_bkImage.ClearCache();
}

void Control::DetachEvent(EventType type)
{
	auto event = OnEvent.find(type);
	if (event != OnEvent.end())
	{
		OnEvent.erase(event);
	}
}

} // namespace ui
