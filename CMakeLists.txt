cmake_minimum_required(VERSION 2.8.8)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  set(EXTENSION_NAME SpatialObject)
  set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/4.1/Extensions/${EXTENSION_NAME}")
  set(EXTENSION_CATEGORY "Surface Models")
  set(EXTENSION_CONTRIBUTORS "Michael Jeulin-L (Kitware), Matthew McCormick (Kitware)")
  set(EXTENSION_DESCRIPTION "The SpatialObjects extension will allow the user to load and adjust display parameters of spatial object data.")
  set(EXTENSION_ICONURL "http://viewvc.slicer.org/viewvc.cgi/Slicer4/trunk/Extensions/Testing/LoadableExtensionTemplate/Resources/Icons/LoadableExtensionTemplate.png?revision=19437&view=co")
endif()

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

#-----------------------------------------------------------------------------
add_subdirectory(SpatialObjectsLoadable)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  include(${Slicer_EXTENSION_CPACK})
endif()
