#!/usr/bin/perl -w
@files=`git status -s` ;
@sync_files = ();

foreach (@files)
{
    #@files=split(/ /);
    $file = substr($_,3);
    #print $file;
    chomp($file);
    push(@sync_files, $file);
}

$cmd = "scp -r ";
foreach $file (@sync_files)
{
    #print $cmd."\n";
    $cmd = $cmd.$file." ";
    #print $file."\n";
}

$cmd = $cmd.' ops@10.1.11.34:/home/ops/';

print $cmd."\n";
system($cmd);

