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

#include <sstream>

#include "vtkAssignAttribute.h"
#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkCellData.h"
#include "vtkPointData.h"

#include "vtkAssignAttribute.h"
#include "vtkPolyDataTensorToColor.h"
#include "vtkTubeFilter.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLSpatialObjectsDisplayPropertiesNode.h"
#include "vtkMRMLSpatialObjectsTubeDisplayNode.h"
#include "vtkPolyDataColorLinesByOrientation.h"

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLSpatialObjectsTubeDisplayNode);


//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsTubeDisplayNode::vtkMRMLSpatialObjectsTubeDisplayNode()
{
  this->ColorMode = vtkMRMLSpatialObjectsDisplayNode::colorModeSolid;

  this->TubeFilter = vtkTubeFilter::New();
  this->TubeFilter->SetVaryRadiusToVaryRadiusByAbsoluteScalar();
  this->TubeNumberOfSides = 6;
  this->TubeRadius = 0.5;

  this->Ambient = 0.25;
  this->Diffuse = 0.8;
  this->Specular = 0.25;
  this->Power = 20;

  // Pipeline
  this->amontAssignAttribute = vtkAssignAttribute::New();
  this->amontAssignAttribute->Assign("TubeRadius",
                                     vtkDataSetAttributes::SCALARS,
                                     vtkAssignAttribute::POINT_DATA);
  this->TubeFilter->SetInputConnection(
    this->amontAssignAttribute->GetOutputPort());

  this->AssignAttribute->SetInputConnection(
    this->TubeFilter->GetOutputPort());
}

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsTubeDisplayNode::~vtkMRMLSpatialObjectsTubeDisplayNode()
{
  this->RemoveObservers(vtkCommand::ModifiedEvent, this->MRMLCallbackCommand);
  this->amontAssignAttribute->Delete();
  this->TubeFilter->Delete();
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsTubeDisplayNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);
  of << indent << " tubeRadius =\"" << this->TubeRadius << "\"";
  of << indent << " tubeNumberOfSides =\"" << this->TubeNumberOfSides << "\"";
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsTubeDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "tubeRadius"))
      {
      std::stringstream ss;
      ss << attValue;
      ss >> this->TubeRadius;
      }

    if (!strcmp(attName, "tubeNumberOfSides"))
      {
      std::stringstream ss;
      ss << attValue;
      ss >> this->TubeNumberOfSides;
      }
    }

  this->EndModify(disabledModify);
}

//------------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, ID
void vtkMRMLSpatialObjectsTubeDisplayNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLSpatialObjectsTubeDisplayNode *node =
    vtkMRMLSpatialObjectsTubeDisplayNode::SafeDownCast(anode);

  this->SetTubeNumberOfSides(node->TubeNumberOfSides);
  this->SetTubeRadius(node->TubeRadius);

  this->EndModify(disabledModify);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsTubeDisplayNode::
PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << indent << "TubeNumberOfSides: " << this->TubeNumberOfSides << "\n";
  os << indent << "TubeRadius: " << this->TubeRadius << "\n";
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsTubeDisplayNode::SetInputToPolyDataPipeline(vtkPolyData* polyData)
{
  this->amontAssignAttribute->SetInput(polyData);
}

//------------------------------------------------------------------------------
vtkPolyData* vtkMRMLSpatialObjectsTubeDisplayNode::GetInputPolyData()
{
  return vtkPolyData::SafeDownCast(this->amontAssignAttribute->GetInput());
}

//------------------------------------------------------------------------------
vtkAlgorithmOutput* vtkMRMLSpatialObjectsTubeDisplayNode::GetOutputPort()
{
  return this->AssignAttribute->GetOutputPort();
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsTubeDisplayNode::UpdatePolyDataPipeline()
{
  if (!this->GetInputPolyData() || !this->Visibility)
    {
    return;
    }

  this->Superclass::UpdatePolyDataPipeline();

  // Set display properties according to the
  // line display properties node
  vtkMRMLSpatialObjectsDisplayPropertiesNode*
    SpatialObjectsDisplayPropertiesNode =
      this->GetSpatialObjectsDisplayPropertiesNode();

  const char * activeScalarName = this->GetActiveScalarName();
  this->AssignAttribute->Assign(activeScalarName,
                                vtkDataSetAttributes::SCALARS,
                                vtkAssignAttribute::POINT_DATA);

  if (SpatialObjectsDisplayPropertiesNode != NULL)
    {
    const int colorMode = this->GetColorMode();
    if (colorMode ==
          vtkMRMLSpatialObjectsDisplayNode::colorModeSolid)
      {
      this->ScalarVisibilityOff();

      vtkMRMLNode* colorNode =
        this->GetScene()->GetNodeByID("vtkMRMLColorTableNodeFullRainbow");
      if (colorNode)
        {
        this->SetAndObserveColorNodeID(colorNode->GetID());
        }

      this->AutoScalarRangeOff();
      this->SetScalarRange(0, 255);
      }

    else if (colorMode ==
               vtkMRMLSpatialObjectsDisplayNode::colorModeScalarData)
      {
      this->ScalarVisibilityOn();
      this->TubeFilter->SetRadius(this->GetTubeRadius());
      this->TubeFilter->SetNumberOfSides(this->GetTubeNumberOfSides());
      this->AssignAttribute->Update();
      }
    }
  else
    {
    this->ScalarVisibilityOff();
    }
}
