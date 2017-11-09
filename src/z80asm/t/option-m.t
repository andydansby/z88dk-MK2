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

# $Header: /cvsroot/z88dk/z88dk/src/z80asm/t/option-m.t,v 1.1 2011/07/11 15:46:33 pauloscustodio Exp $
# $Log: option-m.t,v $
# Revision 1.1  2011/07/11 15:46:33  pauloscustodio
# Added test scripts for all z80asm options
#
#
# Test option -m, -nm

use strict;
use warnings;
use File::Slurp;
use Test::More;
require 't/test_utils.pl';

my $asm = "xdef main \n main: ld b,10 \n loop: djnz loop";

# -m
unlink_testfiles();
write_file(asm_file(), $asm);

t_z80asm_capture("-r0 -b -m ".asm_file(), "", "", 0);
ok -f obj_file();
ok -f bin_file();
ok -f map_file();
is read_file(bin_file(), binmode => ':raw'), "\x06\x0A\x10\xFE";

like read_file(map_file()),
	qr/ \A 
	    LOOP \s+ = \s+ 0002, \s+ L: \s+ TEST \s+
	    MAIN \s+ = \s+ 0000, \s+ G: \s+ TEST \s+
	    
	    MAIN \s+ = \s+ 0000, \s+ G: \s+ TEST \s+
	    LOOP \s+ = \s+ 0002, \s+ L: \s+ TEST \s+
	    \z
	/x;

# -nb
unlink_testfiles();
write_file(asm_file(), $asm);

t_z80asm_capture("-r0 -b -nm ".asm_file(), "", "", 0);
ok -f obj_file();
ok ! -f map_file();
ok -f bin_file();
is read_file(bin_file(), binmode => ':raw'), "\x06\x0A\x10\xFE";

unlink_testfiles();
done_testing();
