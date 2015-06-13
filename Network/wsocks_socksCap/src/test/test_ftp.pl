#!/usr/bin/perl -w
#
use strict;
use warnings;
use Net::FTP;

my $ftp = Net::FTP->new(
	Host	=> "192.168.2.66",
	Port	=> 21,
	Passive	=> 1) or die("Net::FTP->new()");

$ftp->login("ftp", "")
	or die("login()");

my @list = $ftp->ls("/")
	or die("ls()");
foreach my $item (@list){
	print $item . "\n";
}

for(my $i = 0; $i <= $#list; $i ++){
	print $list[$i] . "\n";
}

my $list2 = $ftp->ls("/")
	or die("ls()");
foreach my $item (@$list2){
	print $item . "\n";
}

$ftp->quit()
	or die("quit()");
