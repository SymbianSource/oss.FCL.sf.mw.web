/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

void
emitRange (long value,
           long& start,
           long& previous,
           long& increment)
{
  if (value != previous + increment) {
    if (previous == start + increment) {
      if (start != ~0) {
	printf ("  { 0x%08x, 0x%08x, 0x00 },\n", start, start);
        start = previous;
      } else {
        start = value;
      }
      increment = value - start;
    } else {
      printf ("  { 0x%08x, 0x%08x, 0x%02x },\n", start, previous, increment);
      start = value;
      increment = 0;
    }
  }
  previous = value;
}

int
main (int argc,
      char* argv[])
{
  long start = ~0;
  long previous = ~0;
  long increment = 0;

  while (!feof (stdin)) {
    char buffer[80];
    long value;
    char* endptr;

    (void) memset (buffer, 0, sizeof buffer);
    fgets (buffer, 80, stdin);
    value = strtol (buffer, &endptr, 16);
    if (endptr == buffer) {
      break;
    }

    if (increment == 0 && start != ~0) {
      increment = value - start;
    }

    emitRange (value, start, previous, increment);
  }

  do {
    emitRange (~0, start, previous, increment);
  } while (start != ~0);

  return EXIT_SUCCESS;
}
