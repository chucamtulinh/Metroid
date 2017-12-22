#pragma once

class dxManager;

class Game
{
public:
	Game(void);
	~Game(void);

	bool init(HWND wndHandle);
	void shutdown(void);

	int update(void);
	void render(void);

private:
	// timer variables
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	LARGE_INTEGER timerFreq;
	float         anim_rate;
};
