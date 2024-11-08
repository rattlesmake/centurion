#include "igameUi.h"
#include <environments/game/match/matchUI/matchUi.h>
#include "editor/editorUi.h"

#include <dialogWindows.h>

IGameUI::IGameUI(void)
{
}

IGameUI::~IGameUI(void)
{
}

EditorUI* IGameUI::AsEditorUI(void)
{
	return (EditorUI*)(this);
}

MatchUI* IGameUI::AsMatchUI(void)
{
	return (MatchUI*)(this);
}

std::string IGameUI::GetInfoText(void) const
{
	return this->infoText;
}

void IGameUI::UpdateInfoText(std::string& infoText)
{
	this->infoText = std::move(infoText);
}
