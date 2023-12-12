#include "stdafx.h"
#include "ChildBox.h"

namespace ui
{

ChildBox::ChildBox()
{

}

void ChildBox::Init()
{
	if (!m_strXMLFile.empty())
	{
		Box* pChildWindow = static_cast<Box*>(GlobalManager::CreateBoxWithCache(m_strXMLFile.c_str(), CreateControlCallback()));
		if (pChildWindow) {
			this->Add(pChildWindow);
		}
		else {
			ASSERT(FALSE);
			this->RemoveAll();
		}
	}
}

void ChildBox::SetAttribute( const::ui::string& strName, const ui::string& strValue )
{
	if( strName == ATTR_CHILDBOX_xmlfile)
		SetChildLayoutXML(strValue);
	else
		Box::SetAttribute(strName,strValue);
}

void ChildBox::SetChildLayoutXML(ui::string strXML )
{
	m_strXMLFile = strXML;
}

ui::string ChildBox::GetChildLayoutXML()
{
	return m_strXMLFile;
}

} // namespace ui
