#include "stdafx.h"
#include "utility.h"

Utility::Utility(void)
{
}

Utility::~Utility(void)
{
}


QString Utility::MakeHALSafeName( QString s )
{
	s.replace( " ", "-" );
	return s;
}
