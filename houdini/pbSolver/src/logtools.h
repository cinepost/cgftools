#ifndef __logtools_h__
#define __logtools_h__

#include <iostream>
#include <map>

static 	 int	logutils_indentation;

#define  LOG_INDENT_SIZE	2
#define  LOG_INDENT			logutils_indentation+=LOG_INDENT_SIZE
#define  LOG_UNDENT 		logutils_indentation-=LOG_INDENT_SIZE
#define	 LOG(X)				std::cout << std::string(logutils_indentation, '-') << X << std::endl;

#endif
