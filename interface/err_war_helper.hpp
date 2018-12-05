#pragma once

#define line	\
	"(" << __LINE__ << ")"

#define ERR(FUNC_NAME, WIDGET_NAME)			\
	FUNC_NAME << " - Error [" << WIDGET_NAME << "] " << line

#define WAR(FUNC_NAME, WIDGET_NAME)			\
	FUNC_NAME << " - Warning [" << WIDGET_NAME << "] " << line

#define PROG(FUNC_NAME, WIDGET_NAME, PROG)	\
	FUNC_NAME << " - [" << WIDGET_NAME << "] " << line << ": " << PROG
