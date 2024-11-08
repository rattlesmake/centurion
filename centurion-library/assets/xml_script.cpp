#include "xml_script.h"

#include <unordered_map>
#include <services/pyservice.h>

namespace centurion
{
    namespace assets
    {
        #pragma region Constructor and destructor:
        xml_script::xml_script(std::string&& _id, const bool _bAutoExecute, const bool _bAssociableToCmd, const bool _bNeedTarget, requiredCmdParams_t&& _set) :
            id(std::move(_id)), b_auto_execute(_bAutoExecute),
            b_associable_to_cmd(_bAssociableToCmd), b_need_target(_bNeedTarget),
            required_cmd_params(std::move(_set))
        {
        }

        xml_script::~xml_script(void)
        {
        }
        #pragma endregion


        #pragma region Static public members:
        std::shared_ptr<xml_script> xml_script::create(std::string _id, const bool _bAutoExecute, const bool _bAssociableToCmd, const bool _bNeedTarget, requiredCmdParams_t _set)
        {
            return std::shared_ptr<xml_script>{ new xml_script{ std::move(_id), _bAutoExecute, _bAssociableToCmd, _bNeedTarget, std::move(_set) } };
        }
        #pragma endregion


        #pragma region Public members:
        bool xml_script::is_autoexecutable(void) const noexcept
        {
            return this->b_auto_execute;
        }

        bool xml_script::is_associable_to_cmd(void) const noexcept
        {
            return this->b_associable_to_cmd;
        }

        bool xml_script::is_target_required(void) const noexcept
        {
            return this->b_need_target;
        }

        const std::string& xml_script::get_id_cref(void) const noexcept
        {
            return this->id;
        }

        std::string xml_script::get_id(void) const noexcept
        {
            return this->id;
        }

        const requiredCmdParams_t& xml_script::get_required_cmd_params_cref(void) const noexcept
        {
            return this->required_cmd_params;
        }

        std::string xml_script::get_arguments(const providedCmdParams_t& cmd_params) const
        {
            enum class EngineCommands
            {
                e_command,
                e_target,
            };

            const static std::unordered_map<EngineCommands, std::string> additionEngineArgs =
            {
                { EngineCommands::e_command, "'cmd':" + COMMAND_BINDING_PYTHON + "," },
                { EngineCommands::e_target, "'target':" + OBJECT_BINDING_PYTHON + ".GetMyTarget()," },
            };

            static const string defaultEngineArgs
            {
                "({"
                    "'match':admin.GetEngine().GetEnvironment(),"
                    "'this':" + OBJECT_BINDING_PYTHON + ","
            };

            // Compute args
            std::string args{ defaultEngineArgs };
            if (this->b_associable_to_cmd == true)
                args += additionEngineArgs.at(EngineCommands::e_command);
            if (this->b_need_target == true)
                args += additionEngineArgs.at(EngineCommands::e_target);

            // Include command args
            // TODO - Improve with type different from strings
            for (auto const& param : cmd_params)
            {
                args += "'" + param.name + "':\"" + param.value + "\",";
            }
            args += "})";

            return args;
        }
        #pragma endregion
    }
}

