#include "camera_abilities_list_wrapper.hpp"

#include "helper_gphoto2.hpp"
#include "helper_context.hpp"
#include "camera_list_wrapper.hpp"
#include "gp_port_info_list_wrapper.hpp"

#include "log.h"

namespace gphoto2
{
#include <gphoto2/gphoto2-abilities-list.h>
}

namespace gphoto2pp
{

	CameraAbilitiesListWrapper::CameraAbilitiesListWrapper()
		: m_cameraAbilitiesList(nullptr)
	{
		FILE_LOG(logINFO) << "CameraAbilitiesListWrapper Constructor";
		
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_new(&m_cameraAbilitiesList),"gp_abilities_list_new");
		
		auto spContext = gphoto2pp::getContext();
		
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_load(m_cameraAbilitiesList, spContext.get()),"gp_abilities_list_load");
	}

	CameraAbilitiesListWrapper::~CameraAbilitiesListWrapper()
	{
		FILE_LOG(logINFO) << "~CameraAbilitiesListWrapper Destructor";
		
		if(m_cameraAbilitiesList != nullptr)
		{
			gphoto2pp::checkResponseSilent(gphoto2::gp_abilities_list_free(m_cameraAbilitiesList),"gp_abilities_list_free");
			m_cameraAbilitiesList = nullptr;
		}	
	}
	
	CameraAbilitiesListWrapper::CameraAbilitiesListWrapper(CameraAbilitiesListWrapper&& other)
		: m_cameraAbilitiesList(other.m_cameraAbilitiesList)
	{
		FILE_LOG(logINFO) << "CameraAbilitiesListWrapper move Constructor";
		
		other.m_cameraAbilitiesList = nullptr;
	}
	
	CameraAbilitiesListWrapper& CameraAbilitiesListWrapper::operator=(CameraAbilitiesListWrapper&& other)
	{
		FILE_LOG(logINFO) << "CameraAbilitiesListWrapper move assignment operator";
		
		if(this != &other)
		{
			// Release current objects resource
			if(m_cameraAbilitiesList != nullptr)
			{
				FILE_LOG(logINFO) << "CameraAbilitiesListWrapper move assignment - current abilities is not null";
				gphoto2pp::checkResponse(gphoto2::gp_abilities_list_free(m_cameraAbilitiesList),"gp_abilities_list_free");
			}
			
			// Steal or "move" the other objects resource
			m_cameraAbilitiesList = other.m_cameraAbilitiesList;
			
			// Unreference the other objects resource, so it's destructor doesn't unreference it
			other.m_cameraAbilitiesList = nullptr;
		}
		return *this;
	}
	
	gphoto2::_CameraAbilitiesList* CameraAbilitiesListWrapper::getPtr() const
	{
		return m_cameraAbilitiesList;
	}

	CameraListWrapper CameraAbilitiesListWrapper::listDetect(const GPPortInfoListWrapper& portInfoList)
	{
		FILE_LOG(logDEBUG) << "CameraAbilitiesListWrapper listDetect - portInfoList";
		
		auto spContext = gphoto2pp::getContext();
		CameraListWrapper cameraList;
		
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_detect(m_cameraAbilitiesList, portInfoList.getPtr(), cameraList.getPtr(), spContext.get()),"gp_abilities_list_detect");
		
		return cameraList; // Compiler should do RVO here
	}

	void CameraAbilitiesListWrapper::reset()
	{
		FILE_LOG(logDEBUG) << "CameraAbilitiesListWrapper reset";
		
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_reset(m_cameraAbilitiesList),"gp_abilities_list_reset");
	}

	int CameraAbilitiesListWrapper::count() const
	{
		FILE_LOG(logDEBUG) << "CameraAbilitiesListWrapper count";
		
		return gphoto2pp::checkResponse(gphoto2::gp_abilities_list_count(m_cameraAbilitiesList),"gp_abilities_list_count");
	}

	// Meant to return the index of the specified model, or gphoto error code
	int CameraAbilitiesListWrapper::lookupModel(const std::string& model) const
	{
		FILE_LOG(logDEBUG) << "CameraAbilitiesListWrapper lookupModel - model";
		
		return gphoto2pp::checkResponse(gphoto2::gp_abilities_list_lookup_model(m_cameraAbilitiesList, model.c_str()),"gp_abilities_list_lookup_model");
	}

}

