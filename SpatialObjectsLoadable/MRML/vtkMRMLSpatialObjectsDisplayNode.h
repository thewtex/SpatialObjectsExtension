/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Michael Jeulin-L, Kitware Inc.

==============================================================================*/

/// vtkMRMLSpatialObjectsDisplayNode -
/// MRML node to represent display properties for vessels.
///
/// vtkMRMLSpatialObjectsTubeDisplayNode nodes store
/// display properties of vessels including color type, radius of the tube,
/// number of sides, display on/off for glyphs and display of
/// trajectory as a line or tube.

#ifndef __vtkMRMLSpatialObjectsDisplayNode_h
#define __vtkMRMLSpatialObjectsDisplayNode_h

// MRML includes
#include <vtkMRMLModelDisplayNode.h>

// Tractography includes
#include "vtkSlicerSpatialObjectsModuleMRMLExport.h"

class vtkMRMLSpatialObjectsDisplayPropertiesNode;

class VTK_SLICER_SPATIALOBJECTS_MODULE_MRML_EXPORT
vtkMRMLSpatialObjectsDisplayNode : public vtkMRMLModelDisplayNode
{
public:
  vtkTypeMacro(vtkMRMLSpatialObjectsDisplayNode, vtkMRMLModelDisplayNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  //----------------------------------------------------------------------------
  /// MRMLNode methods
  //----------------------------------------------------------------------------

  ///
  /// Read node attributes from XML (MRML) file
  virtual void ReadXMLAttributes(const char** atts);

  ///
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  ///
  /// Copy the node's attributes to this object
  /// Does NOT copy: ID, FilePrefix, Name, ID
  virtual void Copy(vtkMRMLNode *node);

  ///
  /// Get node XML tag name (like Volume, model...)
  virtual const char* GetNodeTagName() = 0;

  ///
  /// Updates this node if it depends on other nodes
  /// when the node is deleted in the scene
  virtual void UpdateReferences();

  ///
  /// Finds the storage node and read the data
  virtual void UpdateScene(vtkMRMLScene *scene);

  ///
  /// Update the stored reference to another node in the scene
  virtual void UpdateReferenceID(const char *oldID, const char *newID);

  //----------------------------------------------------------------------------
  /// Display Information: Geometry to display (not mutually exclusive)
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /// Display Information: Color Mode
  /// 0) solid color by group
  /// 1) color by scalar invariant
  /// 2) color by avg scalar invariant
  /// 3) color by other
  //----------------------------------------------------------------------------
  enum
  {
    colorModeSolid = 0,
    colorModeScalar = 1,
    colorModeFunctionOfScalar = 2,
    colorModeUseCellScalars = 3,
    colorModeScalarData = 4
  };

  //----------------------------------------------------------------------------
  /// Display Information: ColorMode for ALL nodes
  //----------------------------------------------------------------------------

  /// Description:
  /// Color mode for glyphs. The color modes are mutually exclusive.
  vtkGetMacro(ColorMode, int);
  vtkSetMacro(ColorMode, int);

  ///
  /// Color by solid color (for example the whole vessel bundle red. blue, etc.)
  void SetColorModeToSolid()
  {this->SetColorMode(this->colorModeSolid );}

  ///
  /// Color according to the vessels using various scalar invariants.
  void SetColorModeToScalar()
  {this->SetColorMode(this->colorModeScalar );}

  ///
  /// Color function of scalar invariants along the tract.
  void SetColorModeToFunctionOfScalar()
  {this->SetColorMode(this->colorModeFunctionOfScalar);}

  ///
  /// Use to color by the active cell scalars. This is intended to support
  /// external processing of spatial objects,
  /// for example to get the orientation of each tube with the distance
  /// of that vessel variation from the registration.
  /// Then by making that information the active cell scalar field,
  /// this will allow coloring by that information.
  void SetColorModeToUseCellScalars()
  {this->SetColorMode(this->colorModeUseCellScalars );}

  ///
  /// Color according to the vessels using scalars
  /// from the original SpatialObjectsNode.
  void SetColorModeToScalarData()
  {this->SetColorMode(this->colorModeScalarData );}

  //----------------------------------------------------------------------------
  /// MRML nodes that are observed
  //----------------------------------------------------------------------------

  ///
  /// Get spatial object display properties MRML node object for vessels glyph.
  vtkMRMLSpatialObjectsDisplayPropertiesNode*
    GetSpatialObjectsDisplayPropertiesNode();

  ///
  /// Set spatial object display properties MRML node object for vessels glyph.
  void SetAndObserveSpatialObjectsDisplayPropertiesNodeID(const char *id);

  ///
  /// Get ID of spatial object display properties
  /// MRML node object for vessels glyph.
  vtkGetStringMacro(SpatialObjectsDisplayPropertiesNodeID);

  static int GetNumberOfScalarInvariants();
  static int GetNthScalarInvariant(int i);

 protected:
  vtkMRMLSpatialObjectsDisplayNode();
  ~vtkMRMLSpatialObjectsDisplayNode();
  vtkMRMLSpatialObjectsDisplayNode(const vtkMRMLSpatialObjectsDisplayNode&);
  void operator=(const vtkMRMLSpatialObjectsDisplayNode&);

  virtual void SetSpatialObjectsDisplayPropertiesNodeID(const char* id);

  vtkMRMLSpatialObjectsDisplayPropertiesNode*
    SpatialObjectsDisplayPropertiesNode;
  char* SpatialObjectsDisplayPropertiesNodeID;

  static std::vector<int> GetSupportedColorModes();
  int ColorMode;
};

#endif
