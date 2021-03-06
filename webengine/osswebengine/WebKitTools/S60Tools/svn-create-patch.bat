@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S %0 %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
if errorlevel 1 goto script_failed_so_exit_with_non_zero_val 2>nul
goto endofperl
@rem ';
#!/usr/bin/perl -w
#line 15

# Copyright (C) 2005, 2006 Apple Computer, Inc.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1.  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer. 
# 2.  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution. 
# 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission. 
#
# THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Extended "svn diff" script for WebKit Open Source Project, used to make patches.

# Differences from standard "svn diff":
#
#   Uses the real diff, not svn's built-in diff.
#   Always passes "-p" to diff so it will try to include function names.
#   Handles binary files (encoded as a base64 chunk of text).
#   Sorts the diffs alphabetically by text files, then binary files.
#
# Missing features:
#
#   Handle moved files.

use strict;
use warnings;

use Config;
use Cwd;
use File::Basename;
use File::Spec;
use File::stat;
use Getopt::Long;
use MIME::Base64;
use POSIX qw(:errno_h);
use Time::gmtime;

sub canonicalizePath($);
sub generateDiff($);
sub generateFileList($\%\%);
sub numericcmp($$);
sub outputBinaryContent($);
sub pathcmp($$);
sub processPaths(\@);
sub splitpath($);

my $showHelp;
if (!GetOptions("help" => \$showHelp) || $showHelp) {
    print STDERR basename($0) . " [-h|--help] [svndir1 [svndir2 ...]]\n";
    exit 1;
}

my %paths = processPaths(@ARGV);

# Generate a list of files requiring diffs
my %textFiles;
my %binaryFiles;
for my $path (keys %paths) {
    generateFileList($path, %textFiles, %binaryFiles);
}

# Generate the diff for text files, then binary files, for easy reviewing
for my $file (sort pathcmp keys %textFiles) {
    generateDiff($file);
}
for my $file (sort pathcmp keys %binaryFiles) {
    generateDiff($file);
}

exit 0;


sub canonicalizePath($)
{
    my ($file) = @_;

    # Remove extra slashes and '.' directories in path
    $file = File::Spec->canonpath($file);

    # Remove '..' directories in path
    my @dirs = ();
    foreach my $dir (File::Spec->splitdir($file)) {
        if ($dir eq '..' && $#dirs >= 0 && $dirs[$#dirs] ne '..') {
            pop(@dirs);
        } else {
            push(@dirs, $dir);
        }
    }
    return ($#dirs >= 0) ? File::Spec->catdir(@dirs) : ".";
}

sub generateDiff($)
{
    my ($file) = @_;
    my $errors = "";
    my $isBinary;
    my $lastLine;
    open DIFF, "svn diff --diff-cmd diff -x -uNp $file |" or die;
    while (<DIFF>) {
        $isBinary = 1 if (/^Cannot display: file marked as a binary type\.$/);
        print;
        $lastLine = $_;
    }
    close DIFF;
    print "\n" if ($isBinary && $lastLine =~ m/\S+/);
    outputBinaryContent($file) if ($isBinary);
    print STDERR $errors;
}

sub generateFileList($\%\%)
{
    my ($path, $textFiles, $binaryFiles) = @_;
    my $indexPath;
    my $isBinary;
    open DIFF, "svn diff --diff-cmd diff -x -uNp $path |" or die;
    while (<DIFF>) {
        if (/^Index: (.*)/) {
            my $newIndexPath = $1;
            if ($indexPath) {
                if ($isBinary) {
                    $binaryFiles->{$indexPath} = 1;
                } else {
                    $textFiles->{$indexPath} = 1;
                }
            }
            $indexPath = $newIndexPath;
            $isBinary = 0;
        }
        if (/^Cannot display: file marked as a binary type\.$/) {
            $isBinary = 1;
        }
    }
    close DIFF;
    # Handle last patch
    if ($indexPath) {
        if ($isBinary) {
            $binaryFiles->{$indexPath} = 1;
        } else {
            $textFiles->{$indexPath} = 1;
        }
    }
    # check files for tabs
    foreach my $file ( keys %$textFiles ) {
        next if $file =~ /(\.mmp|\.mk)$/; # makefiles need them
        open (INPUT, $file) || die "can't open $file: $!";
        while (<INPUT>) {
           /\t/ and die "Error: $file contains tab characters, please remove.\n";
        }
        close(INPUT)    || die "can't close $file: $!";
    }
}

# Sort numeric parts of strings as numbers, other parts as strings.
# Makes 1.33 come after 1.3, which is cool.
sub numericcmp($$)
{
    my ($aa, $bb) = @_;

    my @a = split /(\d+)/, $aa;
    my @b = split /(\d+)/, $bb;

    # Compare one chunk at a time.
    # Each chunk is either all numeric digits, or all not numeric digits.
    while (@a && @b) {
        my $a = shift @a;
        my $b = shift @b;
        
        # Use numeric comparison if chunks are non-equal numbers.
        return $a <=> $b if $a =~ /^\d/ && $b =~ /^\d/ && $a != $b;

        # Use string comparison if chunks are any other kind of non-equal string.
        return $a cmp $b if $a ne $b;
    }
    
    # One of the two is now empty; compare lengths for result in this case.
    return @a <=> @b;
}

sub outputBinaryContent($)
{
    my ($path) = @_;
    # Deletion
    return if (! -e $path);
    # Addition or Modification
    my $buffer;
    open BINARY, $path  or die;
    while (read(BINARY, $buffer, 60*57)) {
        print encode_base64($buffer);
    }
    close BINARY;
    print "\n";
}

# Sort first by directory, then by file, so all paths in one directory are grouped
# rather than being interspersed with items from subdirectories.
# Use numericcmp to sort directory and filenames to make order logical.
sub pathcmp($$)
{
    my ($patha, $pathb) = @_;

    my ($dira, $namea) = splitpath($patha);
    my ($dirb, $nameb) = splitpath($pathb);

    return numericcmp($dira, $dirb) if $dira ne $dirb;
    return numericcmp($namea, $nameb);
}

sub processPaths(\@)
{
    my ($paths) = @_;
    return ("." => 1) if (!@{$paths});

    my %result = ();

    for my $file (@{$paths}) {
        die "can't handle absolute paths like \"$file\"\n" if File::Spec->file_name_is_absolute($file);
        die "can't handle empty string path\n" if $file eq "";
        die "can't handle path with single quote in the name like \"$file\"\n" if $file =~ /'/; # ' (keep Xcode syntax highlighting happy)

        my $untouchedFile = $file;

        $file = canonicalizePath($file);

        die "can't handle paths with .. like \"$untouchedFile\"\n" if $file =~ m|/\.\./|;

        $result{$file} = 1;
    }

    return ("." => 1) if ($result{"."});

    # Remove any paths that also have a parent listed.
    for my $path (keys %result) {
        for (my $parent = dirname($path); $parent ne '.'; $parent = dirname($parent)) {
            if ($result{$parent}) {
                delete $result{$path};
                last;
            }
        }
    }

    return %result;
}

# Break up a path into the directory (with slash) and base name.
sub splitpath($)
{
    my ($path) = @_;

    my $pathSeparator = "/";
    my $dirname = dirname($path) . $pathSeparator;
    $dirname = "" if $dirname eq "." . $pathSeparator;

    return ($dirname, basename($path));
}

__END__
:endofperl
