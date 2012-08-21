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

#include "vtkObjectFactory.h"

// VTK includes
#include "vtkCallbackCommand.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLSpatialObjectsLineDisplayNode.h"
#include "vtkMRMLSpatialObjectsDisplayPropertiesNode.h"

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLSpatialObjectsLineDisplayNode);


//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsLineDisplayNode::vtkMRMLSpatialObjectsLineDisplayNode()
{
  this->ColorMode = vtkMRMLSpatialObjectsDisplayNode::colorModeSolid;
}

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsLineDisplayNode::~vtkMRMLSpatialObjectsLineDisplayNode()
{
  this->RemoveObservers (vtkCommand::ModifiedEvent, this->MRMLCallbackCommand);

}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsLineDisplayNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsLineDisplayNode::ReadXMLAttributes(const char** atts)
{
  Superclass::ReadXMLAttributes(atts);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsLineDisplayNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsLineDisplayNode::PrintSelf(ostream& os,
                                                     vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsLineDisplayNode::
SetPolyData(vtkPolyData *linePolyData)
{
  if (this->PolyData != linePolyData)
    {
    Superclass::SetPolyData(linePolyData);
    }
}

//------------------------------------------------------------------------------
vtkPolyData* vtkMRMLSpatialObjectsLineDisplayNode::GetPolyData()
{
  return this->OutputPolyData;
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsLineDisplayNode::UpdatePolyDataPipeline() 
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

  vtkPolyData* IntermediatePolyData = this->PolyData;

  if (SpatialObjectsDisplayPropertiesNode != NULL)
    {
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
      /*vtkDebugMacro("LINE SCALAR NAME: "
                    << this->SecondActiveScalarName
                    << std::endl);*/

      this->ScalarVisibilityOn();

      IntermediatePolyData = this->PolyData;
      IntermediatePolyData->GetPointData()->
        SetActiveScalars(this->GetActiveScalarName());

      IntermediatePolyData->Update();
      }
    }
  else
    {
    this->ScalarVisibilityOff();
    }

  if(this->ActiveScalarName != NULL)
    std::cout << "ScalarMode: " << this->ActiveScalarName << std::endl;
  std::cout << "RangeUp: " << IntermediatePolyData->GetScalarRange()[0]
            << std::endl;

  this->OutputPolyData = IntermediatePolyData;
}
