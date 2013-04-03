#!/usr/bin/perl

system("./p4exe < status_in.txt > out.txt");
system("diff status_out.txt out.txt");

system("./p4exe < show_in.txt > out.txt");
system("diff show_out.txt out.txt");

system("./p4exe < view_in.txt > out.txt");
system("diff view_out.txt out.txt");

system("./p4exe < errors_in.txt > out.txt");
system("diff errors_out.txt out.txt");
