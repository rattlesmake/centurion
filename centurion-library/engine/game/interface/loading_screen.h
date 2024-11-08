/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <iframe/iframe.h>
#include <future>

class LoadingScreen
{
public:
	[[nodiscard]] static LoadingScreen* GetInstance(void);
	static void DeleteInstance(void);
	
	[[nodiscard]] bool IsLoading(void) const;
	void Init(std::tuple<std::function<void(bool, string)>, bool, string> f);
	void Run(void);
	void SetLoadingElementName(string&& name);
	void SetTip(string&& tip);
private:
	LoadingScreen(void);
	~LoadingScreen(void) = default;

	void Create(void);
	void Render(void);

	static LoadingScreen* loadingScreen;
	std::shared_ptr<gui::Iframe> iframe;
	bool bIsLoading = false;
	std::function<void(bool, string)> funct;
	std::future<void> futureToExecuteInParallel;
};
