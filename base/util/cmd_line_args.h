#ifndef BASE_UTIL_CMD_LINE_ARGS_H_
#define BASE_UTIL_CMD_LINE_ARGS_H_

#include "base/base_export.h"
#include <vector>
#include <ctype.h>
#include <tchar.h>

namespace nbase
{

class BASE_EXPORT CmdLineArgs: public std::vector<TCHAR*>
{
public:

	CmdLineArgs(const TCHAR*command_line = 0);
	virtual ~CmdLineArgs();

private:

	bool ParseCommandLine();

	TCHAR*buffer_;
};

} // namespace

#endif // BASE_UTIL_CMD_LINE_ARGS_H_
