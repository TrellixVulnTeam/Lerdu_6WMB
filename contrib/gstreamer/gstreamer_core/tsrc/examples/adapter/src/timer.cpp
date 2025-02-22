/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:
*
*/
#include <e32std.h>
TTime NowTime;
TTime NowTime1;

extern "C"
{
void start_time()
{
	NowTime.HomeTime();
}

void end_time()
{
	NowTime1.HomeTime();
}

long time_diff()
{
	TTimeIntervalMicroSeconds micro_sec;
	TInt64 diff_int64;
	micro_sec = NowTime1.MicroSecondsFrom(NowTime);
	diff_int64 = micro_sec.Int64();
	return diff_int64;
}
}
