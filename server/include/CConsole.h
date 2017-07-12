#ifndef __CCONSOLE_H__
#define __CCONSOLE_H__

#define msg  ( Console::instance()->colorMsg("Msg", 32) )
//#define dbg  ( Console::instance()->colorMsg("Dbg", 36) )
#define dbg  ( Console::instance()->colorErr("Debug", __FILE__, __LINE__, 36) )
#define warn ( Console::instance()->colorErr("Warning", __FILE__, __LINE__, 33) )
#define error  ( Console::instance()->colorErr("Error", __FILE__, __LINE__, 31) )

#include <ostream>

class Console {
private:
	Console(void);
	~Console(void);

public:
	static Console * instance(void);

	std::ostream & colorMsg(const std::string & lbl, unsigned int color);
	std::ostream & colorErr(const std::string & lbl, const std::string & file,
		unsigned int line, unsigned int color);

private:
	static Console * myself;

};

#endif

