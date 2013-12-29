#ifndef CAMERALISTWRAPPER_HPP
#define CAMERALISTWRAPPER_HPP

#include <string>

namespace gphoto2
{
	struct _CameraList;
}

namespace gphoto2pp
{
	/**
	 * \class CameraFileWrapper
	 * A wrapper around the gphoto2 CameraFile struct.
	 */
	class CameraListWrapper
	{
	public:
		CameraListWrapper();
		~CameraListWrapper();
		
		// Move constructor and move assignment are allowed
		CameraListWrapper(CameraListWrapper&& other);
		CameraListWrapper& operator=(CameraListWrapper&& other);
		
		CameraListWrapper(const CameraListWrapper& other);
		CameraListWrapper& operator=(const CameraListWrapper& other);
		
		gphoto2::_CameraList* getPtr() const;
		
		/**
		 * \brief Gets the number of cameras contained in the list
		 * \return the number of cameras
		 * \note Direct wrapper for <tt>gp_list_count(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int count() const;
		
		/**
		 * \brief Gets the model name of the camera at the index
		 * \param[in]	index	of the model name to get
		 * \return the camera model name
		 * \note Direct wrapper for <tt>gp_list_get_name(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getName(int index) const;
		
		/**
		 * \brief Gets the port of the camera at the index
		 * \param[in]	index	of the port value to get
		 * \return the port which the camera is connected to
		 * \note Direct wrapper for <tt>gp_list_get_value(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::string getValue(int index) const;
		
		/**
		 * \brief Sets the camera model name at the specified index
		 * \param[in]	index	of the name to set
		 * \param[in]	name	of the model to set
		 * \note Direct wrapper for <tt>gp_list_set_name(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setName(int index, const std::string& name);
		
		/**
		 * \brief Sets the camera port at the specified index
		 * \param[in]	index	of the port to set
		 * \param[in]	value	of the port name to set
		 * \note Direct wrapper for <tt>gp_list_set_value(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void setValue(int index, const std::string& value);
		
		/**
		 * \brief Adds a new camera model and port pair
		 * \param[in]	name	of the camera model
		 * \param[in]	value	of the port connected to the camera
		 * \note Direct wrapper for <tt>gp_list_append(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void append(const std::string& name, const std::string& value);
		
		/**
		 * \brief Erases all model and port pairs in the current list.
		 * \note Direct wrapper for <tt>gp_list_reset(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void reset();
		
		/**
		 * \brief Sorts the list based on the name (camera model)
		 * \note Direct wrapper for <tt>gp_list_sort(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		void sort();
		
		/**
		 * \brief Sorts the list based on the name (camera model)
		 * \param[in]	name	of the model to search for
		 * \note Direct wrapper for <tt>gp_list_sort(...)</tt>
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		int findByName(const std::string& name) const;
		
		/**
		 * \brief Gets the name (model) and value (port) pair
		 * \param[in]	index	of the model/port pair to get
		 * \return the model/port pair
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::pair<std::string, std::string> getPair(int index) const;
		
		/**
		 * \brief Gets the name (model) and value (port) pair by searching the name
		 * \param[in]	name	of the model to search for
		 * \return the model\port pair
		 * \throw GPhoto2pp::exceptions::gphoto2_exception
		 */
		std::pair<std::string, std::string> getPairByName(const std::string& name) const;

	private:
		gphoto2::_CameraList* m_cameraList;
	};
}

#endif // CAMERALISTWRAPPER_HPP
