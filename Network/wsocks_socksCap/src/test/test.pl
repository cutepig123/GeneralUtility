#!/usr/bin/perl -w
#

use strict;
use warnings;
use IO::Socket::INET;

my $sock = IO::Socket::INET->new(
	PeerAddr	=> "192.168.2.66",
	PeerPort	=> 21,
	Proto		=> 'tcp') || die("IO::Socket::INET->new()");

while(<$sock>){
	print $_;
}

$sock->close();
