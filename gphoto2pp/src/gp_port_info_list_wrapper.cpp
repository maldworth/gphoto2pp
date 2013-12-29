#include "gp_port_info_list_wrapper.hpp"

#include "helper_gphoto2.hpp"

#include "log.h"

namespace gphoto2
{
#include <gphoto2/gphoto2-port-info-list.h>
}

namespace gphoto2pp
{

	GPPortInfoListWrapper::GPPortInfoListWrapper()
	{
		FILE_LOG(logINFO) << "GPPortInfoListWrapper Constructor";

		gphoto2pp::checkResponse(gphoto2::gp_port_info_list_new(&m_portInfoList),"gp_port_info_list_new");
		
		gphoto2pp::checkResponse(gphoto2::gp_port_info_list_load(m_portInfoList),"gp_port_info_list_load");
	}

	GPPortInfoListWrapper::~GPPortInfoListWrapper()
	{
		FILE_LOG(logINFO) << "GPPortInfoListWrapper Destructor";
		
		// Destructors should never throw exceptions. Hence the silent response
		gphoto2pp::checkResponseSilent(gphoto2::gp_port_info_list_free(m_portInfoList),"gp_port_info_list_free");
	}
	
	gphoto2::_GPPortInfoList* GPPortInfoListWrapper::getPtr() const
	{
		return m_portInfoList;
	}

	int GPPortInfoListWrapper::lookupPath(const std::string& path) const
	{
		FILE_LOG(logDEBUG) << "GPPortInfoListWrapper lookupPath - path["<< path << "]";
		
		return gphoto2pp::checkResponse(gphoto2::gp_port_info_list_lookup_path(m_portInfoList, path.c_str()),"gp_port_info_list_lookup_path");
	}
	
	int GPPortInfoListWrapper::lookupName(const std::string& name) const
	{
		FILE_LOG(logDEBUG) << "GPPortInfoListWrapper lookupName - name["<< name << "]";
		
		return gphoto2pp::checkResponse(gphoto2::gp_port_info_list_lookup_name(m_portInfoList, name.c_str()),"gp_port_info_list_lookup_name");
	}
	
	int GPPortInfoListWrapper::count() const
	{
		return gphoto2pp::checkResponse(gphoto2::gp_port_info_list_count(m_portInfoList),"gp_port_info_list_count");
	}

}

