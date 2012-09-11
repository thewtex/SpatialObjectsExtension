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

// MRML includes
#include "vtkMRMLDisplayableNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLSpatialObjectsDisplayNode.h"
#include "vtkMRMLSpatialObjectsNode.h"
#include "vtkMRMLSpatialObjectsDisplayPropertiesNode.h"

// VTK includes
#include <vtkCommand.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
vtkCxxSetReferenceStringMacro(vtkMRMLSpatialObjectsDisplayNode,
                              SpatialObjectsDisplayPropertiesNodeID);

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsDisplayNode::vtkMRMLSpatialObjectsDisplayNode()
{
  this->BackfaceCulling = 0;

  // Enumerated
  this->ColorMode = this->colorModeSolid;
  this->SetColor(0.9, 0.2, 0.1);

  this->SpatialObjectsDisplayPropertiesNode = NULL;
  this->SpatialObjectsDisplayPropertiesNodeID = NULL;

  this->ScalarRange[0] = 0.;
  this->ScalarRange[1] = 1.;
}

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsDisplayNode::~vtkMRMLSpatialObjectsDisplayNode()
{
  this->SetAndObserveSpatialObjectsDisplayPropertiesNodeID(NULL);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);
  of << indent << " colorMode =\"" << this->ColorMode << "\"";

  if (this->SpatialObjectsDisplayPropertiesNodeID != NULL)
    {
    of << indent << " SpatialObjectsDisplayPropertiesNodeRef=\""
       << this->SpatialObjectsDisplayPropertiesNodeID << "\"";
    }
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "colorMode")) 
      {
      std::stringstream ss;
      ss << attValue;
      int colorMode;
      ss >> colorMode;
      this->SetColorMode(colorMode);
      }

    else if (!strcmp(attName, "SpatialObjectsDisplayPropertiesNodeRef"))
      {
      this->SetSpatialObjectsDisplayPropertiesNodeID(attValue);
      }
    }

  this->EndModify(disabledModify);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

 vtkMRMLSpatialObjectsDisplayNode *node =
   vtkMRMLSpatialObjectsDisplayNode::SafeDownCast(anode);
 this->SetColorMode(node->ColorMode);

  Superclass::Copy(anode);

  this->SetSpatialObjectsDisplayPropertiesNodeID(
    node->SpatialObjectsDisplayPropertiesNodeID);

  this->EndModify(disabledModify);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
  os << indent << "ColorMode: " << this->ColorMode << "\n";
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayNode::UpdateScene(vtkMRMLScene *scene)
{
   Superclass::UpdateScene(scene);

   this->SetAndObserveSpatialObjectsDisplayPropertiesNodeID(
     this->GetSpatialObjectsDisplayPropertiesNodeID());
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayNode::UpdateReferences()
{
  Superclass::UpdateReferences();

  if (this->SpatialObjectsDisplayPropertiesNodeID != NULL &&
      this->Scene->GetNodeByID(
        this->SpatialObjectsDisplayPropertiesNodeID) == NULL)
    {
    this->SetAndObserveSpatialObjectsDisplayPropertiesNodeID(NULL);
    }
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayNode::UpdateReferenceID(const char *oldID,
                                                         const char *newID)
{
  if (this->SpatialObjectsDisplayPropertiesNodeID &&
      !strcmp(oldID, this->SpatialObjectsDisplayPropertiesNodeID))
    {
    this->SetSpatialObjectsDisplayPropertiesNodeID(newID);
    }
}

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsDisplayPropertiesNode* vtkMRMLSpatialObjectsDisplayNode::
GetSpatialObjectsDisplayPropertiesNode()
{
  vtkMRMLSpatialObjectsDisplayPropertiesNode* node = NULL;

  // Find the node corresponding to the ID we have saved.
  if  (this->GetScene() && this->GetSpatialObjectsDisplayPropertiesNodeID())
    {
    vtkMRMLNode* cnode = this->
      GetScene()->GetNodeByID(this->SpatialObjectsDisplayPropertiesNodeID);
    node = vtkMRMLSpatialObjectsDisplayPropertiesNode::SafeDownCast(cnode);
    }

  return node;
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsDisplayNode::
SetAndObserveSpatialObjectsDisplayPropertiesNodeID(const char *id )
{
  if (
      (id != this->GetSpatialObjectsDisplayPropertiesNodeID())
      && id != NULL && this->GetSpatialObjectsDisplayPropertiesNodeID() != NULL
      && (strcmp(id, this->GetSpatialObjectsDisplayPropertiesNodeID()) == 0)
      )
    {
    return;
    }

  // Stop observing any old node
  vtkSetAndObserveMRMLObjectMacro(this->SpatialObjectsDisplayPropertiesNode,
                                  NULL);

  // Set the ID. This is the "ground truth" reference to the node.
  this->SetSpatialObjectsDisplayPropertiesNodeID(id);

  // Get the node corresponding to the ID.
  // This pointer is only to observe the object.
  vtkMRMLNode *cnode = this->GetSpatialObjectsDisplayPropertiesNode();

  // Observe the node using the pointer.
  vtkSetAndObserveMRMLObjectMacro(this->SpatialObjectsDisplayPropertiesNode ,
                                  cnode);

  //The new SpatialObjectsDisplayPropertiesNode can have a different setting
  // on the properties so we emit the event that the polydata has been modified.
  if (cnode)
    {
    this->InvokeEvent(vtkMRMLModelNode::PolyDataModifiedEvent, this);
    }
}

//------------------------------------------------------------------------------
std::vector<int> vtkMRMLSpatialObjectsDisplayNode::GetSupportedColorModes()
{
  std::vector<int> modes;

  modes.clear();
  modes.push_back(vtkMRMLSpatialObjectsDisplayPropertiesNode::ColorOrientation);
  modes.push_back(vtkMRMLSpatialObjectsDisplayPropertiesNode::LinearMeasure);
  modes.push_back(
    vtkMRMLSpatialObjectsDisplayPropertiesNode::RelativeAnisotropy);

  return modes;
}

//------------------------------------------------------------------------------
int vtkMRMLSpatialObjectsDisplayNode::GetNumberOfScalarInvariants()
{
  static std::vector<int> modes =
    vtkMRMLSpatialObjectsDisplayNode::GetSupportedColorModes();

  return modes.size();
}

//------------------------------------------------------------------------------
int vtkMRMLSpatialObjectsDisplayNode::GetNthScalarInvariant(int i)
{
  static std::vector<int> modes =
    vtkMRMLSpatialObjectsDisplayNode::GetSupportedColorModes();

  return modes[i];
}
