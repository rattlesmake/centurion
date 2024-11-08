/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <memory>
#include <string>

#include "xml_class_command.h"

namespace centurion
{
	namespace assets
	{
		class xml_script
		{
		private:
			xml_script(std::string&& _id, const bool _bAutoExecute, const bool _bAssociableToCmd, const bool _bNeedTarget, requiredCmdParams_t&& _set);

			const std::string id;
			const bool b_auto_execute;
			const bool b_associable_to_cmd;
			const bool b_need_target;
			const requiredCmdParams_t required_cmd_params;
		public:
			xml_script(const xml_script& other) = delete;
			~xml_script(void);

			xml_script& operator=(const xml_script& other) = delete;

			[[nodiscard]] static std::shared_ptr<xml_script> create(std::string _id, const bool _bAutoExecute, const bool _bAssociableToCmd, const bool _bNeedTarget, requiredCmdParams_t _set);

			[[nodiscard]] bool is_autoexecutable(void) const noexcept;
			[[nodiscard]] bool is_associable_to_cmd(void) const noexcept;
			[[nodiscard]] bool is_target_required(void) const noexcept;
			[[nodiscard]] const std::string& get_id_cref(void) const noexcept;
			[[nodiscard]] std::string get_id(void) const noexcept;
			[[nodiscard]] const requiredCmdParams_t& get_required_cmd_params_cref(void) const noexcept;
			[[nodiscard]] std::string get_arguments(const providedCmdParams_t& cmd_params) const;
		};
	};
};
