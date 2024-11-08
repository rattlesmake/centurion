#include "decoration.h"
#include <players/players_array.h>
#include <engine.h>
#include <environments/game/editor/editor.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <keyboard.h>


#pragma region Constructor and destructor
Decoration::Decoration(glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource)
	: GObject(std::move(_pos), _bTemporary, _objData, surface, dataSource)
{
	this->SetPosition(_pos.x, _pos.y); 
}

Decoration::~Decoration(void)
{
}
#pragma endregion

#pragma region Inherited methods:
void Decoration::ApplyGameLogics(void)
{
	const std::shared_ptr<IEnvironment> currentEnv = Engine::GetInstance().GetEnvironment();
	if (currentEnv->GetType() == IEnvironment::Environments::e_editor)
	{
		/// In editor, decorations can be selected by selection area (if there isn't any unit already selected).
		const std::shared_ptr<SelectionArea> selectionArea = std::static_pointer_cast<Editor>(currentEnv)->GetCurrentScenario()->GetSelectionAreaPtr().lock();
		if (selectionArea->IsActive() == true)
		{
			// Get current player ID.
			const auto playersArray = GObject::playersWeakRef.lock();
			const uint8_t currentPlayerID = playersArray->GetCurrentPlayerID();

			auto objSO = playersArray->GetPlayerRef(currentPlayerID).lock()->GetSelection()->Get(0);
			if (objSO && objSO->IsUnit() == true)
				return;  // At least a unit was previously selected by rectangle area, so ignore the decoration

			// Check if the decoration is taken by selection rectangle
			const bool bSelectedFromRect = selectionArea->Intersect(this->HitBox);
			if (bSelectedFromRect == true)
				this->Select(currentPlayerID);
			else if (this->IsSelected() == true)
				this->Deselect(currentPlayerID);
		}
	}
}

void Decoration::CheckPlaceability(const bool bAlwaysPlaceable)
{
	std::weak_ptr<SurfaceGrid> surfaceGrid = Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentScenario()->GetSurface()->GetSurfaceGrid();

	this->bIsPlaceable = true;
	if (bAlwaysPlaceable == true)
		return;

	if (rattlesmake::peripherals::keyboard::get_instance().IsKeyNotReleased(340) || rattlesmake::peripherals::keyboard::get_instance().IsKeyNotReleased(344))  // left&right shift 
		return;

	GObject::PlacementErrors placementError = GObject::PlacementErrors::e_no_error;

	this->bIsPlaceable = surfaceGrid.lock()->CheckObjectGridAvailability(this->grid, (*this->GetPosition()));
	if (this->bIsPlaceable == false)
		placementError = GObject::PlacementErrors::e_impassable_point;

	this->SetPlacementError(placementError);
	this->SendInfoText();
}

void Decoration::GetBinRepresentation(std::vector<uint8_t>& data, const bool calledByChild) const
{
	// Firstly, calls always GObject::GetBinSignature
	this->GObject::GetBinSignature(data);

	// In decoration we do NOT need to call method GObject::GetBinRepresentation
}

std::ostream& Decoration::Serialize(std::ostream& out, const bool calledByChild) const
{
	const std::string tabs(GObject::nTabs, '\t');

	if (calledByChild == false)
		out << tabs
			<< "<decoration class=\"" << this->GetClassName() << "\" "
			<< "id=\"" << std::to_string(this->GetUniqueID()) << "\" "
			<< "xPos=\"" << std::to_string(this->GetPositionX()) << "\" "
			<< "yPos=\"" << std::to_string(this->GetPositionY()) << "\""
			<< ">\n";
	this->GObject::Serialize(out, true);
	if (calledByChild == false)
		out << tabs << "</decoration>";

	return out;
}
#pragma endregion
