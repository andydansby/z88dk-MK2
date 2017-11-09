#!/usr/bin/perl

#     ZZZZZZZZZZZZZZZZZZZZ    8888888888888       00000000000
#   ZZZZZZZZZZZZZZZZZZZZ    88888888888888888    0000000000000
#                ZZZZZ      888           888  0000         0000
#              ZZZZZ        88888888888888888  0000         0000
#            ZZZZZ            8888888888888    0000         0000       AAAAAA         SSSSSSSSSSS   MMMM       MMMM
#          ZZZZZ            88888888888888888  0000         0000      AAAAAAAA      SSSS            MMMMMM   MMMMMM
#        ZZZZZ              8888         8888  0000         0000     AAAA  AAAA     SSSSSSSSSSS     MMMMMMMMMMMMMMM
#      ZZZZZ                8888         8888  0000         0000    AAAAAAAAAAAA      SSSSSSSSSSS   MMMM MMMMM MMMM
#    ZZZZZZZZZZZZZZZZZZZZZ  88888888888888888    0000000000000     AAAA      AAAA           SSSSS   MMMM       MMMM
#  ZZZZZZZZZZZZZZZZZZZZZ      8888888888888       00000000000     AAAA        AAAA  SSSSSSSSSSS     MMMM       MMMM
#
# Copyright (C) Paulo Custodio, 2011

# $Header: /cvsroot/z88dk/z88dk/src/z80asm/t/option-i-x.t,v 1.2 2011/08/05 20:23:53 pauloscustodio Exp $
# $Log: option-i-x.t,v $
# Revision 1.2  2011/08/05 20:23:53  pauloscustodio
# test case where .lib is not passed in -i, -x, e.g "-itest"
#
# Revision 1.1  2011/07/11 15:46:33  pauloscustodio
# Added test scripts for all z80asm options
#
#
# Test option -i, -x

use strict;
use warnings;
use File::Slurp;
use Test::More;
require 't/test_utils.pl';

my $lib = lib_file(); $lib =~ s/\.lib$/2.lib/i;
my $lib2 = $lib; $lib2 =~ s/\.lib$//i;

# -x with .lib
unlink_testfiles();
write_file(asm_file(), "xlib main \n main: ret");
t_z80asm_capture("-x".$lib." ".asm_file(), "", "", 0);
ok -f $lib;

# -i with .lib
t_z80asm_ok(0, "lib main \n call main", 
		"\xCD\x03\x00\xC9",
		"-i".$lib);

unlink_testfiles($lib, $lib2);

# -x without .lib
unlink_testfiles();
write_file(asm_file(), "xlib main \n main: ret");
t_z80asm_capture("-x".$lib2." ".asm_file(), "", "", 0);
ok -f $lib;

# -i without .lib
t_z80asm_ok(0, "lib main \n call main", 
		"\xCD\x03\x00\xC9",
		"-i".$lib2);

unlink_testfiles($lib, $lib2);

done_testing();
