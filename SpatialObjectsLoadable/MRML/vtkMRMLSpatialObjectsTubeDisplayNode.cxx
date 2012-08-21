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

#include "vtkPolyDataTensorToColor.h"
#include "vtkTubeFilter.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
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
  this->TubeNumberOfSides = 6;
  this->TubeRadius = 0.5;

  this->Ambient = 0.25;
  this->Diffuse = 0.8;
  this->Specular = 0.25;
  this->Power = 20;

  // Pipeline
  this->amontAssignAttribute = vtkAssignAttribute::New();
  this->TubeFilter->SetInputConnection(
    this->amontAssignAttribute->GetOutputPort());

  this->avalAssignAttribute = vtkAssignAttribute::New();
  this->avalAssignAttribute->SetInputConnection(
    this->TubeFilter->GetOutputPort());
}

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsTubeDisplayNode::~vtkMRMLSpatialObjectsTubeDisplayNode()
{
  this->RemoveObservers(vtkCommand::ModifiedEvent, this->MRMLCallbackCommand);
  this->amontAssignAttribute->Delete();
  this->TubeFilter->Delete();
  this->avalAssignAttribute->Delete();
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
void vtkMRMLSpatialObjectsTubeDisplayNode::SetPolyData(vtkPolyData* polyData)
{
  if (this->PolyData != polyData &&
      this->TubeFilter &&
      this->amontAssignAttribute &&
      this->avalAssignAttribute
      )
    {
    Superclass::SetPolyData(polyData);

    this->amontAssignAttribute->SetInput(polyData);
    this->amontAssignAttribute->Assign("TubeRadius",
                                       vtkDataSetAttributes::SCALARS,
                                       vtkAssignAttribute::POINT_DATA);
    this->TubeFilter->SetVaryRadiusToVaryRadiusByAbsoluteScalar();
    this->TubeFilter->SetInputConnection(amontAssignAttribute->GetOutputPort());
    this->avalAssignAttribute->SetInputConnection(TubeFilter->GetOutputPort());

    this->SetTubeNumberOfSides(this->TubeFilter->GetNumberOfSides());
    this->SetTubeRadius(this->TubeFilter->GetRadius());
    }
}

//------------------------------------------------------------------------------
vtkPolyData* vtkMRMLSpatialObjectsTubeDisplayNode::GetPolyData()
{
  if (this->TubeFilter && this->TubeFilter->GetInput())
    {
    if(this->TubeFilter->GetOutput() &&
       this->TubeFilter->GetOutput()->GetPointData() &&
       this->TubeFilter->GetOutput()->GetPointData()->GetScalars())
      {
      vtkDebugMacro("Upper Bound Scalar value: " <<
                    this->TubeFilter->GetOutput()->
                      GetPointData()->GetScalars()->GetMaxNorm());
      }

    return this->OutputPolyData;
    }
  else
    {
    return NULL;
    }
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsTubeDisplayNode::UpdatePolyDataPipeline() 
{
  if (!this->PolyData || !this->Visibility)
    {
    return;
    }

  // Set display properties according to the
  // line display properties node
  vtkMRMLSpatialObjectsDisplayPropertiesNode*
    SpatialObjectsDisplayPropertiesNode =
      this->GetSpatialObjectsDisplayPropertiesNode();

  this->amontAssignAttribute->SetInput(this->PolyData);

  if (SpatialObjectsDisplayPropertiesNode != NULL)
    {
    // set line coloring
    if (this->GetColorMode() ==
          vtkMRMLSpatialObjectsDisplayNode::colorModeSolid)
      {
      this->ScalarVisibilityOff();

      vtkMRMLNode* ColorNode =
        this->GetScene()->GetNodeByID("vtkMRMLColorTableNodeFullRainbow");
      if (ColorNode)
        {
        this->SetAndObserveColorNodeID(ColorNode->GetID());
        }

      this->AutoScalarRangeOff();
      this->SetScalarRange(0, 255);
      }

    else if (this->GetColorMode() ==
               vtkMRMLSpatialObjectsDisplayNode::colorModeScalarData)
      {
      this->ScalarVisibilityOn();

      this->PolyData->GetPointData()->
        SetActiveScalars(this->GetActiveScalarName());

      this->amontAssignAttribute->Assign("TubeRadius",
                                         vtkDataSetAttributes::SCALARS,
                                         vtkAssignAttribute::POINT_DATA);

      this->TubeFilter->Update();

      this->avalAssignAttribute->Assign(this->GetActiveScalarName(),
                                        vtkDataSetAttributes::SCALARS,
                                        vtkAssignAttribute::POINT_DATA);

      this->avalAssignAttribute->Update();
      }
    }
  else
    {
    this->ScalarVisibilityOff();
    }

  // Should be aval
  this->OutputPolyData =
    vtkPolyData::SafeDownCast(this->avalAssignAttribute->GetOutput());
}
