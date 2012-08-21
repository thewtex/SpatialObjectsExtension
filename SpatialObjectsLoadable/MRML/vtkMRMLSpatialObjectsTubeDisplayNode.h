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

/// vtkMRMLSpatialObjectsTubeDisplayNode -
/// MRML node to represent display properties for spatial objects.
///
/// vtkMRMLSpatialObjectsTubeDisplayNode nodes store
/// display properties of vessels including color type, radius of the tube,
/// number of sides, display on/off for glyphs and display of
/// trajectory as a line or tube.

#ifndef __vtkMRMLSpatialObjectsTubeDisplayNode_h
#define __vtkMRMLSpatialObjectsTubeDisplayNode_h

#include "vtkMRMLSpatialObjectsDisplayNode.h"

class vtkAssignAttribute;
class vtkPolyData;
class vtkPolyDataTensorToColor;
class vtkTubeFilter;
class vtkPolyDataColorLinesByOrientation;

class VTK_SLICER_SPATIALOBJECTS_MODULE_MRML_EXPORT
vtkMRMLSpatialObjectsTubeDisplayNode : public vtkMRMLSpatialObjectsDisplayNode
{
 public:
  ///
  /// Dispaly pipeline
  vtkTubeFilter* TubeFilter;

  static vtkMRMLSpatialObjectsTubeDisplayNode* New();
  vtkTypeMacro(vtkMRMLSpatialObjectsTubeDisplayNode,
               vtkMRMLSpatialObjectsDisplayNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  //----------------------------------------------------------------------------
  /// MRMLNode methods
  //----------------------------------------------------------------------------

  virtual vtkMRMLNode* CreateNodeInstance();

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
  /// Get node XML tag name (like Volume, UnstructuredGrid)
  virtual const char* GetNodeTagName()
  {return "SpatialObjectsTubeDisplayNode";}

  ///
  /// Sets polydata (usually stored in SpatialObjectsNode)
  void SetPolyData(vtkPolyData *polyData);

  ///
  /// Gets resulting PolyData
  virtual vtkPolyData* GetPolyData();

  ///
  /// Update the pipeline based on this node attributes
  virtual void UpdatePolyDataPipeline();

  //----------------------------------------------------------------------------
  /// Display Information: Geometry to display (not mutually exclusive)
  //----------------------------------------------------------------------------

  ///
  /// Number of tube sides
  vtkSetMacro(TubeRadius, double);
  vtkGetMacro(TubeRadius, double);

  ///
  /// Number of tube sides
  vtkSetMacro(TubeNumberOfSides, int);
  vtkGetMacro(TubeNumberOfSides, int);

 protected:
  vtkMRMLSpatialObjectsTubeDisplayNode();
  ~vtkMRMLSpatialObjectsTubeDisplayNode();
  vtkMRMLSpatialObjectsTubeDisplayNode(
    const vtkMRMLSpatialObjectsTubeDisplayNode&);
  void operator=(const vtkMRMLSpatialObjectsTubeDisplayNode&);

  ///
  /// Properties
  int TubeNumberOfSides;
  double TubeRadius;

  /// Pipeline
  vtkAssignAttribute* amontAssignAttribute;
  vtkAssignAttribute* avalAssignAttribute;


};

#endif
