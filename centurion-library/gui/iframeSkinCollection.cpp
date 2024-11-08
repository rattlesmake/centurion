#include "iframeSkinCollection.h"

#include <vector>

#include <ui.h>
#include <engine.h>
#include <fileservice.h>
#include <services/logservice.h>
#include <zipservice.h>
#include <encode_utils.h>

#include <png_shader.h>

namespace gui
{
	namespace IframeSkinCollection
	{
#pragma region Static attributes initialization:
		std::unordered_map<std::string, std::shared_ptr<IframeSkin>> IframeSkin::SKINS;
#pragma endregion


#pragma region Static public members:
		void IframeSkin::Initialize(void)
		{
			try
			{
				IframeSkin::SKINS.clear();

				std::vector<std::string> listOfSkins = rattlesmake::services::zip_service::get_instance().get_all_files_within_folder("?data.zip", "interface/iframe", "xml");

				for (auto const& file : listOfSkins)
				{
					string xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?data.zip", file);

					tinyxml2::XMLDocument xmlFile;
					if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS)
						throw std::runtime_error("Cannot open file");  // TODO excption - Sostituire con opportuna eccezione

					std::shared_ptr<IframeSkin> skinSP{ new IframeSkin() };
					IframeSkin& skin = (*skinSP);

					uint8_t r = 0, g = 0, b = 0, a = 0;
					skin.name = xmlFile.FirstChildElement("iframe")->Attribute("name");
					skin.back_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("background"), "image_name"));
					skin.topleft_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("top_left"), "image_name"));
					skin.topright_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("top_right"), "image_name"));
					skin.bottomright_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("bottom_right"), "image_name"));
					skin.bottomleft_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("bottom_left"), "image_name"));
					skin.right_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("right"), "image_name"));
					skin.left_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("left"), "image_name"));
					skin.top_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("top"), "image_name"));
					skin.bottom_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("bottom"), "image_name"));
					skin.button_x_name = Encode::FixImageName(tinyxml2::TryParseStrAttribute(xmlFile.FirstChildElement("iframe")->FirstChildElement("close_button"), "image_name"));

					// The following instruction will store all variables for each of the current skin's elements
					// Title data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("title") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("title");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the title::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.titleData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.titleData.font = "tahoma_12";

						skin.titleData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Text data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("text") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("text");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the text::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.textData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.textData.font = "tahoma_11";

						skin.textData.color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Tabs data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("tab") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("tab");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tab::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.tabData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("hoverBackground") != nullptr)
						{
							auto hbg = el->FirstChildElement("hoverBackground");
							try
							{
								r = hbg->IntAttribute("r");
								g = hbg->IntAttribute("g");
								b = hbg->IntAttribute("b");
								a = hbg->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tab::hoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.tabData.hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("selectedBackground") != nullptr)
						{
							auto sbg = el->FirstChildElement("selectedBackground");
							try
							{
								r = sbg->IntAttribute("r");
								g = sbg->IntAttribute("g");
								b = sbg->IntAttribute("b");
								a = sbg->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tab::selectedBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.tabData.selected_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tab::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.tabData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.tabData.font = "tahoma_11";

						//Active case as well
						if (el->FirstChildElement("font")->Attribute("active_name") != "")
							skin.tabData.active_font = el->FirstChildElement("font")->Attribute("active_name");
						else
							skin.tabData.active_font = "tahoma_11_bold";

						skin.tabData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Button data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("button") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("button");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the button::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.buttonData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.buttonData.font = "tahoma_15";

						skin.buttonData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

						//Set up default buttons images according to the skin names
						//If there is any "image" tag, assign respective values; assign default ones otherwise.
						if (el->FirstChildElement("image") != nullptr)
						{
							auto img = el->FirstChildElement("image");
							if (img->Attribute("name") != nullptr)
								skin.buttonData.image_name = Encode::FixImageName(img->Attribute("name"));
							//else
								//skin.buttonData.image_name = "buttons/button_1";

							if (img->Attribute("hover") != nullptr)
								skin.buttonData.hover_image_name = Encode::FixImageName(img->Attribute("hover"));
							//else
							//	skin.buttonData.hover_image_name = "buttons/button_1_hover";

							if (img->Attribute("pressed") != nullptr)
								skin.buttonData.pressed_image_name = Encode::FixImageName(img->Attribute("pressed"));
							//else
								//skin.buttonData.pressed_image_name = "buttons/button_1_pressed";

							if (img->Attribute("disabled") != nullptr)
								skin.buttonData.disabled_image_name = Encode::FixImageName(img->Attribute("disabled"));
							else
								skin.buttonData.disabled_image_name = "common/buttons/button_1_disabled";
						}
						else
						{
							skin.buttonData.image_name = "common/buttons/button_1";
							skin.buttonData.hover_image_name = "common/buttons/button_1_hover";
							skin.buttonData.pressed_image_name = "common/buttons/button_1_pressed";
							skin.buttonData.disabled_image_name = "common/buttons/button_1_disabled";
						}
					}

					//Textlist data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("textList") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("textList");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textListData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("hoverBackground") != nullptr)
						{
							auto hbgr = el->FirstChildElement("hoverBackground");
							try
							{
								r = hbgr->IntAttribute("r");
								g = hbgr->IntAttribute("g");
								b = hbgr->IntAttribute("b");
								a = hbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::hoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textListData.hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("selectedBackground") != nullptr)
						{
							auto sbgr = el->FirstChildElement("selectedBackground");
							try
							{
								r = sbgr->IntAttribute("r");
								g = sbgr->IntAttribute("g");
								b = sbgr->IntAttribute("b");
								a = sbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::selectedBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textListData.selected_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("pressedBackground") != nullptr)
						{
							auto pbgr = el->FirstChildElement("pressedBackground");
							try
							{
								r = pbgr->IntAttribute("r");
								g = pbgr->IntAttribute("g");
								b = pbgr->IntAttribute("b");
								a = pbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::pressedBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textListData.pressed_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("sliderBackground") != nullptr)
						{
							auto slbgr = el->FirstChildElement("sliderBackground");
							try
							{
								r = slbgr->IntAttribute("r");
								g = slbgr->IntAttribute("g");
								b = slbgr->IntAttribute("b");
								a = slbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::sliderBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textListData.slider_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("slider") != nullptr)
						{
							auto sld = el->FirstChildElement("slider");
							try
							{
								r = sld->IntAttribute("r");
								g = sld->IntAttribute("g");
								b = sld->IntAttribute("b");
								a = sld->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::slider tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textListData.slider_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("sliderHover") != nullptr)
						{
							auto sldh = el->FirstChildElement("sliderHover");
							try
							{
								r = sldh->IntAttribute("r");
								g = sldh->IntAttribute("g");
								b = sldh->IntAttribute("b");
								a = sldh->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::sliderHover tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textListData.slider_hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("sliderPressed") != nullptr)
						{
							auto sldp = el->FirstChildElement("sliderPressed");
							try
							{
								r = sldp->IntAttribute("r");
								g = sldp->IntAttribute("g");
								b = sldp->IntAttribute("b");
								a = sldp->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::sliderPressed tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textListData.slider_pressed_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("border") != nullptr)
						{
							auto brd = el->FirstChildElement("border");
							try
							{
								r = brd->IntAttribute("r");
								g = brd->IntAttribute("g");
								b = brd->IntAttribute("b");
								a = brd->IntAttribute("a");
								skin.textListData.has_border = true;
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::border tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
								skin.textListData.has_border = false;
							}
						}
						else
						{
							skin.textListData.has_border = false;
						}
						skin.textListData.border_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textList::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.textListData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.textListData.font = "tahoma_11";

						skin.textListData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Textinput data 
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("textInput") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("textInput");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
								skin.textInputData.has_background = true;
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInput::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
								skin.textInputData.has_background = false;
							}
						}
						else
						{
							skin.textInputData.has_background = false;
						}
						skin.textInputData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInput::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textInputData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0; g = 0; b = 0, a = 0;

						if (el->FirstChildElement("border") != nullptr)
						{
							auto brd = el->FirstChildElement("border");
							try
							{
								r = brd->IntAttribute("r");
								g = brd->IntAttribute("g");
								b = brd->IntAttribute("b");
								a = brd->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInput::border tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.textInputData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.textInputData.font = "tahoma_11";

						skin.textInputData.border_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Textinput multiline data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("textInputMultiline") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("textInputMultiline");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
								skin.textInputMultilineData.has_background = true;
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInputMultiline::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
								skin.textInputMultilineData.has_background = false;
							}
						}
						else
						{
							skin.textInputMultilineData.has_background = false;
						}
						skin.textInputMultilineData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInputMultiline::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textInputMultilineData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("sliderBackground") != nullptr)
						{
							auto sbgr = el->FirstChildElement("sliderBackground");
							try
							{
								r = sbgr->IntAttribute("r");
								g = sbgr->IntAttribute("g");
								b = sbgr->IntAttribute("b");
								a = sbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInputMultiline::sliderBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textInputMultilineData.slider_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("slider") != nullptr)
						{
							auto sld = el->FirstChildElement("slider");
							try
							{
								r = sld->IntAttribute("r");
								g = sld->IntAttribute("g");
								b = sld->IntAttribute("b");
								a = sld->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInputMultiline::slider tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textInputMultilineData.slider_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("sliderHover") != nullptr)
						{
							auto sldh = el->FirstChildElement("sliderHover");
							try
							{
								r = sldh->IntAttribute("r");
								g = sldh->IntAttribute("g");
								b = sldh->IntAttribute("b");
								a = sldh->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInputMultiline::sliderHover tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textInputMultilineData.slider_hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("sliderPressed") != nullptr)
						{
							auto sldp = el->FirstChildElement("sliderPressed");
							try
							{
								r = sldp->IntAttribute("r");
								g = sldp->IntAttribute("g");
								b = sldp->IntAttribute("b");
								a = sldp->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInputMultiline::sliderPressed tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.textInputMultilineData.slider_pressed_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0; g = 0; b = 0, a = 0;

						if (el->FirstChildElement("border") != nullptr)
						{
							auto brd = el->FirstChildElement("border");
							try
							{
								r = brd->IntAttribute("r");
								g = brd->IntAttribute("g");
								b = brd->IntAttribute("b");
								a = brd->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the textInputMultiline::border tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.textInputMultilineData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.textInputMultilineData.font = "tahoma_11";

						skin.textInputMultilineData.border_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//ComboBox data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("comboBox") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("comboBox");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the comboBox::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.comboBoxData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("hoverBackground") != nullptr)
						{
							auto hbgr = el->FirstChildElement("hoverBackground");
							try
							{
								r = hbgr->IntAttribute("r");
								g = hbgr->IntAttribute("g");
								b = hbgr->IntAttribute("b");
								a = hbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the comboBox::hoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.comboBoxData.hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("arrowBackground") != nullptr)
						{
							auto abgr = el->FirstChildElement("arrowBackground");
							try
							{
								r = abgr->IntAttribute("r");
								g = abgr->IntAttribute("g");
								b = abgr->IntAttribute("b");
								a = abgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the comboBox::arrowBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.comboBoxData.arrow_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("arrowHoverBackground") != nullptr)
						{
							auto ahbgr = el->FirstChildElement("arrowHoverBackground");
							try
							{
								r = ahbgr->IntAttribute("r");
								g = ahbgr->IntAttribute("g");
								b = ahbgr->IntAttribute("b");
								a = ahbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the comboBox::arrowHoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.comboBoxData.arrow_hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("selectableHoverBackground") != nullptr)
						{
							auto shbgr = el->FirstChildElement("selectableHoverBackground");
							try
							{
								r = shbgr->IntAttribute("r");
								g = shbgr->IntAttribute("g");
								b = shbgr->IntAttribute("b");
								a = shbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the comboBox::selectableHoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.comboBoxData.selectable_hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("selectablePressedBackground") != nullptr)
						{
							auto spbg = el->FirstChildElement("selectablePressedBackground");
							try
							{
								r = spbg->IntAttribute("r");
								g = spbg->IntAttribute("g");
								b = spbg->IntAttribute("b");
								a = spbg->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the comboBox::arrowHoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.comboBoxData.selectable_pressed_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the comboBox::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.comboBoxData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.comboBoxData.font = "tahoma_11";

						skin.comboBoxData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//CheckBox data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("checkBox") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("checkBox");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the checkBox::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.checkBoxData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("hoverBackground") != nullptr)
						{
							auto hbgr = el->FirstChildElement("hoverBackground");
							try
							{
								r = hbgr->IntAttribute("r");
								g = hbgr->IntAttribute("g");
								b = hbgr->IntAttribute("b");
								a = hbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the checkBox::hoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.checkBoxData.hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("selectedBackground") != nullptr)
						{
							auto sbgr = el->FirstChildElement("selectedBackground");
							try
							{
								r = sbgr->IntAttribute("r");
								g = sbgr->IntAttribute("g");
								b = sbgr->IntAttribute("b");
								a = sbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the checkBox::selectedBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.checkBoxData.selected_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("markColor") != nullptr)
						{
							auto mkClr = el->FirstChildElement("markColor");
							try
							{
								r = mkClr->IntAttribute("r");
								g = mkClr->IntAttribute("g");
								b = mkClr->IntAttribute("b");
								a = mkClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the checkBox::markColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.checkBoxData.mark_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the checkBox::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.checkBoxData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.checkBoxData.font = "tahoma_11";

						skin.checkBoxData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//InputInt data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("inputInt") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("inputInt");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the inputInt::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.inputIntData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("buttonBackground") != nullptr)
						{
							auto bbgr = el->FirstChildElement("buttonBackground");
							try
							{
								r = bbgr->IntAttribute("r");
								g = bbgr->IntAttribute("g");
								b = bbgr->IntAttribute("b");
								a = bbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the inputInt::buttonBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.inputIntData.button_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("buttonSelectedBackground") != nullptr)
						{
							auto bsbgr = el->FirstChildElement("buttonSelectedBackground");
							try
							{
								r = bsbgr->IntAttribute("r");
								g = bsbgr->IntAttribute("g");
								b = bsbgr->IntAttribute("b");
								a = bsbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the inputInt::buttonSelectedBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.inputIntData.button_selected_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("buttonHoverBackground") != nullptr)
						{
							auto bhbgr = el->FirstChildElement("buttonHoverBackground");
							try
							{
								r = bhbgr->IntAttribute("r");
								g = bhbgr->IntAttribute("g");
								b = bhbgr->IntAttribute("b");
								a = bhbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the inputInt::buttonHoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.inputIntData.button_hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the inputInt::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.inputIntData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.inputIntData.font = "tahoma_11";

						skin.inputIntData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Slider data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("slider") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("slider");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the slider::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.sliderData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("pressedBackground") != nullptr)
						{
							auto pbgr = el->FirstChildElement("pressedBackground");
							try
							{
								r = pbgr->IntAttribute("r");
								g = pbgr->IntAttribute("g");
								b = pbgr->IntAttribute("b");
								a = pbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the slider::pressedBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.sliderData.pressed_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("barBackground") != nullptr)
						{
							auto bbgr = el->FirstChildElement("barBackground");
							try
							{
								r = bbgr->IntAttribute("r");
								g = bbgr->IntAttribute("g");
								b = bbgr->IntAttribute("b");
								a = bbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the slider::barBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.sliderData.bar_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("barHoverBackground") != nullptr)
						{
							auto bhbgr = el->FirstChildElement("barBackground");
							try
							{
								r = bhbgr->IntAttribute("r");
								g = bhbgr->IntAttribute("g");
								b = bhbgr->IntAttribute("b");
								a = bhbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the slider::barHoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.sliderData.bar_hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("barPressedBackground") != nullptr)
						{
							auto bpbgr = el->FirstChildElement("barPressedBackground");
							try
							{
								r = bpbgr->IntAttribute("r");
								g = bpbgr->IntAttribute("g");
								b = bpbgr->IntAttribute("b");
								a = bpbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the slider::barPressedBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.sliderData.bar_pressed_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("border") != nullptr)
						{
							auto brd = el->FirstChildElement("border");
							try
							{
								r = brd->IntAttribute("r");
								g = brd->IntAttribute("g");
								b = brd->IntAttribute("b");
								a = brd->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the slider::border tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.sliderData.border_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the slider::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.sliderData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.sliderData.font = "tahoma_11";

						//If there isn't any type specified, a default one should be applied
						if (el->FirstChildElement("type")->Attribute("name") != "")
							skin.sliderData.type = el->FirstChildElement("type")->Attribute("name");
						else
							skin.sliderData.type = "circle";

						skin.sliderData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Progress Bar data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("progressBar") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("progressBar");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the progressBar::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.progressBarData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("barColor") != nullptr)
						{
							auto brClr = el->FirstChildElement("barColor");
							try
							{
								r = brClr->IntAttribute("r");
								g = brClr->IntAttribute("g");
								b = brClr->IntAttribute("b");
								a = brClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the progressBar::barBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.progressBarData.bar_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the progressBar::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.progressBarData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.progressBarData.font = "tahoma_11";

						skin.progressBarData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Buffering Bar data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("bufferingBar") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("bufferingBar");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the bufferingBar::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.bufferingBarData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("barColor") != nullptr)
						{
							auto brClr = el->FirstChildElement("barColor");
							try
							{
								r = brClr->IntAttribute("r");
								g = brClr->IntAttribute("g");
								b = brClr->IntAttribute("b");
								a = brClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the bufferingBar::barBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.bufferingBarData.bar_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Spinner data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("spinner") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("spinner");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("color") != nullptr)
						{
							auto col = el->FirstChildElement("color");
							try
							{
								r = col->IntAttribute("r");
								g = col->IntAttribute("g");
								b = col->IntAttribute("b");
								a = col->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the spinner::color tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.spinnerData.color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Tree (node) data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("tree") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("tree");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("nodeBackground") != nullptr)
						{
							auto nbgr = el->FirstChildElement("nodeBackground");
							try
							{
								r = nbgr->IntAttribute("r");
								g = nbgr->IntAttribute("g");
								b = nbgr->IntAttribute("b");
								a = nbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::nodeBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.node_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("nodeHoverBackground") != nullptr)
						{
							auto nhbgr = el->FirstChildElement("nodeHoverBackground");
							try
							{
								r = nhbgr->IntAttribute("r");
								g = nhbgr->IntAttribute("g");
								b = nhbgr->IntAttribute("b");
								a = nhbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::nodeHoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.node_hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("nodePressedBackground") != nullptr)
						{
							auto npbgr = el->FirstChildElement("nodePressedBackground");
							try
							{
								r = npbgr->IntAttribute("r");
								g = npbgr->IntAttribute("g");
								b = npbgr->IntAttribute("b");
								a = npbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::nodeActiveBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.node_pressed_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("sliderBackground") != nullptr)
						{
							auto sbgr = el->FirstChildElement("sliderBackground");
							try
							{
								r = sbgr->IntAttribute("r");
								g = sbgr->IntAttribute("g");
								b = sbgr->IntAttribute("b");
								a = sbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::sliderBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.slider_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("slider") != nullptr)
						{
							auto sld = el->FirstChildElement("slider");
							try
							{
								r = sld->IntAttribute("r");
								g = sld->IntAttribute("g");
								b = sld->IntAttribute("b");
								a = sld->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::slider tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.slider_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("sliderHover") != nullptr)
						{
							auto sldh = el->FirstChildElement("sliderHover");
							try
							{
								r = sldh->IntAttribute("r");
								g = sldh->IntAttribute("g");
								b = sldh->IntAttribute("b");
								a = sldh->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::sliderHover tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.slider_hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("sliderPressed") != nullptr)
						{
							auto sldp = el->FirstChildElement("sliderPressed");
							try
							{
								r = sldp->IntAttribute("r");
								g = sldp->IntAttribute("g");
								b = sldp->IntAttribute("b");
								a = sldp->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::sliderActive tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.slider_pressed_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("resizeGrip") != nullptr)
						{
							auto rzgr = el->FirstChildElement("resizeGrip");
							try
							{
								r = rzgr->IntAttribute("r");
								g = rzgr->IntAttribute("g");
								b = rzgr->IntAttribute("b");
								a = rzgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::resizeGrip tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.resize_grip_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("resizeGripHover") != nullptr)
						{
							auto rzgh = el->FirstChildElement("resizeGripHover");
							try
							{
								r = rzgh->IntAttribute("r");
								g = rzgh->IntAttribute("g");
								b = rzgh->IntAttribute("b");
								a = rzgh->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::resizeGripHover tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.resize_grip_hover_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("resizeGripPressed") != nullptr)
						{
							auto rzgp = el->FirstChildElement("resizeGripPressed");
							try
							{
								r = rzgp->IntAttribute("r");
								g = rzgp->IntAttribute("g");
								b = rzgp->IntAttribute("b");
								a = rzgp->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::resizeGripPressed tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.treeData.resize_grip_pressed_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the tree::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.treeData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.treeData.font = "tahoma_11";

						//If there isn't any big font specified, the default one should be applied
						if (el->FirstChildElement("fontBig")->Attribute("name") != "")
							skin.treeData.font_big = el->FirstChildElement("fontBig")->Attribute("name");
						else
							skin.treeData.font_big = "tahoma_13";

						skin.treeData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					//Multiple Choice data
					if (xmlFile.FirstChildElement("iframe")->FirstChildElement("multipleChoice") != nullptr)
					{
						auto el = xmlFile.FirstChildElement("iframe")->FirstChildElement("multipleChoice");
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("background") != nullptr)
						{
							auto bgrn = el->FirstChildElement("background");
							try
							{
								r = bgrn->IntAttribute("r");
								g = bgrn->IntAttribute("g");
								b = bgrn->IntAttribute("b");
								a = bgrn->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the multipleChoice::background tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.multipleChoiceData.background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("hoverBackground") != nullptr)
						{
							auto hbgr = el->FirstChildElement("hoverBackground");
							try
							{
								r = hbgr->IntAttribute("r");
								g = hbgr->IntAttribute("g");
								b = hbgr->IntAttribute("b");
								a = hbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the multipleChoice::hoverBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.multipleChoiceData.hover_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("pressedBackground") != nullptr)
						{
							auto pbgr = el->FirstChildElement("pressedBackground");
							try
							{
								r = pbgr->IntAttribute("r");
								g = pbgr->IntAttribute("g");
								b = pbgr->IntAttribute("b");
								a = pbgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the multipleChoice::pressedBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.multipleChoiceData.pressed_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("optionBackground") != nullptr)
						{
							auto obgr = el->FirstChildElement("optionBackground");
							try
							{
								r = obgr->IntAttribute("r");
								g = obgr->IntAttribute("g");
								b = obgr->IntAttribute("b");
								a = obgr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the multipleChoice::optionBackground tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.multipleChoiceData.option_background_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("border") != nullptr)
						{
							auto brd = el->FirstChildElement("border");
							try
							{
								r = brd->IntAttribute("r");
								g = brd->IntAttribute("g");
								b = brd->IntAttribute("b");
								a = brd->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the multipleChoice::border tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}
						skin.multipleChoiceData.border_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
						r = 0, g = 0, b = 0, a = 0;

						if (el->FirstChildElement("textColor") != nullptr)
						{
							auto txtClr = el->FirstChildElement("textColor");
							try
							{
								r = txtClr->IntAttribute("r");
								g = txtClr->IntAttribute("g");
								b = txtClr->IntAttribute("b");
								a = txtClr->IntAttribute("a");
							}
							catch (...)
							{
								Logger::LogMessage lmsg = Logger::LogMessage("An error occurred while parsing the multipleChoice::textColor tag of the following iframe: " + file, "Warn", "gui", "Iframe", __FUNCTION__);
								Logger::Warn(lmsg);
							}
						}

						//If there isn't any font specified, the default one should be applied
						if (el->FirstChildElement("font")->Attribute("name") != "")
							skin.multipleChoiceData.font = el->FirstChildElement("font")->Attribute("name");
						else
							skin.multipleChoiceData.font = "tahoma_11";

						skin.multipleChoiceData.text_color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
					}

					skin.Create();

					// add skin to the unordered map
					IframeSkin::SKINS.insert({ skin.name, std::move(skinSP) });
				}
			}
			catch (const std::exception&)
			{
				throw;
			}
		}

		std::shared_ptr<IframeSkin> IframeSkin::GetSkin(const std::string& skinName)
		{
			if (IframeSkin::SKINS.contains(skinName) == false)
			{
				Logger::Warn("The requested skin with name \"" + skinName + "\" doesn't exist. emptyIframeMenu returned");
				return IframeSkin::SKINS.at("emptyIframeMenu");
			}
			return IframeSkin::SKINS.at(skinName);
		}
#pragma endregion


#pragma region Constructor and destructor:
		IframeSkin::IframeSkin(void)
		{
		}

		IframeSkin::~IframeSkin(void)
		{
		}
#pragma endregion


		void IframeSkin::Create(void)
		{
			//auto& imgService = ImageService::GetInstance();
			auto& imgShader = rattlesmake::image::png_shader::get_instance();
			if (back_name.empty() == false) back = imgShader.get_ui_image_data(back_name);
			if (left_name.empty() == false) left = imgShader.get_ui_image_data(left_name);
			if (top_name.empty() == false) top = imgShader.get_ui_image_data(top_name);
			if (right_name.empty() == false) right = imgShader.get_ui_image_data(right_name);
			if (bottom_name.empty() == false) bottom = imgShader.get_ui_image_data(bottom_name);
			if (topleft_name.empty() == false) topleft = imgShader.get_ui_image_data(topleft_name);
			if (topright_name.empty() == false) topright = imgShader.get_ui_image_data(topright_name);
			if (bottomright_name.empty() == false) bottomright = imgShader.get_ui_image_data(bottomright_name);
			if (bottomleft_name.empty() == false) bottomleft = imgShader.get_ui_image_data(bottomleft_name);
			if (button_x_name.empty() == false) button_x = imgShader.get_ui_image_data(button_x_name);

			back_name.clear(); left_name.clear(); top_name.clear(); right_name.clear(); bottomleft_name.clear();
			bottom_name.clear(); topleft_name.clear(); topright_name.clear(); bottomright_name.clear();

			this->bCreated = true;
		}

		void IframeSkin::Render(ImDrawList* drawList, const ImVec2& _position, const ImVec2& _size)
		{
			auto texID = 0;
			auto left_width = 0;
			auto right_width = 0;
			auto top_height = 0;
			auto bottom_height = 0;

			// skin topleft
			if (this->topleft)
			{
				texID = this->topleft->get_opengl_texture_id();
				left_width = this->topleft->get_image_width();
				top_height = this->topleft->get_image_height();
				auto _positionMin = ImVec2(_position.x - left_width, _position.y - top_height);
				auto _positionMax = ImVec2(_position.x, _position.y);
				drawList->AddImage((void*)(intptr_t)texID, _positionMin, _positionMax);
			}

			// skin topright
			if (this->topright)
			{
				texID = this->topright->get_opengl_texture_id();
				right_width = this->topright->get_image_width();
				auto _positionMin = ImVec2(_position.x + _size.x, _position.y - top_height);
				auto _positionMax = ImVec2(_position.x + _size.x + right_width, _position.y);
				drawList->AddImage((void*)(intptr_t)texID, _positionMin, _positionMax);
			}

			// skin top
			if (this->top)
			{
				texID = this->top->get_opengl_texture_id();
				auto _positionMin = ImVec2(_position.x, _position.y - top_height);
				auto _positionMax = ImVec2(_position.x + _size.x, _position.y);
				drawList->AddImage((void*)(intptr_t)texID, _positionMin, _positionMax);
			}

			// skin bottomleft
			if (this->bottomleft)
			{
				texID = this->bottomleft->get_opengl_texture_id();
				bottom_height = this->bottomleft->get_image_height();
				auto _positionMin = ImVec2(_position.x - left_width, _position.y + _size.y);
				auto _positionMax = ImVec2(_position.x, _position.y + _size.y + bottom_height);
				drawList->AddImage((void*)(intptr_t)texID, _positionMin, _positionMax);
			}

			// skin bottomright
			if (this->bottomright)
			{
				texID = this->bottomright->get_opengl_texture_id();
				auto _positionMin = ImVec2(_position.x + _size.x, _position.y + _size.y);
				auto _positionMax = ImVec2(_position.x + _size.x + right_width, _position.y + _size.y + bottom_height);
				drawList->AddImage((void*)(intptr_t)texID, _positionMin, _positionMax);
			}

			// skin left
			if (this->left)
			{
				texID = this->left->get_opengl_texture_id();
				auto _positionMin = ImVec2(_position.x - left_width, _position.y);
				auto _positionMax = ImVec2(_position.x, _position.y + _size.y);
				drawList->AddImage((void*)(intptr_t)texID, _positionMin, _positionMax);
			}

			// skin right
			if (this->right)
			{
				texID = this->right->get_opengl_texture_id();
				auto _positionMin = ImVec2(_position.x + _size.x, _position.y);
				auto _positionMax = ImVec2(_position.x + _size.x + right_width, _position.y + _size.y);
				drawList->AddImage((void*)(intptr_t)texID, _positionMin, _positionMax);
			}

			// skin bottom
			if (this->bottom)
			{
				texID = this->bottom->get_opengl_texture_id();
				auto _positionMin = ImVec2(_position.x, _position.y + _size.y);
				auto _positionMax = ImVec2(_position.x + _size.x, _position.y + _size.y + bottom_height);
				drawList->AddImage((void*)(intptr_t)texID, _positionMin, _positionMax);
			}

			// skin back
			if (this->back)
			{
				texID = this->back->get_opengl_texture_id();
				auto _positionMin = ImVec2(_position.x, _position.y);
				auto _positionMax = ImVec2(_position.x + _size.x, _position.y + _size.y);
				drawList->AddImage((void*)(intptr_t)texID, _positionMin, _positionMax);
			}
		}

		float IframeSkin::GetLeftWidth(void) const
		{
			auto sp = this->left;
			return (sp) ? static_cast<float>(sp->get_image_width()) : 0.f;
		}

		float IframeSkin::GetRightWidth(void) const
		{
			auto sp = this->right;
			return (sp) ? static_cast<float>(sp->get_image_width()) : 0.f;
		}

		float IframeSkin::GetTopHeight(void) const
		{
			auto sp = this->top;
			return (sp) ? static_cast<float>(sp->get_image_height()) : 0.f;
		}

		float IframeSkin::GetBottomHeight(void) const
		{
			auto sp = this->bottom;
			return (sp) ? static_cast<float>(sp->get_image_height()) : 0.f;
		}
	};
};
