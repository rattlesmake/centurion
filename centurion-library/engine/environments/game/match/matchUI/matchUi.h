/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <environments/game/igameUi.h>
#include <environments/game/match/matchUI/bottombar.h>
#include <environments/game/match/matchUI/topbar.h>

class GObject;
class Match;

class MatchUI : public IGameUI
{
public:
	~MatchUI(void);
	MatchUI(const MatchUI& other) = delete;
	MatchUI& operator=(const MatchUI& other) = delete;
	
	[[nodiscard]] std::shared_ptr<Command> GetCommandByPriority(const uint8_t priority) const;
	[[nodiscard]] uint8_t GetNumberOfCommands(void);
	[[nodiscard]] std::weak_ptr<gui::Iframe> GetIframeById(const std::string& id) const;
	void Render(void) override;
	void Clear(void);
	
	[[nodiscard]] bool IsHovering(void) const;
private:
	MatchUI(void);
	void SetOwnerWeakRef(const std::shared_ptr<Match>& _owner);

	std::unique_ptr<MatchUI_BottomBar> bottomBar;
	std::unique_ptr<MatchUI_TopBar> topBar;

	friend class Match;
};
