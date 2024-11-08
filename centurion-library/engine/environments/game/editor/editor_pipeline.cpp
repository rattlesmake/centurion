#include "editor_pipeline.h"
#include "editor.h"

#include <keyboard.h>
#include <mouse.h>

#include <environments/game/igame.h>
#include <environments/game/adventure/scenario/scenario.h>
#include <environments/game/classes/objectsSet/objects_list.h>
#include <environments/game/adventure/scenario/minimap.h>

#include <environments/game/classes/gobject.h>
#include <environments/game/classes/playable.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/objectsSet/settlement.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainBrush.h>
#include <xml_assets.h>

#include <GLFW/glfw3.h>
#include <dialogWindows.h>

namespace centurion
{
	namespace game
	{
		bool editor_pipeline::b_finalize_shifting = false;

		editor_pipeline::editor_pipeline(IGame* creator) :
			game_pipeline(creator),
			ref_editor(*creator->AsEditor())
		{
			b_finalize_shifting = false; // reset static boolean
		}
		void editor_pipeline::execute_game_logics(void)
		{
			if (b_gui_any_iframe_opened || b_gui_any_dialog_window_active || sp_selection_area->IsActive())
				return;

			switch (ref_editor.GetState())
			{

			case editor_state::idle:

				// if idle, the user was doing nothing
				// until the previous frame
				// -> check if it's starting to do something
				check_user_input_actions();
				return;

			case editor_state::inserting_object:

				// it activates by UI
				perform_object_insertion();
				return;

			case editor_state::shifting_object:

				// it activates by user input
				perform_objects_shifting();
				return;

			case editor_state::changing_terrain:

				// it activates by UI
				perform_terrain_change();
				break;

			case editor_state::drawing_areas:

				// it activates by UI
				// todo
				break;

			case editor_state::using_area_gizmo:

				// it activates by UI
				// todo
				break;

			default:
				return;

			}
		}

		void editor_pipeline::perform_object_insertion()
		{
			//TODO questi parametri dovranno essere sceglibili
			const uint8_t h_distance = 50;
			const uint8_t v_distance = 100;

			auto rp_obj_to_insert_opt = &ref_editor.insertingObject.objectToInsert;
			auto rp_obj_offsets = &ref_editor.insertingObject.objectOffsets;
			auto rp_start_pos = &ref_editor.insertingObject.startPos;

			//Abort object insertion
			if (ref_mouse.RightClick || ref_keyboard.IsKeyPressed(GLFW_KEY_ESCAPE))
			{
				ref_mouse.RightClick = false;
				ref_keyboard.SetKeyStatus(GLFW_KEY_ESCAPE, 0);

				// set state to idle and reset variables
				ref_editor.SetState_Idle();
				return;
			}

			// if we are here, the optional should always have value
			// because we call this function under b_inserting_object
			// that is exactly equal to rp_temp_object_optional->has_value()
			assert(rp_obj_to_insert_opt->has_value());

			auto sp_object_to_insert = rp_obj_to_insert_opt->value();

			// if i'm here it means that i selected the initial position and i'm waiting to release 
			// the mouse to insert one or more objects.
			if (ref_mouse.LeftClick) // the Tracing has been performed so i can use Y2D
			{
				(*rp_start_pos) = glm::ivec2(ref_mouse.GetXMapCoordinate(), ref_mouse.GetY2DMapCoordinate()); // here the Y 2D coord
				rp_obj_offsets->push_back({ (*rp_start_pos), true });
			}

			// if i'm here it means that i want to perform a multiple insertion of UNITS
			if (ref_mouse.LeftHold && sp_object_to_insert->IsUnit() == true)
			{
				sp_object_to_insert->SetAsTemporary();
				rp_obj_offsets->clear();
				int delta_x = int(ref_mouse.GetXMapCoordinate() - rp_start_pos->x);
				int delta_y = int(ref_mouse.GetY2DMapCoordinate() - rp_start_pos->y); // here the Y 2D coord
				int n_objects_x = std::abs(delta_x) / h_distance + 1;
				int n_objects_y = std::abs(delta_y) / v_distance + 1;

				for (int ix = 0; ix < n_objects_x; ix++)
				{
					float pos_x = rp_start_pos->x;
					(delta_x > 0) ? pos_x += h_distance * ix : pos_x -= h_distance * ix;
					for (int iy = 0; iy < n_objects_y; iy++)
					{
						float pos_y = rp_start_pos->y;
						(delta_y > 0) ? pos_y += v_distance * iy : pos_y -= v_distance * iy;
						rp_obj_offsets->push_back({ glm::vec2(pos_x, pos_y), true });
					}
				}
			}

			// if i'm here it means that i want to insert at least one object 
			if (ref_mouse.Release)
			{
				// don't overwrite tempObject! 
				// it's important to leave the temporary object to allow
				// other insertion after the current object creation

				// note that CreateGObject resets rp_obj_offsets 
				// todo: maybe is clearer removing .pop_front inside CreateGObject 
				// and adding .reset below? 
				ObjsCollection::CreateGObject(
					igame.GetXmlAssets()->get_xml_classes(),
					ref_editor.insertingObject.className,
					*rp_obj_offsets,
					ref_editor.insertingObject.playerId,
					false,
					sp_scenario->GetSurface()
				);
				sp_minimap->Update();

				ref_mouse.LeftClick = false;
				sp_adventure->MarkAsEdited();

				//AreaArray::EnableUpdatingObjectsInside();
				return;
			}

			// if i'm here it means that i'm not placing or aborting and i'm choosing the object position
			if (rp_obj_offsets->empty() == true)
			{
				glm::vec2 pos{ ref_mouse.GetXMapCoordinate(), ref_mouse.GetYMapCoordinate() }; // here NOT the Y 2D coord
				rp_obj_offsets->push_back({ std::move(pos), true });
				ObjsCollection::RenderTemporaryObject(sp_object_to_insert, false, *rp_obj_offsets);
				rp_obj_offsets->clear();
			}

			// if i'm here it means that i'm placing at least one object (i've chosen the initial position)
			else
			{
				rp_obj_offsets->size() > 1 ? sp_object_to_insert->SetAsTemporaryMultiple() : sp_object_to_insert->SetAsTemporary();
				ObjsCollection::RenderTemporaryObject(sp_object_to_insert, true, *rp_obj_offsets);
			}
		}

		void editor_pipeline::perform_objects_deletion(const std::shared_ptr<ObjsList> selection)
		{
			bool canBeDeleted = true;
			while (selection->Count() >= 1 && canBeDeleted == true)
			{
				//Removal cannot always take place (for instance when it would break a settlement chain)
				canBeDeleted = perform_object_deletion(selection->Get(0));
			}

			//Reset mouse.
			ref_mouse.ResetAllButtons();
		}

		bool editor_pipeline::perform_object_deletion(const std::shared_ptr<GObject>& object_to_delete)
		{
			if (object_to_delete->IsBuilding() == true)
			{
				auto selbSP = std::static_pointer_cast<Building>(object_to_delete);
				if (selbSP->IsFirstBuilding() == true && selbSP->GetSettlement()->GetNumberOfBuildings() > 1)
				{
					std::function<void()> rejectDeletionFunc;
					std::function<void()> confirmDeletionFunc = [selbSP]() {
						auto buildingsSet = selbSP->GetSettlement()->GetBuildings();
						ObjsList::objs_iterator it = buildingsSet->begin();
						//Delete each building of the settlement.
						while (it != buildingsSet->cend())
						{
							assert(it->expired() == false);
							ObjsCollection::DestroyGObject(it->lock());
							//The next iterator is always the begin because DestroyGObject remove also a building from a settlement. 
							//And buildingsSet is a reference to these buildings :)
							it = buildingsSet->begin();
						}
					};
					//Show a question window asking for 
					gui::NewQuestionWindow("e_text_delete_settlement", confirmDeletionFunc, rejectDeletionFunc, IEnvironment::Environments::e_editor);
					return false; //Avoid freez when returning to Editor::PerformObjectsDeletion.
				}
				else if (selbSP->CheckIfSettlementChainIsNotBroken(true) == true)
				{
					ObjsCollection::DestroyGObject(object_to_delete);
				}
				else
				{
					gui::NewInfoWindow("e_text_unable_to_delete_building", IEnvironment::Environments::e_editor);
					return false; //Avoid freez when returning to Editor::PerformObjectsDeletion.
				}
			}
			else
			{
				ObjsCollection::DestroyGObject(object_to_delete);
			}

			sp_adventure->MarkAsEdited();
			sp_minimap->Update();
			return true;
		}

		void editor_pipeline::perform_objects_shifting()
		{
			const uint32_t valid_positions = set_shifting_positions(false);

			if (ref_mouse.LeftHold == false)
			{
				//I'm releasing the left button of the mouse, so I want to place the object: try to confirm a new position
				stop_object_shifting(valid_positions);
			}

			if (b_finalize_shifting)
			{
				finalize_object_shifting();
			}
		}

		void editor_pipeline::perform_terrain_change()
		{
			// This part of code is executed when you are FINISHING the terrain change
			if (ref_mouse.RightClick == true || ref_keyboard.IsKeyPressed(GLFW_KEY_ESCAPE))
			{
				ref_mouse.RightClick = false;
				ref_keyboard.SetKeyStatus(GLFW_KEY_ESCAPE, 0);
				sp_surface->GetSurfaceGrid().lock()->UpdateGrid();
				ref_editor.SetState_Idle();
				return;
			}

			if (ref_mouse.LeftClick == false && ref_mouse.LeftHold == false)
				return;

			// if it's LeftHold, apply the texture 1 frame each 20
			if (ref_mouse.LeftClick == false && ref_mouse.LeftHold && Engine::GetGameTime().GetCurrentFrame() % 5 != 0)
				return;

			// This part of code is executed when you are DOING the terrain change
			ref_editor.GetTerrainBrushPtr()->Apply();

			sp_adventure->MarkAsEdited();
			sp_minimap->Update();
		}

		void editor_pipeline::initialize_object_shifting(const std::shared_ptr<ObjsList> selection)
		{
			auto focused_obj = ref_editor.movingObject.focusedObject.lock();
			if (focused_obj == nullptr)
				return; //This can happen, for instance, if I'm drawing a selection area.

			//Save information about the focused GObject's beginning position
			ref_editor.movingObject.startObjectXPos = focused_obj->GetPosition()->x;
			ref_editor.movingObject.startObjectYPos = focused_obj->GetPosition()->y;
			ref_editor.movingObject.StartXMouse = static_cast<uint32_t>(ref_mouse.GetXMapCoordinate());
			ref_editor.movingObject.StartYMouse = static_cast<uint32_t>(ref_mouse.GetY2DMapCoordinate());

			//Clear initial grid for each selected GObject
			auto lastIt = selection->cend();
			for (auto firstIt = selection->cbegin(); firstIt != lastIt; firstIt++)
				(*firstIt).lock()->ClearGrid(true, sp_surface);

			//Start shifting.
			ref_editor.movingObject.objectsToShift = selection;
			ref_editor.SetState_ShiftingObject();
		}

		void editor_pipeline::stop_object_shifting(const uint32_t valid_positions)
		{
			auto objects_to_shift = ref_editor.movingObject.objectsToShift.lock();
			auto focused_object = ref_editor.movingObject.focusedObject.lock();

			if (valid_positions == objects_to_shift->Count()) //Shifting is valid
			{
				//Update grid of each selected GObject
				auto lastIt = objects_to_shift->cend();

				for (auto firstIt = objects_to_shift->cbegin(); firstIt != lastIt; firstIt++)
					(*firstIt).lock()->UpdateGrid(true, sp_surface);

				focused_object->UpdateGrid(true, sp_surface);

				//Update minimap
				sp_minimap->Update();

				//Update also the settlement hitbox
				if (focused_object->IsBuilding() == true)
				{
					auto settlement = static_cast<Building&>(*focused_object).GetSettlement();
					if (settlement != nullptr)
					{
						settlement->UpdateHitbox();
					}
				}

				//Mark adventure as edited
				sp_adventure->MarkAsEdited();

				b_finalize_shifting = true;
			}
			else //At least a selected GObject cannot be shifted
			{
				std::function<void()> reject_shifting_fun = []()
				{
					auto editor = Engine::GetInstance().GetEnvironment()->AsEditor();
					auto objects_to_shift = editor->movingObject.objectsToShift.lock();
					auto focused_object = editor->movingObject.focusedObject.lock();
					auto surface = Engine::GetInstance().GetSurface().lock();

					///For EACH selected GObject, restore old positions

					const uint32_t validPos = set_shifting_positions(false);
					//ASSERTION: after restoring the old positions, they must be valid. 
					//N.B.: Check if GObject is placed in a valid position! This assertion can fail for example because GObject was in a invalid position (remember checkPlaceability algorithm, grid are constantly changed)
					assert(validPos == objects_to_shift->Count());

					//Update grid of each selected GObject
					auto lastIt = objects_to_shift->cend();
					for (auto firstIt = objects_to_shift->cbegin(); firstIt != lastIt; firstIt++)
						(*firstIt).lock()->UpdateGrid(true, surface);
					focused_object->UpdateGrid(true, surface);

					b_finalize_shifting = true;
				};
				std::function<void()> confirm_shifting_func = [valid_positions, reject_shifting_fun]()
				{
					auto editor = Engine::GetInstance().GetEnvironment()->AsEditor();
					auto objects_to_shift = editor->movingObject.objectsToShift.lock();
					auto focused_object = editor->movingObject.focusedObject.lock();
					auto surface = Engine::GetInstance().GetSurface().lock();

					///Restore old positions ONLY for UNPLACEABLE selected GObjects

					const uint32_t valid_pos = set_shifting_positions(true);
					if (valid_pos == (objects_to_shift->Count() - valid_positions)) //it was possible to reposition all the objects in their initial position, so finalize creation
					{
						//Update grid of each selected GObject
						auto lastIt = objects_to_shift->cend();
						for (auto firstIt = objects_to_shift->cbegin(); firstIt != lastIt; firstIt++)
							(*firstIt).lock()->UpdateGrid(true, surface);
						focused_object->UpdateGrid(true, surface);

						b_finalize_shifting = true;
					}
					else //At least one GObject cannot be recover its position. So reject shifting of each GObject.
						reject_shifting_fun(); //TODO da testare bene!!!! Non sono sicuro che il reject in questo caso avvenga corettamente perche' non sono ancora riuscito a scatenarlo.
				};

				//If no object can be placed (i.e. validPositions == 0), there is no need to show the dialog window. So automatically reject
				//Even if I'm sfhifting a settlement (or a building), if at least one building isn't placeable, shifting will be automatically rejected.
				if (valid_positions >= 1 && focused_object->IsBuilding() == false) //Show a question window asking for confirm shifting only of placeable selected GObjects.
					gui::NewQuestionWindow("e_text_restoring_shifted_objects", confirm_shifting_func, reject_shifting_fun, IEnvironment::Environments::e_editor);
				else
					reject_shifting_fun();
			}
		}

		void editor_pipeline::finalize_object_shifting(void)
		{
			//Reset mouse.
			ref_mouse.ResetAllButtons();

			b_finalize_shifting = false;
			ref_editor.SetState_Idle();
		}

		void editor_pipeline::check_user_input_actions(void)
		{
			// n.b. 
			// user can start doing something in (at least) two ways
			// 1. by UI -> the UI calls an Editor function that changes the editor state
			//             for example: object insertion, terrain change
			// 2. by user input -> for example: shifting objects

			// here we check only user input actions

			auto selection = igame.Selos();
			if (selection->Count() >= 1)
			{
				/*
					OBJECTS SHIFTING
				*/
				{
					std::shared_ptr<ObjsList> objects_to_shift = nullptr;
					if (check_object_shifting(selection, objects_to_shift))
					{
						initialize_object_shifting(objects_to_shift);
						return;
					}
				}

				/*
					OBJECTS DELETION
				*/
				{
					std::shared_ptr<ObjsList> objects_to_shift = nullptr;
					if (check_object_deletion())
					{
						perform_objects_deletion(selection);
						return;
					}
				}
			}
		}

		bool editor_pipeline::check_object_shifting(const std::shared_ptr<ObjsList> selection, std::shared_ptr<ObjsList>& objects_to_shift)
		{
			// Do NOT perform shifting if left button isn't holden
			// while terrain is being changed at the same time
			if (ref_mouse.LeftHold == false || b_changing_terrain == true)
				return false;

			auto first_obj = selection->Get(0);

			//I start to move a settlement
			if (first_obj->IsBuilding() == true && ref_keyboard.IsKeyNotReleased(GLFW_KEY_LEFT_SHIFT) == true)
			{
				auto buildings = std::static_pointer_cast<Building>(first_obj)->GetSettlement()->GetBuildings();
				objects_to_shift = buildings;
				return true;
			}

			// I start to move a multipleselection (i.e. units or decorations) or a single GObject (i.e. unit/building/decoration)
			objects_to_shift = selection;
			return true;
		}

		bool editor_pipeline::check_object_deletion()
		{
			return ref_keyboard.IsKeyPressed(GLFW_KEY_DELETE);
		}

		void editor_pipeline::handle_controls(void)
		{
			// check condition
			bool b_condition =
				b_changing_terrain == false &&
				b_inserting_object == false &&
				b_drawing_areas == false &&
				b_shifting_object == false &&
				b_gui_any_dialog_window_active == false &&
				b_imgui_capture_keyboard == false;
			if (b_condition == false)
				return;

			// ESC
			if (b_changing_terrain == false && ref_keyboard.IsKeyPressed(GLFW_KEY_ESCAPE))
			{
				handle_controls_ESC();
				return;
			}

			// SPACE or MIDDLE CLICK
			if (ref_keyboard.IsKeyPressed(GLFW_KEY_SPACE) || ref_mouse.MiddleClick)
			{
				handle_controls_SPACE();
				return;
			}
		}

		void editor_pipeline::handle_controls_ESC(void)
		{
			if (b_gui_any_iframe_focused == false)
			{
				if (sp_minimap->IsOpen() == false)
				{
					igame.Quit();
				}
				else
				{
					sp_minimap->Close();
				}
			}
		}
		bool editor_pipeline::picking_condition(void)
		{
			if (game_pipeline::picking_condition() == false)
				return false;

			if (b_user_is_performing_actions)
				return false;

			return true;
		}

		// static function
		uint32_t editor_pipeline::set_shifting_positions(const bool b_only_for_not_placeable)
		{
			uint32_t validPositions = 0;

			auto& mouse = rattlesmake::peripherals::mouse::get_instance();
			auto& editor = *Engine::GetInstance().GetEnvironment()->AsEditor();
			auto objects_to_shift = editor.movingObject.objectsToShift.lock();
			auto focused_object = editor.movingObject.focusedObject.lock();

			//Get the current position of the GObject that acts as a center of mass in the shifting
			const uint32_t oldXPos = focused_object->GetPositionX();
			const uint32_t oldYPos = focused_object->GetPositionY();

			//Compute the new position of the GObject that acts as a center of mass in the shifting
			const uint32_t dx = mouse.GetXMapCoordinate() - editor.movingObject.StartXMouse;
			const uint32_t dy = mouse.GetY2DMapCoordinate() - editor.movingObject.StartYMouse;
			const uint32_t newPosX = editor.movingObject.startObjectXPos + dx;
			const uint32_t newPosY = editor.movingObject.startObjectYPos + dy;

			//For each selected unit, shift it considering its offset with the GObject that acts as a center of mass in the shifting
			auto lastIt = objects_to_shift->cend();
			for (auto firstIt = objects_to_shift->cbegin(); firstIt != lastIt; firstIt++)
			{
				const std::shared_ptr<GObject> obj = (*firstIt).lock();

				if (b_only_for_not_placeable == true && obj->IsPlaceable() == true)
					continue;

				//Compute offset between current positions.
				//Obviously, when obj is the GObject that acts as a center of mass in the shifting, offsets are zero.
				const uint32_t xOffset = (obj->GetPositionX() - oldXPos);
				const uint32_t yOffset = (obj->GetPositionY() - oldYPos);

				//Shift the GObject in its new position.
				obj->SetPosition(newPosX + xOffset, newPosY + yOffset);

				//Check placeability. If the GObject is placeable, increment by 1 the number of placeble selected GObjects.
				obj->CheckPlaceability();
				validPositions += obj->IsPlaceable();
			}

			return validPositions;
		}
	};
};
