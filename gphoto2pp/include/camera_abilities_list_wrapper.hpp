#ifndef CAMERAABILITIESLISTWRAPPER_H
#define CAMERAABILITIESLISTWRAPPER_H

#include <string>

namespace gphoto2
{
	// Forward Declaration
	struct _CameraAbilitiesList;
}

namespace gphoto2pp
{
	class GPPortInfoListWrapper;
	class CameraListWrapper;
	
	/**
	 * \class CameraAbilitiesListWrapper
	 * A wrapper around the gphoto2 CameraAbilitiesList struct.
	 */
	class CameraAbilitiesListWrapper
	{
	public:
		CameraAbilitiesListWrapper();
		~CameraAbilitiesListWrapper();
		
		// Move constructor and move assignment are allowed
		CameraAbilitiesListWrapper(CameraAbilitiesListWrapper&& other);
		CameraAbilitiesListWrapper& operator=(CameraAbilitiesListWrapper&& other);
		
		// We cannot support copy constructor or assignment at this time
		CameraAbilitiesListWrapper(const CameraAbilitiesListWrapper& other) = delete;
		CameraAbilitiesListWrapper& operator=( const CameraAbilitiesListWrapper& ) = delete;
		
		gphoto2::_CameraAbilitiesList* getPtr() const;
		
		/**
		 * \brief Tries to detect any cameras connected to the computer using the list of ports provided
		 * \param[in]	portInfoList	of ports to scan for camera models
		 * \note Direct wrapper for <tt>gp_abilities_list_detect(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		CameraListWrapper listDetect(const GPPortInfoListWrapper& portInfoList);
		
		/**
		 * \brief Resets the abilities list
		 * \note Direct wrapper for <tt>gp_abilities_list_reset(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void reset();
		
		/**
		 * \brief Counts the entries in the abilities list
		 * \return he number of items in the list
		 * \note Direct wrapper for <tt>gp_abilities_list_count(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int count() const;
		
		/**
		 * \brief Scans the list for the given model
		 * \param[in]	model	of camera to search for
		 * \return the index of the item (if found)
		 * \note Direct wrapper for <tt>gp_abilities_list_count(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int lookupModel(const std::string& model) const;
		
	private:
		gphoto2::_CameraAbilitiesList* m_cameraAbilitiesList;
	};
}

#endif // CAMERAABILITIESLISTWRAPPER_H
