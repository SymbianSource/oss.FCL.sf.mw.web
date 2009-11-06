#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#
#!/usr/bin/perl

open INH,"<",$ARGV[0];
open OUTH,">",$ARGV[1];

foreach $line (<INH>){
  if($line =~ m/CACHEPOSTPONE:/)
  {
    print OUTH $line;
  }
}

