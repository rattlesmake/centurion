/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

class DebugUI 
{
public:
	DebugUI(void);
	void setStatus(const bool b) { debuguiIsActive = b; }
	bool getStatus(void) { return debuguiIsActive; }
	void Render(void);
	~DebugUI(void);
private:
	bool debuguiIsActive = false;
};