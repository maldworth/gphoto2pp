#include "non_value_widget.hpp"

#include "helper_gphoto2.hpp"
#include "exceptions.hpp"

#include "log.h"

namespace gphoto2
{
#include <gphoto2/gphoto2-widget.h>
}

namespace gphoto2pp
{

	NonValueWidget::NonValueWidget(gphoto2::_CameraWidget* cameraWidget)
		: CameraWidgetWrapper(cameraWidget)
	{
		FILE_LOG(logINFO) << "NonValueWidget constructor - widget";	
	}
	
	int NonValueWidget::countChildren() const
	{
		FILE_LOG(logDEBUG) << "NonValueWidget countChildren";
		
		return gphoto2pp::checkResponse(gphoto2::gp_widget_count_children(m_cameraWidget),"gp_widget_count_children");
	}
	
	// Protected Methods, this call is done here so we don't have to put any gphoto includes in the header hpp files.
	gphoto2::_CameraWidget* NonValueWidget::getChildByNameWrapper(const std::string& name) const
	{
		gphoto2::_CameraWidget* childWidget;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_child_by_name(m_cameraWidget, name.c_str(), &childWidget),"gp_widget_get_child_by_name");
		return childWidget;
	}
	
	gphoto2::_CameraWidget* NonValueWidget::getChildByLabelWrapper(const std::string& label) const
	{
		gphoto2::_CameraWidget* childWidget;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_child_by_label(m_cameraWidget, label.c_str(), &childWidget),"gp_widget_get_child_by_label");
		return childWidget;
	}
	
	gphoto2::_CameraWidget* NonValueWidget::getChildWrapper(int index) const
	{
		if( index >= countChildren())
		{
			throw exceptions::IndexOutOfRange("You are trying to get a child widget at an index which is greater than the maximum index.");
		}
		
		gphoto2::_CameraWidget* childWidget;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_child(m_cameraWidget, index, &childWidget),"gp_widget_get_child");
		return childWidget;
	}
	
	gphoto2::_CameraWidget* NonValueWidget::getChildByIdWrapper(int id) const
	{
		gphoto2::_CameraWidget* childWidget;
		gphoto2pp::checkResponse(gphoto2::gp_widget_get_child_by_id(m_cameraWidget, id, &childWidget),"gp_widget_get_child_by_id");
		return childWidget;
	}
}

