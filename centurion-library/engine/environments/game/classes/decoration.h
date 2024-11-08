/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once
#include <environments/game/classes/gobject.h>

class Decoration : public GObject
{
public:
	#pragma region Constructors and destructor and operators:
	Decoration(glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource);
	Decoration(const Decoration& other) = delete;
	virtual ~Decoration(void);
	Decoration& operator=(const Decoration& other) = delete;
	#pragma endregion

	/// <summary>
	/// This function performs the current decorations game logic checks.
	/// </summary>
	void ApplyGameLogics(void) override;

	/// <summary>
	/// This function checks if a decoration can be positioned during its creation.
	/// </summary>
	void CheckPlaceability(const bool bAlwaysPlaceable = false) override;

	void GetBinRepresentation(std::vector<uint8_t>& data, const bool calledByChild) const;
protected:
	std::ostream& Serialize(std::ostream& out, const bool calledByChild) const override;
};
