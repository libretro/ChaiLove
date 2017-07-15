#ifdef __HAVE_TESTS__

#ifndef _TEST_H_INCLUDED_
#define _TEST_H_INCLUDED_

#include "../chaigame/src/Font.h"
#include "../chaigame/src/Config.h"

class Test {
public:
	void conf(chaigame::Config& t);
	bool load();
	void update(float delta);
	void draw();
	int tester = 0;
	chaigame::Font* thefont;
	chaigame::Font* secondfont;
};

#endif
#endif
