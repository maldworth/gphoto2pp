/** \file 
 * \author Copyright (c) 2013 maldworth <https://github.com/maldworth>
 *
 * \note
 * This file is part of gphoto2pp
 * 
 * \note
 * gphoto2pp is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * \note
 * gphoto2pp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * \note
 * You should have received a copy of the GNU Lesser General Public
 * License along with gphoto2pp.
 * If not, see http://www.gnu.org/licenses
 */

#include <gphoto2pp/helper_camera_wrapper.hpp>

#include <gphoto2pp/helper_gphoto2.hpp>
#include <gphoto2pp/exceptions.hpp>
#include <gphoto2pp/camera_list_wrapper.hpp>
#include <gphoto2pp/camera_wrapper.hpp>
#include <gphoto2pp/camera_file_wrapper.hpp>
#include <gphoto2pp/camera_file_path_wrapper.hpp>

#include <sstream>

namespace gphoto2pp
{
	namespace helper
	{
		void capturePreview(CameraWrapper& cameraWrapper, std::string const & outputFilename)
		{
			CameraFileWrapper cameraFile = cameraWrapper.capturePreview(); // No point in duplicating code, call overloaded method
			
			cameraFile.save(outputFilename);
		}

		void capturePreview(CameraWrapper& cameraWrapper, std::ostream& outputStream)
		{
			CameraFileWrapper cameraFile = cameraWrapper.capturePreview(); // No point in duplicating code, call overloaded method
			
			auto buffer = cameraFile.getDataAndSize();
			
			outputStream.write(buffer.data(), buffer.size());
			
			outputStream.flush();
			// Not sure if I should close the file in here, or let the user do it. I'll let the user do it for now.
		}
		
		void capture(CameraWrapper& cameraWrapper, CameraFileWrapper& cameraFile, bool autoDeleteImageFromSrc /* = false */, CameraCaptureTypeWrapper const & captureType /* = Image */, CameraFileTypeWrapper const & fileType /* = Normal */)
		{
			auto cameraFilePath = cameraWrapper.capture(captureType);
			
			// This should be the move assignment operator
			cameraFile = cameraWrapper.fileGet(cameraFilePath.Folder, cameraFilePath.Name, fileType);
			
			if(autoDeleteImageFromSrc)
			{
				cameraWrapper.fileDelete(cameraFilePath.Folder, cameraFilePath.Name);
			}
		}
		
		void capture(CameraWrapper& cameraWrapper, std::string const & outputFilename, bool autoDeleteImageFromSrc /* = false */, CameraCaptureTypeWrapper const & captureType /* = Image */, CameraFileTypeWrapper const & fileType /* = Normal */)
		{
			auto cameraFilePath = cameraWrapper.capture(captureType);
			
			auto cameraFile = cameraWrapper.fileGet(cameraFilePath.Folder, cameraFilePath.Name, fileType);
			
			cameraFile.save(outputFilename);
			
			if(autoDeleteImageFromSrc)
			{
				cameraWrapper.fileDelete(cameraFilePath.Folder, cameraFilePath.Name);
			}
		}

		void capture(CameraWrapper& cameraWrapper, std::ostream& outputStream, bool autoDeleteImageFromSrc /* = false */, CameraCaptureTypeWrapper const & captureType /* = Image */, CameraFileTypeWrapper const & fileType /* = Normal */)
		{
			auto cameraFilePath = cameraWrapper.capture(captureType);
			
			auto cameraFile = cameraWrapper.fileGet(cameraFilePath.Folder, cameraFilePath.Name, fileType);
			
			auto temp = cameraFile.getDataAndSize();
			
			outputStream.write(temp.data(),temp.size());
			
			outputStream.flush(); // If we don't flush, I found strange things might happen to the jpg, for one thing the thumbnail wouldn't show up. Makes sense, as once we leave this scope some items are disposed, so we want to make sure that the stream is flushed.
			// Not sure if I should close the file in here as well, or let the user do it. I'll let the user do it for now.
			
			if(autoDeleteImageFromSrc)
			{
				cameraWrapper.fileDelete(cameraFilePath.Folder, cameraFilePath.Name);
			}
		}
		
		//Private Method
		void getChildrenItems(CameraWrapper& cameraWrapper, std::string const & folder, std::vector<std::string>& allItems, bool getFiles)
		{
			if(getFiles == false)
			{
				allItems.push_back(folder);
			}
			else
			{
				auto cameraListFiles = cameraWrapper.folderListFiles(folder);
				
				for(int i = 0; i < cameraListFiles.count(); ++i)
				{
					allItems.push_back(folder + cameraListFiles.getName(i));
				}
			}
			
			auto cameraListFolders = cameraWrapper.folderListFolders(folder);
			
			for(int i = 0; i < cameraListFolders.count(); ++i)
			{
				getChildrenItems(cameraWrapper, folder + cameraListFolders.getName(i) + "/", allItems, getFiles);
			}
		}
		
		std::vector<std::string> getAllFolders(CameraWrapper& cameraWrapper)
		{
			std::vector<std::string> allFolders;
			
			getChildrenItems(cameraWrapper, "/", allFolders, false);
			
			return allFolders;
		}
		
		std::vector<std::string> getAllFiles(CameraWrapper& cameraWrapper)
		{
			std::vector<std::string> allFiles;
			
			getChildrenItems(cameraWrapper, "/", allFiles, true);
			
			return allFiles;
		}
	}
}
