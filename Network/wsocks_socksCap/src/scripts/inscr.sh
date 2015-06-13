#!/bin/bash
#
cr_dir=$(dirname $0)
cr_file="$cr_dir/copyright.txt"

cr_one()
{
	local tmp_file="/tmp/cr_one_$$"
	local cr_file=$1
	local orig_file=$2
	local line=$(wc -l $cr_file | cut -d " " -f 1)
	head -n $line $orig_file | diff - $cr_file > /dev/null && return
	cat $cr_file $orig_file > $tmp_file
	mv $tmp_file $orig_file
}

for i in $@; do
	cr_one $cr_file $i
done
