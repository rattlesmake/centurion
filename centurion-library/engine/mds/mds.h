/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>
#include <ui.h>
#include <environments/ienvironment.h>

namespace centurion
{
	namespace assets
	{
		class xml_classes;
		class xml_scripts;
		class xml_values;
	};
};

/// <summary>
/// Mod Development Software
/// </summary>
class MDS : public IEnvironment
{
public:
	MDS(const MDS& other) = delete;
	MDS& operator = (const MDS& other) = delete;
	~MDS(void);

	[[nodiscard]] static std::shared_ptr<MDS> CreateMDS(void);

	/// <summary>
	/// This static function returns an instance of the MDS environment.
	/// It's returned a reference. This approach guarantees both correctly-destruction and thread-safing.
	/// </summary>
	[[nodiscard]] static MDS& GetInstance(void);

	/// <summary>
	/// This function, frame by frame, runs the MDS environment.
	/// </summary>
	void Run(void) override;
private:
	///Constructor and destructor can't be called out of a member of the class.
	explicit MDS(void);

	std::shared_ptr<centurion::assets::xml_classes> xml_classes;
	std::shared_ptr<centurion::assets::xml_scripts> xml_scripts;
	std::shared_ptr<centurion::assets::xml_values> xml_values;

	/*
		classes
	*/
	std::vector<std::string> classes_names;
	std::vector<bool> classes_bools;
	std::string selected_class;
};
