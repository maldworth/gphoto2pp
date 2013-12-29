#include "camera_wrapper.hpp"

#include "exceptions.hpp"

#include "helper_gphoto2.hpp"
#include "helper_context.hpp"
#include "window_widget.hpp"
#include "camera_abilities_list_wrapper.hpp"
#include "camera_list_wrapper.hpp"
#include "gp_port_info_list_wrapper.hpp"

#include "camera_file_wrapper.hpp"
#include "camera_file_path_wrapper.hpp"
#include "camera_event_type_wrapper.hpp"
#include "camera_capture_type_wrapper.hpp"

#include "log.h"

namespace gphoto2
{
#include <gphoto2/gphoto2-camera.h>
#ifndef GPHOTO_LESS_25
#include <gphoto2/gphoto2-abilities-list.h> // Only needed for the pre 2.5 initialize method (because _autodetect doesn't exist)
#endif
#include <gphoto2/gphoto2-file.h>
}

#include <fstream>
#include <utility>

namespace gphoto2pp
{

	CameraWrapper::CameraWrapper(const std::string& model, const std::string& port)
		: m_camera(nullptr)
		, m_context(gphoto2pp::getContext())
		, m_model(model)
		, m_port(port)
		, m_listenForEvents(false)
	{
		FILE_LOG(logINFO) << "CameraWrapper Constructor - model[" << model.c_str() << "], port[" << port.c_str() << "]";
		
		gphoto2pp::checkResponse(gphoto2::gp_camera_new(&m_camera),"gp_camera_new");
		
		initialize(m_model, m_port);
		
		FILE_LOG(logDEBUG) << "CameraWrapper Constructor end - params";
	}

	CameraWrapper::CameraWrapper()
		: m_camera(nullptr)
		, m_context(gphoto2pp::getContext())
		, m_model()
		, m_port()
		, m_listenForEvents(false)
	{
		FILE_LOG(logINFO) << "CameraWrapper Constructor";
		
		gphoto2pp::checkResponse(gphoto2::gp_camera_new(&m_camera),"gp_camera_new");
		
		initialize();
		
		FILE_LOG(logDEBUG) << "CameraWrapper Constructor end - noparams";
	}

	CameraWrapper::~CameraWrapper()
	{
		FILE_LOG(logINFO) << "~CameraWrapper Destructor";
		
		stopListeningForEvents();
		
		if(m_camera != nullptr)
		{
			FILE_LOG(logINFO) << "CameraWrapper exit";
			gphoto2pp::checkResponse(gphoto2::gp_camera_unref(m_camera),"gp_camera_unref");
			m_camera = nullptr;
		}
	}
	
	CameraWrapper::CameraWrapper(CameraWrapper&& other)
		: m_camera(other.m_camera)
		, m_context(other.m_context)
		, m_model(std::move(other.m_model))
		, m_port(std::move(other.m_port))
		, m_listenForEvents(other.m_listenForEvents.load())
	{
		FILE_LOG(logINFO) << "CameraWrapper move Constructor";
		
		m_listenForEvents = other.m_listenForEvents.load();
		
		// We cannot transfer the thread atomics over, so we have to stop listening in the previous class and start in the next one.
		other.stopListeningForEvents();
		
		m_cameraEvents = std::move(other.m_cameraEvents);
		
		// If the other CameraWrapper was listening to events, then we start listening to events here.
		if(m_listenForEvents)
			startListeningForEvents();
		
		other.m_camera = nullptr;
	}
	
	CameraWrapper& CameraWrapper::operator=(CameraWrapper&& other)
	{
		FILE_LOG(logINFO) << "CameraWrapper move assignment operator";
		
		if(this != &other)
		{
			// Stops current objects listening for events (it might not be, but better safe than sorry)
			stopListeningForEvents();
			
			// Release current objects resource
			if(m_camera != nullptr)
			{
				gphoto2pp::checkResponse(gphoto2::gp_camera_unref(m_camera),"gp_camera_unref");
			}
			
			// Steal or "move" the other objects resource
			m_camera = other.m_camera;
			m_context = other.m_context;
			m_model = std::move(other.m_model);
			m_port = std::move(other.m_port);
			
			m_listenForEvents = other.m_listenForEvents.load();
			
			// We cannot transfer the thread atomics over, so we have to stop listening in the previous class and start in the next one.
			other.stopListeningForEvents();
			
			m_cameraEvents = std::move(other.m_cameraEvents);
			
			// If the other CameraWrapper was listening to events, then we start listening to events here.
			if(m_listenForEvents)
				startListeningForEvents();
			
			// Unreference the other objects resource, so it's destructor doesn't unreference it
			other.m_camera = nullptr;
		}
		return *this;
	}

	void CameraWrapper::initialize()
	{
		gphoto2pp::checkResponse(gphoto2::gp_camera_init(m_camera, m_context.get()),"gp_camera_init");
	}

	void CameraWrapper::initialize(const std::string& model, const std::string& port)
	{
		FILE_LOG(logDEBUG) << "Initializing Camera on Port: " << port << " with Model: " << model;
		
		CameraAbilitiesListWrapper cameraAbilitiesList;
		int model_index = cameraAbilitiesList.lookupModel(model);
		
		GPPortInfoListWrapper portInfoList;
		int port_index = portInfoList.lookupPath(port);
		
		// I didn't wrap this because its only used in this one place, and the comments say its public use is "questionable"
		gphoto2::CameraAbilities cameraAbilities;
		gphoto2pp::checkResponse(gphoto2::gp_abilities_list_get_abilities(cameraAbilitiesList.getPtr(), model_index, &cameraAbilities),"gp_abilities_list_get_abilities");
		
		// Sets camera abilities, probably should make this a member method
		gphoto2pp::checkResponse(gphoto2::gp_camera_set_abilities(m_camera, cameraAbilities),"gp_camera_set_abilities");
		
		gphoto2::GPPortInfo portInfo;
		gphoto2pp::checkResponse(gphoto2::gp_port_info_list_get_info(portInfoList.getPtr(), port_index, &portInfo),"gp_port_info_list_get_info");
		
		// Sets camera port info, probably should make this a member method
		gphoto2pp::checkResponse(gphoto2::gp_camera_set_port_info(m_camera, portInfo),"gp_camera_set_port_info");
		
		// Finally, init the camera
		gphoto2pp::checkResponse(gphoto2::gp_camera_init(m_camera, m_context.get()),"gp_camera_init");
	}

	std::string CameraWrapper::getSummary() const
	{
		gphoto2::CameraText text;
		
		m_cameraIOMutex.lock();
		try
		{
			gphoto2pp::checkResponse(gphoto2::gp_camera_get_summary(m_camera, &text, m_context.get()),"gp_camera_get_summary");
		}
		catch (...)
		{
			m_cameraIOMutex.unlock();
			
			throw;
		}
		m_cameraIOMutex.unlock();
		
		return std::string(text.text);
	}

	CameraFileWrapper CameraWrapper::capturePreview()
	{
		FILE_LOG(logDEBUG) << "CameraWrapper capturePreview";
		
		CameraFileWrapper cameraFile;
		
		m_cameraIOMutex.lock();
		try
		{
			gphoto2pp::checkResponse(gphoto2::gp_camera_capture_preview(m_camera, cameraFile.getPtr(), m_context.get()),"gp_camera_capture_preview");
		}
		catch (...)
		{
			m_cameraIOMutex.unlock();
			
			throw;
		}
		m_cameraIOMutex.unlock();
		
		return cameraFile;
	}

	CameraFilePathWrapper CameraWrapper::capture(const CameraCaptureTypeWrapper& captureType)
	{
		FILE_LOG(logDEBUG) << "CameraWrapper capture";
		
		gphoto2::CameraFilePath cameraFilePath; // No wrapper made for this struct because it doesn't have an api to manipulate (similar to getSummary CameraText)
		
		// TODO, canon capture needs to enable the toggle widget "capture" with a value of 1, or 0 for off. I will need too get my hands on a canon and implement this.
		
		m_cameraIOMutex.lock();
		try
		{
			gphoto2pp::checkResponse(gphoto2::gp_camera_capture(m_camera, static_cast<gphoto2::CameraCaptureType>(captureType),  &cameraFilePath, m_context.get()),"gp_camera_capture");
		}
		catch (...)
		{
			m_cameraIOMutex.unlock();
			
			throw;
		}
		m_cameraIOMutex.unlock();
		
		FILE_LOG(logDEBUG) << "Pathname on the camera: '" << cameraFilePath.folder << "/" << cameraFilePath.name << "'";
		
		return gphoto2pp::CameraFilePathWrapper{cameraFilePath.name, cameraFilePath.folder};
	}
	
	void CameraWrapper::triggerCapture()
	{
#ifdef GPHOTO_LESS_25
		throw exceptions::InvalidLinkedVersionException("You are using a version of gphoto2 that doesn't support this command. Please link to gphoto 2.5 or greater");
#else
		FILE_LOG(logDEBUG) << "CameraWrapper triggerCapture";
		
		m_cameraIOMutex.lock();
		try
		{
			gphoto2pp::checkResponse(gphoto2::gp_camera_trigger_capture(m_camera, m_context.get()),"gp_camera_trigger_capture");
		}
		catch (...)
		{
			m_cameraIOMutex.unlock();
			
			throw;
		}
		m_cameraIOMutex.unlock();
#endif
	}


	WindowWidget CameraWrapper::getConfig() const
	{
		FILE_LOG(logDEBUG) << "CameraWrapper getConfig";
		
		gphoto2::CameraWidget* cameraWidget = nullptr;
		
		m_cameraIOMutex.lock();
		try
		{
			gphoto2pp::checkResponse(gphoto2::gp_camera_get_config(m_camera, &cameraWidget, m_context.get()),"gp_camera_get_config");
		}
		catch (...)
		{
			m_cameraIOMutex.unlock();
			
			throw;
		}
		m_cameraIOMutex.unlock();
		
		auto rootWidget = WindowWidget(cameraWidget);
		rootWidget.unref(); // This will have an extra ref added because of the constructor. And so we need to remove it.
		return std::move(rootWidget);
	}

	void CameraWrapper::setConfig(const CameraWidgetWrapper& cameraWidget)
	{
		FILE_LOG(logDEBUG) << "CameraWrapper setConfig - cameraWidget";
		
		auto rootWidget = cameraWidget.getRoot();
		
		m_cameraIOMutex.lock();
		try
		{
			gphoto2pp::checkResponse(gphoto2::gp_camera_set_config(m_camera, rootWidget.getPtr(), m_context.get()),"gp_camera_set_config"); // we can use cameraWidget->m_cameraWidget because this is a friend class of the camera_widget_wrapper
		}
		catch (...)
		{
			m_cameraIOMutex.unlock();
			
			throw;
		}
		m_cameraIOMutex.unlock();
	}

	bool CameraWrapper::startListeningForEvents()
	{
		FILE_LOG(logDEBUG) << "CameraWrapper startListeningForEvents";
		
		if(m_listenForEventSignalFuture.valid() == true)
		{
			// This means we already have a promise and so the thread must already be started
			FILE_LOG(logWARN1) << "Already listening to events";
			return true;
		}
		
		// Set to true so our thread loop will repeat until set back
		m_listenForEvents = true;
					
		std::packaged_task<bool()> event_task([this](){
			FILE_LOG(logINFO) << "Starting to listen for camera events";
			
			while(m_listenForEvents == true)
			{
				gphoto2::CameraEventType eventType;
				void* eventData;
				
				m_cameraIOMutex.lock();
				try
				{
					gphoto2pp::checkResponse(gphoto2::gp_camera_wait_for_event(m_camera, 400, &eventType, &eventData, m_context.get()),"gp_camera_wait_for_event");
				}
				catch (...)
				{
					FILE_LOG(logCRITICAL) << "An Exception was thrown when waiting for camera events. The Camera possibly lost connection with the computer";
					
					m_cameraIOMutex.unlock();
					
					return false;
				}
				m_cameraIOMutex.unlock();
				
				FILE_LOG(logDEBUG) << "EventType Received: '" << static_cast<int>(eventType) << "'";
				
				switch(eventType)
				{
					case gphoto2::GP_EVENT_UNKNOWN:
					{
						if(eventData)
						{
							FILE_LOG(logDEBUG) << "Listener - Unknown Has Data";
							m_cameraEvents(static_cast<CameraEventTypeWrapper>(eventType), CameraFilePathWrapper{"",""}, std::string((char*)eventData));
							break; // Break out of case
						}
						FILE_LOG(logDEBUG) << "Listener - Unknown Fallthrough";
					}
					case gphoto2::GP_EVENT_TIMEOUT:
					case gphoto2::GP_EVENT_CAPTURE_COMPLETE:
					{
						FILE_LOG(logDEBUG) << "Listener - Capture Complete or Timeout";
						m_cameraEvents(static_cast<CameraEventTypeWrapper>(eventType), CameraFilePathWrapper{"",""}, std::string("No Event Data Returned"));
						break;
					}
					case gphoto2::GP_EVENT_FILE_ADDED:
					case gphoto2::GP_EVENT_FOLDER_ADDED:
					{
						FILE_LOG(logDEBUG) << "Listener - Folder or File Added";
						gphoto2::CameraFilePath* cameraFilePath = (gphoto2::CameraFilePath*)eventData;
						m_cameraEvents(static_cast<CameraEventTypeWrapper>(eventType), CameraFilePathWrapper{cameraFilePath->name, cameraFilePath->folder}, std::string(""));
						break;
					}
					default:
					{
						FILE_LOG(logWARN) << "Un-recognized EventType Fired: '" << eventType << "'";
						break;
					}
				}
			}
			
			FILE_LOG(logDEBUG) << "Loop has stopped gracefully, returning...";
			
			return true;
		});
		
		m_listenForEventSignalFuture = event_task.get_future();
		
		std::thread(std::move(event_task)).detach();

		return true;
	}
	
	observer::Registration CameraWrapper::subscribeToCameraEvent(const CameraEventTypeWrapper& event, std::function<void(const CameraFilePathWrapper&, const std::string&)> func)
	{
		return m_cameraEvents.registerObserver(event, std::move(func));
	}
	
	void CameraWrapper::stopListeningForEvents()
	{
		FILE_LOG(logDEBUG) << "stopping event listener (if started)";
		
		if(m_listenForEventSignalFuture.valid())
		{
			FILE_LOG(logDEBUG) << "listener is running, we will signal it to stop";
			
			// We are already listening to events, and so we signal them to stop
			m_listenForEvents = false;
			
			// TODO probably make this a wait_for with a timeout, so we cannot freeze anything...
			m_listenForEventSignalFuture.wait();
			
			FILE_LOG(logDEBUG) << "listener just stopped";
		}
		else
		{
			FILE_LOG(logDEBUG) << "listener is already stopped";
		}
	}
	
	CameraListWrapper CameraWrapper::folderListFiles(const std::string& folder) const
	{
		CameraListWrapper cameraList;
		
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_list_files(m_camera, folder.c_str(), cameraList.getPtr(), m_context.get()),"gp_camera_folder_list_files");
		
		return cameraList;
	}
	
	CameraListWrapper CameraWrapper::folderListFolders(const std::string& folder) const
	{
		CameraListWrapper cameraList;
		
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_list_folders(m_camera, folder.c_str(), cameraList.getPtr(), m_context.get()),"gp_camera_folder_list_folders");
		
		return cameraList;
	}
	
	
	
	void CameraWrapper::folderDeleteAll(const std::string& folder)
	{
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_delete_all(m_camera, folder.c_str(), m_context.get()),"gp_camera_folder_delete_all");
	}
	
	void CameraWrapper::folderPutFile(const std::string& folder, const std::string& fileName, const CameraFileTypeWrapper& fileType, CameraFileWrapper cameraFile)
	{
#ifdef GPHOTO_LESS_25
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_put_file(m_camera, folder.c_str(), cameraFile.getPtr(), m_context.get()),"gp_camera_folder_put_file");
#else
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_put_file(m_camera, folder.c_str(), fileName.c_str(), static_cast<gphoto2::CameraFileType>(fileType), cameraFile.getPtr(), m_context.get()),"gp_camera_folder_put_file");
#endif
	}
	
	void CameraWrapper::folderMakeDir(const std::string& folder, const std::string& name)
	{
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_make_dir(m_camera, folder.c_str(), name.c_str(), m_context.get()),"gp_camera_folder_make_dir");
	}
	
	void CameraWrapper::folderRemoveDir(const std::string& folder, const std::string& name)
	{
		gphoto2pp::checkResponse(gphoto2::gp_camera_folder_remove_dir(m_camera, folder.c_str(), name.c_str(), m_context.get()),"gp_camera_folder_remove_dir");
	}
	
	CameraFileWrapper CameraWrapper::fileGet(const std::string& folder, const std::string& fileName, const CameraFileTypeWrapper& fileType) const
	{
		CameraFileWrapper cameraFileWrapper;
		
		gphoto2pp::checkResponse(gphoto2::gp_camera_file_get(m_camera, folder.c_str(), fileName.c_str(), static_cast<gphoto2::CameraFileType>(fileType), cameraFileWrapper.getPtr(), m_context.get()),"gp_camera_file_get");
		
		return cameraFileWrapper;
	}
	
	void CameraWrapper::fileDelete(const std::string& folder, const std::string& fileName) const
	{
		gphoto2pp::checkResponse(gphoto2::gp_camera_file_delete(m_camera, folder.c_str(), fileName.c_str(), m_context.get()),"gp_camera_file_delete");
	}
}

