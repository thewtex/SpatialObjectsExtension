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
#include "vtkCallbackCommand.h"

// VTK includes
#include <vtkConeSource.h>
#include <vtkGlyph3DMapper.h>
#include <vtkSource.h>

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLSpatialObjectsGlyphDisplayNode.h"
#include "vtkMRMLDiffusionTensorDisplayPropertiesNode.h"

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLSpatialObjectsGlyphDisplayNode);

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsGlyphDisplayNode::vtkMRMLSpatialObjectsGlyphDisplayNode()
{
  this->Glyph3DMapper = vtkGlyph3DMapper::New();
  this->ColorMode = vtkMRMLSpatialObjectsDisplayNode::colorModeScalar;
}

//------------------------------------------------------------------------------
vtkMRMLSpatialObjectsGlyphDisplayNode::~vtkMRMLSpatialObjectsGlyphDisplayNode()
{
  this->RemoveObservers(vtkCommand::ModifiedEvent, this->MRMLCallbackCommand);
  this->Glyph3DMapper->Delete();
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsGlyphDisplayNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsGlyphDisplayNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();
  Superclass::ReadXMLAttributes(atts);
  this->EndModify(disabledModify);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsGlyphDisplayNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();
  Superclass::Copy(anode);
  this->EndModify(disabledModify);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsGlyphDisplayNode::PrintSelf(ostream& os,
                                                      vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsGlyphDisplayNode::
SetPolyData(vtkPolyData* polyData)
{
  if ((this->Glyph3DMapper) && polyData != this->PolyData)
    {
    Superclass::SetPolyData(polyData);

    /*vtkConeSource *squad = vtkConeSource::New();
    squad->SetHeight(1.0);
    squad->SetRadius(1.0);
    squad->SetResolution(50);
    squad->SetDirection(0.0,0.0,1.0);

    this->Glyph3DMapper->SetInputConnection(calc->GetOutputPort());
    this->Glyph3DMapper->SetOrientationArray("orientation");
    this->Glyph3DMapper->SetOrientationModeToRotation();
    this->Glyph3DMapper->SetScaleFactor(0.01);
    this->Glyph3DMapper->SetSourceConnection(squad->GetOutputPort());*/

    /*this->Glyph3DMapper->
      SetInputConnection(polyData->GetSource()->GetOutputPort());*/
    }
}

//------------------------------------------------------------------------------
vtkPolyData* vtkMRMLSpatialObjectsGlyphDisplayNode::GetPolyData()
{
  if (!this->Glyph3DMapper)
    {
    return 0;
    }

  return this->PolyData;//Glyph3DMapper->GetSource();
}

//------------------------------------------------------------------------------
void vtkMRMLSpatialObjectsGlyphDisplayNode::UpdatePolyDataPipeline() 
{
  if (!this->PolyData || !this->Visibility)
    {
    return;
    }

  /*if (this->Glyph3DMapper)
    {
    this->Glyph3DMapper->SetInputConnection(
      this->PolyData->GetSource()->GetOutputPort());
    }*/

  // Set display properties according to the
  // tensor-specific display properties node for glyphs
  // TODO Create own prooperties nodes

}
