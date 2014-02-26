#!/usr/bin/perl -w
#
# script for making our experiments
#

$sourceFileRuns = 3;
$outFileNameAvgN    = "testresults/data_avg_new";
$outFileNameMaxN    = "testresults/data_max_new";
$outFileNameTotN    = "testresults/data_tot_new";
$outFileNameUsrN    = "testresults/data_usr_new";

$outFileNameAvgO    = "testresults/data_avg_old";
$outFileNameMaxO    = "testresults/data_max_old";
$outFileNameTotO    = "testresults/data_tot_old";
$outFileNameUsrO    = "testresults/data_usr_old";


#space waste in intro + train
$outFileNameLiveO   = "testresults/data_live_old";
$outFileNameDeadO   = "testresults/data_dead_old";
$outFileNameUnusedO = "testresults/data_unused_old";

$outFileNameLiveN   = "testresults/data_live_new";
$outFileNameDeadN   = "testresults/data_dead_new";
$outFileNameUnusedN = "testresults/data_unused_new";

#space waste in train
$outFileNameLiveO_Tr   = "testresults/data_live_old_tr";
$outFileNameDeadO_Tr   = "testresults/data_dead_old_tr";
$outFileNameUnusedO_Tr = "testresults/data_unused_old_tr";

$outFileNameLiveN_Tr   = "testresults/data_live_new_tr";
$outFileNameDeadN_Tr   = "testresults/data_dead_new_tr";
$outFileNameUnusedN_Tr = "testresults/data_unused_new_tr";

#space waste in intro
$outFileNameLiveO_In   = "testresults/data_live_old_in";
$outFileNameDeadO_In   = "testresults/data_dead_old_in";
$outFileNameUnusedO_In = "testresults/data_unused_old_in";

$outFileNameLiveN_In   = "testresults/data_live_new_in";
$outFileNameDeadN_In   = "testresults/data_dead_new_in";
$outFileNameUnusedN_In = "testresults/data_unused_new_in";

#allocation stats
$outFileNameAllocBase   = "testresults/data_alloc_";
$outFileNameAllocLive   = $outFileNameAllocBase . "live";
$outFileNameAllocDead   = $outFileNameAllocBase . "dead";
$outFileNameAllocUnused = $outFileNameAllocBase . "unused";

@sourceFiles = ("src/nodist/beta-test-file/tst-norep3.gbc"
#		,"src/nodist/gbeta_programs/cruncher3.gbc"
		,"src/nodist/beta-test-file/tst-norep3BO.gbc"
		,"src/nodist/gbeta_programs/allocator.gbc"
		,"src/nodist/gbeta_programs/constAlloc.gbc"
		,"src/nodist/gbeta_programs/dfstree.gbc"
#      		,"src/nodist/gbeta_programs/derive.gbc"
		);

#12-20, 12-18

$introMin      = 12;
#$introMin      = 18;
#$introMax      = 12;
$introMax      = 20;

$carMin        = 12;
#$carMin        = 17;
$carMax        = 18;

#define the pairs of sizes used for generating 2D graphs
@introSpSzs = (12, 12, 15, 15, 15, 20, 20, 20);
@carSzs     = (12, 18, 12, 15, 18, 12, 15, 18);

@versions   = ("new", "old");

$execPath      = "src/vm";
$execName      = "gbvm";
$execWithPath  = "$execPath/$execName";
#$globalOptions = "DEBUG=n OPTIMIZE=0";
$globalOptions = "DEBUG=n OPTIMIZE=2";
#$globalOptions = "INTEGRITYCHECK=y OPTIMIZE=0";
$os            = `uname`;

$doExecBuild  = 0;
$doGatherData = 0;
$doExecTest   = 0;
$doSpeedCalib = 0;
$forReal      = 0;
$makeGraphs3D = 0;
$makeGraphs2D = 0;

#parse input options
foreach (@ARGV) {
    if($_ eq "build") {
	$doExecBuild = 1;
	next;
    } elsif ($_ eq "gatherdata") {
	$doGatherData = 1;
	next;
    } elsif ($_ eq "test") {
	$doExecTest = 1;
	next;
    } elsif ($_ eq "speedcalib") {
	$doSpeedCalib = 1;
	next;
    } elsif ($_ eq "forreal") {
	$forReal = 1;
	next;
    } elsif ($_ eq "graph") {
	$makeGraphs2D = $makeGraphs3D = 1;
	next;
    } elsif ($_ eq "graph2D") {
	$makeGraphs2D = 1;
	next;
    } elsif ($_ eq "graph3D") {
	$makeGraphs3D = 1;
	next;
    } elsif ($_ eq "noos") {
	$os = "Windows NT\n";
	next;
    } else {
	print "unrecognized option: $_\n";
	exit(0);
    }
}

print "\nOperating system is: $os\n";
if($os eq "SunOS\n") {
    $globalOptions = $globalOptions . " -j4 ";
}


#function to build executable
sub buildExecutable {
    my($options, $newExecName) = @_;
    if($forReal == 1) {
	print "cd src/gc_new; make clean\n" . `cd src/gc_new; make clean`;
	print "make $globalOptions $options \n";
	print `make $globalOptions $options`;
	print `strip $execWithPath`;
	print `mv $execWithPath bin/$newExecName`;
    } else {
	print "make $globalOptions $options\n";
	print "should now have build executable if forreal was specified\n";
    }
}


##### main #####


if($doExecBuild) {
#    if(0) {
    print "*** BUILDING DISRUPT STAT EXECS NEW METHOD ***\n";
    #executables for disruption time test
    for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
	    print `make clean`; #setVMReference() is inlined so we have to start over...
	    my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_dsrpt_new";
	    buildExecutable("DISRUPT_STAT=y NEW_STACK_RESCUE=0.8 " .
			    "INTROSP_EXP=$introExp CAR_EXP=$carExp SETVMREF=m", $newExecName);
	}
    }
    print "*** BUILDING DISRUPT STAT EXECS OLD METHOD ***\n";
    #executables for disruption time test
    for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
	    print `make clean`; #setVMReference() is inlined so we have to start over...
	    my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_dsrpt_old";
	    buildExecutable("DISRUPT_STAT=y INTROSP_EXP=$introExp " .
			    "CAR_EXP=$carExp SETVMREF=m", $newExecName);
	}
    }
#    } #if(0) 

    print "*** BUILDING SPACE WASTE EXECS OLD METHOD ***\n";
    print `make clean`;
    #executables for space waste test with old rescue method
    for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
	    my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_waste_old";
	    buildExecutable("LIVE=y INTROSP_EXP=$introExp CAR_EXP=$carExp", $newExecName);
	}
    }
    print "*** BUILDING SPACE WASTE EXECS NEW METHOD ***\n";
    print `make clean`;
    #executables for space waste test with new rescue method
    for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
	    my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_waste_new";
	    buildExecutable("LIVE=y NEW_STACK_RESCUE=0.8 INTROSP_EXP=$introExp " .
			    "CAR_EXP=$carExp", $newExecName);
	}
    }
}

if($doSpeedCalib) {
    #compile our calibration executable
    print `cd src/nodist; g++ -Wall -O2 printer.cc -o printer`;
    #run speed test
    print `src/nodist/printer &> testoutput/speedcalib`;
}

if($doExecTest || $doGatherData) {
    
    #read result of speed calibration
    open(SPEEDFILE, "<testoutput/speedcalib") or die "\n $0 Cannot open $! \n";

    while(!eof(SPEEDFILE)) {
	$myline = <SPEEDFILE>;
	if($myline =~ m/ONE_PRINTOUT_TAKES:/) {
	    my(@results) = split(/\s/, $myline); #match whitespace chars as dividers
	    $printout_speed = $results[1];
	}
    }
    print "Speed for one printout was: ${printout_speed}\n";
    close(SPEEDFILE);


#    if(0) {
    print "***** disrupt stat new *****\n";
    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
#    for($srcNum = 1; $srcNum < 2; ++$srcNum) { #only test the new source file
	if($doGatherData) {
	    open(OUTFILEAVG, ">$outFileNameAvgN" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEMAX, ">$outFileNameMaxN" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILETOT, ">$outFileNameTotN" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEUSR, ">$outFileNameUsrN" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	}
	for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	    for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
		my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_dsrpt_new";
		$avgtime = 0; $avgTotalTime = 0; $avgworsttime = 0; $avgUsrTime = 0;
		for($run = 0; $run < $sourceFileRuns; ++$run) {
		    my($fileName) = "testoutput/out_" . $newExecName . "_src_$srcNum" . "_run$run";
		    if($doExecTest) {
			my($cmdline) = "bash -c \"if (! \\time bin/$newExecName " .
			    "$sourceFiles[$srcNum] &> $fileName ); then echo " .
				"BAD_TERMINATION $introExp $carExp; fi \"";
			print `$cmdline`;
		    }
		    if($doGatherData) {
			open(MYFILE, $fileName) or die "\n $0 Cannot open $! \n";
			#print <myfile>;
			while (!eof(MYFILE)) {
			    $myline = <MYFILE>; 
			    #print $myline;
			    if($myline =~ m/gc\'s:/) {
				@results = split(/\s/, $myline); #match whitespace chars as dividers
				$avgtime      = (($avgtime      * $run) + $results[4] )/($run + 1);
				$avgworsttime = (($avgworsttime * $run) + $results[7] )/($run + 1);
				$avgTotalTime = (($avgTotalTime * $run) + $results[10])/($run + 1);
				$numGCs       = $results[1];
#			        print "av: $avgtime av_max: $avgworsttime av_tot: $avgTotalTime\n";
				#last; #no need to search more lines
			    }
			    if($os eq "SunOS\n") {
				if($myline =~ m/user/) {
				    @results = split(/\s/, $myline); #match whitespace chars as dividers
				    $usertime = $results[1];
				    $avgUsrTime = (($avgUsrTime * $run) + ($usertime - ($numGCs * $printout_speed)))/($run + 1);
				}
			    } else {
				if($myline =~ m/\d+.\d+user/) {
				    @results = split(/\s/, $myline); #match whitespace chars as dividers
				    $usertime = $results[0];
				    $usertime =~ s/user//;
				    $avgUsrTime = (($avgUsrTime * $run) + ($usertime - ($numGCs * $printout_speed)))/($run + 1);
				} 
			    }
			}
			close(MYFILE);
		    }
		}
		if($doGatherData) {
		    print OUTFILEAVG "$introExp $carExp $avgtime\n";
		    print OUTFILEMAX "$introExp $carExp $avgworsttime\n";
		    print OUTFILETOT "$introExp $carExp $avgTotalTime\n";
		    my($usrfrac) = $avgTotalTime / $avgUsrTime;
		    #print OUTFILEUSR "$introExp $carExp $avgUsrTime\n";
		    print OUTFILEUSR "$introExp $carExp $usrfrac\n";
		    #print "\n";
		    if($carExp == $carMax) {
			print OUTFILEAVG "\n";
			print OUTFILEMAX "\n";
			print OUTFILETOT "\n";
			print OUTFILEUSR "\n";
			#print "\n";
		    }
		}
	    }
	}
	if($doGatherData) {
	    close(OUTFILEAVG);
	    close(OUTFILEMAX);
	    close(OUTFILETOT);
	    close(OUTFILEUSR);
	}
    }
#    } #if(0)

#    if(0) {
    print "***** disrupt stat old *****\n";
    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
#    for($srcNum = 1; $srcNum < 2; ++$srcNum) { #only test source 1
	if($doGatherData) {
	    open(OUTFILEAVG, ">$outFileNameAvgO" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEMAX, ">$outFileNameMaxO" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILETOT, ">$outFileNameTotO" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEUSR, ">$outFileNameUsrO" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	}
	for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	    for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
		my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_dsrpt_old";
		$avgtime = 0; $avgTotalTime = 0; $avgworsttime = 0; $avgUsrTime = 0;
		for($run = 0; $run < $sourceFileRuns; ++$run) {
		    my($fileName) = "testoutput/out_" . $newExecName . "_src_$srcNum" . "_run$run";
		    if($doExecTest) {
			my($cmdline) = "bash -c \"if (! \\time bin/$newExecName $sourceFiles[$srcNum] " .
			    "&> $fileName ); then echo BAD_TERMINATION $introExp $carExp; fi \"";
			print `$cmdline`;
		    }
		    if($doGatherData) {
			open(MYFILE, $fileName) or die "\n $0 Cannot open $! \n";
			#print <myfile>;
			while (!eof(MYFILE)) {
			    $myline = <MYFILE>; 
			    #print $myline;
			    if($myline =~ m/gc\'s:/) {
				@results = split(/\s/, $myline); #match whitespace chars as dividers
				$avgtime      = (($avgtime      * $run) + $results[4] )/($run + 1);
				$avgworsttime = (($avgworsttime * $run) + $results[7] )/($run + 1);
				$avgTotalTime = (($avgTotalTime * $run) + $results[10])/($run + 1);
				$numGCs       = $results[1];
#			        print "av: $avgtime av_max: $avgworsttime av_tot: $avgTotalTime\n";
				#last; #no need to search more lines
			    }
			    if($os eq "SunOS\n") {
				if($myline =~ m/user/) {
				    @results = split(/\s/, $myline); #match whitespace chars as dividers
				    $usertime = $results[1];
				    $avgUsrTime = (($avgUsrTime * $run) + ($usertime - ($numGCs * $printout_speed)))/($run + 1);
				}
			    } else {
				if($myline =~ m/\d+.\d+user/) {
				    @results = split(/\s/, $myline); #match whitespace chars as dividers
				    $usertime = $results[0];
				    $usertime =~ s/user//;
				    $avgUsrTime = (($avgUsrTime * $run) + ($usertime - ($numGCs * $printout_speed)))/($run + 1);
#				    $avgUsrTime = (($avgUsrTime * $run) + $usertime   )/($run + 1);
				} 
			    }
			}
			close(MYFILE);
		    }
		}
		if($doGatherData) {
		    print OUTFILEAVG "$introExp $carExp $avgtime\n";
		    print OUTFILEMAX "$introExp $carExp $avgworsttime\n";
		    print OUTFILETOT "$introExp $carExp $avgTotalTime\n";
		    my($usrfrac) = $avgTotalTime / $avgUsrTime;
		    #print OUTFILEUSR "$introExp $carExp $avgUsrTime\n";
		    print OUTFILEUSR "$introExp $carExp $usrfrac\n";
		    #print "\n";
		    if($carExp == $carMax) {
			print OUTFILEAVG "\n";
			print OUTFILEMAX "\n";
			print OUTFILETOT "\n";
			print OUTFILEUSR "\n";
			#print "\n";
		    }
		}
	    }
	}
	if($doGatherData) {
	    close(OUTFILEAVG);
	    close(OUTFILEMAX);
	    close(OUTFILETOT);
	    close(OUTFILEUSR);
	}
    }
    #} #if(0)

    #if(0) { 
    print "***** space waste old *****\n";
    
    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
#    for($srcNum = 1; $srcNum < 2; ++$srcNum) {
	print "source $srcNum \n";
	if($doGatherData) {
	    open(OUTFILELIVE,   ">$outFileNameLiveO"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEDEAD,   ">$outFileNameDeadO"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEUNUSED, ">$outFileNameUnusedO" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";

	    open(OUTFILELIVE_TR,   ">$outFileNameLiveO_Tr"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEDEAD_TR,   ">$outFileNameDeadO_Tr"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEUNUSED_TR, ">$outFileNameUnusedO_Tr" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";

	    open(OUTFILELIVE_IN,   ">$outFileNameLiveO_In"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEDEAD_IN,   ">$outFileNameDeadO_In"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEUNUSED_IN, ">$outFileNameUnusedO_In" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	}
	for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	    for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
		my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_waste_old";
		my($statLine) = -2; #skip the first two stat lines
		my($fileName) = "testoutput/out_" . $newExecName . "_src_$srcNum";
		if($doExecTest) {
		    my($cmdline) = "bash -c \"if (! bin/$newExecName $sourceFiles[$srcNum] &> $fileName ); "
			. "then echo BAD_TERMINATION $introExp $carExp; fi\"";
		    print `$cmdline`;
		}
		if($doGatherData) {
		    $avgLive = 0; $avgDead = 0; $avgUnused = 0;
		    $avgLiveTr = 0; $avgDeadTr = 0; $avgUnusedTr = 0;
		    $avgLiveIn = 0; $avgDeadIn = 0; $avgUnusedIn = 0;
		    open(MYFILE, $fileName) or die "\n $0 Cannot open $! \n";
		    #print <myfile>;
		    while (!eof(MYFILE)) {
			$myline = <MYFILE>; 
			#print $myline;
			if($myline =~ m/MEMORY STAT/) {
			    if($statLine >= 0) { #excluding the first line, starts at -1
				@results = split(/\s/, $myline); #match whitespace chars as dividers
				my($live_in)   = $results[3];
				my($dead_in)   = $results[4];
				my($unused_in) = $results[5];
				my($total_in)  = $live_in + $dead_in + $unused_in;

				my($live_tr)   = $results[7];
				my($dead_tr)   = $results[8];
				my($unused_tr) = $results[9];
				my($total_tr)  = $live_tr + $dead_tr + $unused_tr;
				
				my($live)      = $live_in   + $live_tr;
				my($dead)      = $dead_in   + $dead_tr;
				my($unused)    = $unused_in + $unused_tr;
				my($total)     = $live + $dead + $unused;


				if($total_in != 0) {
				    $live_in   = $live_in/$total_in;
				    $dead_in   = $dead_in/$total_in;
				    $unused_in = $unused_in/$total_in;
				} else {
				    die "TOTAL NUMBER OF BYTES IN INTRO WAS 0!!!, INTERNAL ERROR!!!\n";
				}

				if($total_tr != 0) {
				    $live_tr   = $live_tr/$total_tr;
				    $dead_tr   = $dead_tr/$total_tr;
				    $unused_tr = $unused_tr/$total_tr;
				} else {
				    die "TOTAL NUMBER OF BYTES IN TRAIN WAS 0!!!, INTERNAL ERROR!!!\n";
				}

				if($total != 0) {
				    $live   = $live/$total;
				    $dead   = $dead/$total;
				    $unused = $unused/$total;
				} else {
				    die "TOTAL NUMBER OF BYTES IN TOTAL WAS 0!!!, INTERNAL ERROR!!!\n";
				}

				$avgLive      = (($avgLive   * $statLine) + $live  )/($statLine + 1);
				$avgDead      = (($avgDead   * $statLine) + $dead  )/($statLine + 1);
				$avgUnused    = (($avgUnused * $statLine) + $unused)/($statLine + 1);

				$avgLiveTr      = (($avgLiveTr   * $statLine) + $live_tr  )/($statLine + 1);
				$avgDeadTr      = (($avgDeadTr   * $statLine) + $dead_tr  )/($statLine + 1);
				$avgUnusedTr    = (($avgUnusedTr * $statLine) + $unused_tr)/($statLine + 1);

				$avgLiveIn      = (($avgLiveIn   * $statLine) + $live_in  )/($statLine + 1);
				$avgDeadIn      = (($avgDeadIn   * $statLine) + $dead_in  )/($statLine + 1);
				$avgUnusedIn    = (($avgUnusedIn * $statLine) + $unused_in)/($statLine + 1);
			    }
			    ++$statLine;
			}
		    }
		    close(MYFILE);
	    
		    print OUTFILELIVE      "$introExp $carExp $avgLive\n";
		    print OUTFILEDEAD      "$introExp $carExp $avgDead\n";
		    print OUTFILEUNUSED    "$introExp $carExp $avgUnused\n";

		    print OUTFILELIVE_TR   "$introExp $carExp $avgLiveTr\n";
		    print OUTFILEDEAD_TR   "$introExp $carExp $avgDeadTr\n";
		    print OUTFILEUNUSED_TR "$introExp $carExp $avgUnusedTr\n";

		    print OUTFILELIVE_IN   "$introExp $carExp $avgLiveIn\n";
		    print OUTFILEDEAD_IN   "$introExp $carExp $avgDeadIn\n";
		    print OUTFILEUNUSED_IN "$introExp $carExp $avgUnusedIn\n";


		    
		    if($carExp == $carMax) {
			print OUTFILELIVE "\n";    print OUTFILEDEAD "\n";    print OUTFILEUNUSED "\n";
			print OUTFILELIVE_TR "\n"; print OUTFILEDEAD_TR "\n"; print OUTFILEUNUSED_TR "\n";
			print OUTFILELIVE_IN "\n"; print OUTFILEDEAD_IN "\n"; print OUTFILEUNUSED_IN "\n";
#			print "\n";
		    }
		}
	    }
	}
	if($doGatherData) {
	    close(OUTFILELIVE);    close(OUTFILEDEAD);    close(OUTFILEUNUSED);
	    close(OUTFILELIVE_TR); close(OUTFILEDEAD_TR); close(OUTFILEUNUSED_TR);
	    close(OUTFILELIVE_IN); close(OUTFILEDEAD_IN); close(OUTFILEUNUSED_IN);
	}
    }

    print "***** space waste new *****\n";

    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
#    for($srcNum = 1; $srcNum < 2; ++$srcNum) {
	print "source $srcNum \n";
	if($doGatherData) {
	    open(OUTFILELIVE,   ">$outFileNameLiveN"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEDEAD,   ">$outFileNameDeadN"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEUNUSED, ">$outFileNameUnusedN" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";

	    open(OUTFILELIVE_TR,   ">$outFileNameLiveN_Tr"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEDEAD_TR,   ">$outFileNameDeadN_Tr"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEUNUSED_TR, ">$outFileNameUnusedN_Tr" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";

	    open(OUTFILELIVE_IN,   ">$outFileNameLiveN_In"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEDEAD_IN,   ">$outFileNameDeadN_In"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	    open(OUTFILEUNUSED_IN, ">$outFileNameUnusedN_In" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	}
	for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	    for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
		my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_waste_new";
		my($statLine) = -2;
		my($fileName) = "testoutput/out_" . $newExecName . "_src_$srcNum";
		if($doExecTest) {
		    my($cmdline) = "bash -c \"if (! bin/$newExecName $sourceFiles[$srcNum] &> $fileName ); "
			. "then echo BAD_TERMINATION $introExp $carExp; fi \"";
		    print `$cmdline`;
		}
		if($doGatherData) {
		    $avgLive = 0; $avgDead = 0; $avgUnused = 0;
		    $avgLiveTr = 0; $avgDeadTr = 0; $avgUnusedTr = 0;
		    $avgLiveIn = 0; $avgDeadIn = 0; $avgUnusedIn = 0;
		    open(MYFILE, $fileName) or die "\n $0 Cannot open $! \n";
		    #print <myfile>;
		    while (!eof(MYFILE)) {
			$myline = <MYFILE>; 
			#print $myline;
			if($myline =~ m/MEMORY STAT/) {
			    if($statLine >= 0) { #excluding the first line, starts at -1
				@results = split(/\s/, $myline); #match whitespace chars as dividers
				my($live_in)   = $results[3];
				my($dead_in)   = $results[4];
				my($unused_in) = $results[5];
				my($total_in)  = $live_in + $dead_in + $unused_in;

				my($live_tr)   = $results[7];
				my($dead_tr)   = $results[8];
				my($unused_tr) = $results[9];
				my($total_tr)  = $live_tr + $dead_tr + $unused_tr;
				
				my($live)      = $live_in   + $live_tr;
				my($dead)      = $dead_in   + $dead_tr;
				my($unused)    = $unused_in + $unused_tr;
				my($total)     = $live + $dead + $unused;


				if($total_in != 0) {
				    $live_in   = $live_in/$total_in;
				    $dead_in   = $dead_in/$total_in;
				    $unused_in = $unused_in/$total_in;
				} else {
				    die "TOTAL NUMBER OF BYTES IN INTRO WAS 0!!!, INTERNAL ERROR!!!\n";
				}

				if($total_tr != 0) {
				    $live_tr   = $live_tr/$total_tr;
				    $dead_tr   = $dead_tr/$total_tr;
				    $unused_tr = $unused_tr/$total_tr;
				} else {
				    die "TOTAL NUMBER OF BYTES IN TRAIN WAS 0!!!, INTERNAL ERROR!!!\n";
				}

				if($total != 0) {
				    $live   = $live/$total;
				    $dead   = $dead/$total;
				    $unused = $unused/$total;
				} else {
				    die "TOTAL NUMBER OF BYTES IN TOTAL WAS 0!!!, INTERNAL ERROR!!!\n";
				}

				$avgLive      = (($avgLive   * $statLine) + $live  )/($statLine + 1);
				$avgDead      = (($avgDead   * $statLine) + $dead  )/($statLine + 1);
				$avgUnused    = (($avgUnused * $statLine) + $unused)/($statLine + 1);

				$avgLiveTr      = (($avgLiveTr   * $statLine) + $live_tr  )/($statLine + 1);
				$avgDeadTr      = (($avgDeadTr   * $statLine) + $dead_tr  )/($statLine + 1);
				$avgUnusedTr    = (($avgUnusedTr * $statLine) + $unused_tr)/($statLine + 1);

				$avgLiveIn      = (($avgLiveIn   * $statLine) + $live_in  )/($statLine + 1);
				$avgDeadIn      = (($avgDeadIn   * $statLine) + $dead_in  )/($statLine + 1);
				$avgUnusedIn    = (($avgUnusedIn * $statLine) + $unused_in)/($statLine + 1);
			    }
			    ++$statLine;
			}
		    }
		    close(MYFILE);
	    
		    print OUTFILELIVE      "$introExp $carExp $avgLive\n";
		    print OUTFILEDEAD      "$introExp $carExp $avgDead\n";
		    print OUTFILEUNUSED    "$introExp $carExp $avgUnused\n";

		    print OUTFILELIVE_TR   "$introExp $carExp $avgLiveTr\n";
		    print OUTFILEDEAD_TR   "$introExp $carExp $avgDeadTr\n";
		    print OUTFILEUNUSED_TR "$introExp $carExp $avgUnusedTr\n";

		    print OUTFILELIVE_IN   "$introExp $carExp $avgLiveIn\n";
		    print OUTFILEDEAD_IN   "$introExp $carExp $avgDeadIn\n";
		    print OUTFILEUNUSED_IN "$introExp $carExp $avgUnusedIn\n";


		    
		    if($carExp == $carMax) {
			print OUTFILELIVE "\n";    print OUTFILEDEAD "\n";    print OUTFILEUNUSED "\n";
			print OUTFILELIVE_TR "\n"; print OUTFILEDEAD_TR "\n"; print OUTFILEUNUSED_TR "\n";
			print OUTFILELIVE_IN "\n"; print OUTFILEDEAD_IN "\n"; print OUTFILEUNUSED_IN "\n";
#			print "\n";
		    }
		}
	    }
	}
	if($doGatherData) {
	    close(OUTFILELIVE);    close(OUTFILEDEAD);    close(OUTFILEUNUSED);
	    close(OUTFILELIVE_TR); close(OUTFILEDEAD_TR); close(OUTFILEUNUSED_TR);
	    close(OUTFILELIVE_IN); close(OUTFILEDEAD_IN); close(OUTFILEUNUSED_IN);
	}
    }
#    } #if(0)


    if($doGatherData) {

	print "\n***** 2d livestats *****\n\n";

	for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
	    print "source $srcNum \n";    
	    for(my($i) = 0; $i < @versions; ++$i) {
		$version = $versions[$i];
		for($graph2DIndex = 0; $graph2DIndex < @introSpSzs; ++$graph2DIndex) {
		    $introExp = $introSpSzs[$graph2DIndex];
		    $carExp   =     $carSzs[$graph2DIndex];


		    #open output file live
		    open(OUTPUTFILE_L,   ">${outFileNameAllocLive}_${introExp}_${carExp}_${version}_src_$srcNum")
			or die "\n $0 Cannot open $! \n";
		    
		    #open output file dead
		    open(OUTPUTFILE_D,   ">${outFileNameAllocDead}_${introExp}_${carExp}_${version}_src_$srcNum")
			or die "\n $0 Cannot open $! \n";
		    
		    #open output file unused
		    open(OUTPUTFILE_U,   ">${outFileNameAllocUnused}_${introExp}_${carExp}_${version}_src_$srcNum")
			or die "\n $0 Cannot open $! \n";
		    
		    #open input file
		    my($fileName)  = "testoutput/out_${execName}_${introExp}_${carExp}_waste_${version}_src_$srcNum";
		    
		    open(INPUTFILE, "<$fileName") or die "\n $0 Cannot open $! \n";
		    
		    print OUTPUTFILE_L "0 0\n";
		    print OUTPUTFILE_D "0 0\n";
		    print OUTPUTFILE_U "0 0\n";
		    $gc_num = 0.5;
		    while (!eof(INPUTFILE)) {
			$myline = <INPUTFILE>;
			if($myline =~ m/MEMORY STAT/) {
			    @results = split(/\s/, $myline); #match whitespace chars as dividers
			    my($live)   = $results[3] + $results[7];
			    my($dead)   = $results[8];#$results[4] + 
				my($unused) = $results[9];
			    print OUTPUTFILE_L "$gc_num ${live}\n";
			    print OUTPUTFILE_D "$gc_num ${dead}\n";
			    print OUTPUTFILE_U "$gc_num ${unused}\n";
			    $gc_num += 0.5;
			}
		    }
		    
		    close(INPUTFILE);
		    close(OUTPUTFILE_L);
		    close(OUTPUTFILE_D);
		    close(OUTPUTFILE_U);
		}
	    }
	}
    }
}

if($makeGraphs3D) {
    print "Making 3D graphs\n";
    @datafiles = ($outFileNameAvgN, $outFileNameMaxN, $outFileNameTotN, $outFileNameUsrN,
		  $outFileNameAvgO, $outFileNameMaxO, $outFileNameTotO, $outFileNameUsrO,
		  $outFileNameLiveO, $outFileNameDeadO, $outFileNameUnusedO,
		  $outFileNameLiveN, $outFileNameDeadN, $outFileNameUnusedN,
		  $outFileNameLiveO_Tr, $outFileNameDeadO_Tr, $outFileNameUnusedO_Tr,
		  $outFileNameLiveN_Tr, $outFileNameDeadN_Tr, $outFileNameUnusedN_Tr,
		  $outFileNameLiveO_In, $outFileNameDeadO_In, $outFileNameUnusedO_In,
		  $outFileNameLiveN_In, $outFileNameDeadN_In, $outFileNameUnusedN_In,
);
    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
	my(@srcFilePathSplit) = split(/\//, $sourceFiles[$srcNum]);
	my($srcFileName) = (@srcFilePathSplit > 1)
	    ? $srcFilePathSplit[@srcFilePathSplit -1]
		: $srcFilePathSplit[0];
	
	foreach $datafile (@datafiles){
	    
	    
	    open(CMDFILE, ">gnuplot_cmds");
	    print CMDFILE "set nokey\n";
	    print CMDFILE "set xtics 1\n";
	    print CMDFILE "set ytics 1\n";
	    print CMDFILE "set ticslevel 0\n";
#	    print CMDFILE "set view 90,310\n"; #sideview
	    print CMDFILE "set view 45,326\n";
#	    print CMDFILE "set view 10,310\n"; #from above
	    print CMDFILE "set xlabel \"Introductory Space Size (2^x)/bytes\"\n";
	    print CMDFILE "set ylabel \"Car Size (2^x)/bytes\"\n";
	    print CMDFILE "set title \"Source: ${srcFileName}\"\n";# 0, -46\n";
	    
	    #a kind of a switch-case statement
	    for($datafile){
		(/$outFileNameAvgN/ or /$outFileNameAvgO/) and do {
		    print CMDFILE "set zlabel \"Avg Disruption Time/s\"\n";
		    print CMDFILE "set zrange [0:0.5]\n";
		    last;
		};
		(/$outFileNameMaxN/ or /$outFileNameMaxO/) and do {
		    print CMDFILE "set zlabel \"Avg max Disruption Time/s\"\n";
		    print CMDFILE "set zrange [0:0.5]\n";
		    last;
		};
		(/$outFileNameTotN/ or /$outFileNameTotO/) and do {
		    print CMDFILE "set zlabel \"Total Disruption Time/s\"\n";
		    print CMDFILE "set zrange [0:]\n";
		    if($srcNum == 0) {
			print CMDFILE "set zrange [0:1]\n";
		    } elsif($srcNum == 1) {
			print CMDFILE "set zrange [0:10]\n";
		    } elsif($srcNum == 2) {
			print CMDFILE "set zrange [0:1.2]\n";
		    } elsif($srcNum == 3) {
			print CMDFILE "set zrange [0:20]\n";
		    } elsif($srcNum == 4) {
			print CMDFILE "set zrange [0:10]\n";
		    }
		    last;
		};
		(/$outFileNameUsrN/ or /$outFileNameUsrO/) and do {
		    print CMDFILE "set zlabel \"GC Fraction of Tot. Time\"\n";
		    print CMDFILE "set zrange [0:1]\n";
		    last;
		};

		(/$outFileNameLiveO/    or /$outFileNameLiveN/    or
		 /$outFileNameLiveO_Tr/ or /$outFileNameLiveN_Tr/ or
		 /$outFileNameLiveO_In/ or /$outFileNameLiveN_In/) and do {
		     print CMDFILE "set zlabel \"Avg Live Fraction\"\n";
		     print CMDFILE "set zrange [0:1]\n";
		     last;
		 };
		(/$outFileNameDeadO/    or /$outFileNameDeadN/    or
		 /$outFileNameDeadO_Tr/ or /$outFileNameDeadN_Tr/ or
		 /$outFileNameDeadO_In/ or /$outFileNameDeadN_In/) and do {
		     print CMDFILE "set zlabel \"Avg Dead Fraction\"\n";
		     print CMDFILE "set zrange [0:1]\n";
		     last;
		 };
		(/$outFileNameUnusedO/    or /$outFileNameUnusedN/    or
		 /$outFileNameUnusedO_Tr/ or /$outFileNameUnusedN_Tr/ or
		 /$outFileNameUnusedO_In/ or /$outFileNameUnusedN_In/) and do {
		     print CMDFILE "set zlabel \"Avg Unused Fraction\"\n";
		     print CMDFILE "set zrange [0:1]\n";
		     last;
		 };
		die "Unknown datafile: " . $_;
	    }

	    print CMDFILE "set xrange [" . ($introMin-1) . ":" . ($introMax+1) . "]\n";
	    print CMDFILE "set yrange [" . ($carMin-1)   . ":" . ($carMax+1)   . "]\n";
	    print CMDFILE "set terminal fig\n";
	    #enhanced allows superscript, number is the fontsize
#           print CMDFILE "set terminal postscript eps enhanced 12\n";
	    print CMDFILE "set output \"$datafile" . "_src_$srcNum" . ".fig\"\n";
#	    print CMDFILE "set output \"$datafile" . "_src_$srcNum" . ".eps\"\n";
	    print CMDFILE "set data style impulses\n";
	    print CMDFILE "splot \"$datafile" . "_src_$srcNum" . "\"\n";
	    print CMDFILE "set data style lines\n";
	    print CMDFILE "splot \"$datafile" . "_src_$srcNum" . "\"\n";
	    print CMDFILE "exit\n";
	    close CMDFILE;
	    
	    print `gnuplot gnuplot_cmds`;
	    $fileNameBase = "$datafile" . "_src_$srcNum";
	    $figFileName = $fileNameBase . ".fig";
	    $epsFileName = $fileNameBase . ".eps";
	    $convertCmd = "fig2dev -L ps -m 2 -s 3 -f Helvetica "
		. "$figFileName $epsFileName";
	    print `$convertCmd`;
	    print `rm $figFileName`;

	}
    }
}

if($makeGraphs2D) {
    @datafiles2D = ($outFileNameAllocBase);
    print "Making 2D graphs\n";
    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
	my(@srcFilePathSplit) = split(/\//, $sourceFiles[$srcNum]);
	my($srcFileName) = $srcFilePathSplit[@srcFilePathSplit -1];
	for(my($i) = 0; $i < @versions; ++$i) {
	    $version = $versions[$i];
	    for($graph2DIndex = 0; $graph2DIndex < @introSpSzs; ++$graph2DIndex) {
		$introExp = $introSpSzs[$graph2DIndex];
		$carExp   =     $carSzs[$graph2DIndex];

		my($introspacesize) = (1<<$introExp)/1024; #introspace size in KB
		my($carsize)        = (1<<$carExp)  /1024; #car size in KB

		foreach $datafile (@datafiles2D){

		    open(CMDFILE, ">gnuplot_cmds");
#	    print CMDFILE "set nokey\n";
		    print CMDFILE "set yrange [0:]\n";
		    print CMDFILE "set ticslevel 0\n";
		    print CMDFILE "set xlabel \"Allocations of about ${introspacesize}KB\"\n";
		    print CMDFILE "set ylabel \"Bytes\"\n";
		    print CMDFILE "set terminal fig color big\n";
		    $fileNameBase = "$datafile${introExp}_${carExp}_${version}_src_${srcNum}";
		    $figFileName = $fileNameBase . ".fig";
		    $epsFileName = $fileNameBase . ".eps";
		    print CMDFILE "set title \"Source: ${srcFileName}, Introductory Space: ${introspacesize}KB, ".
			"Car: ${carsize}KB, Train Creation Policy: ${version} \"\n";# 0, -46\n";
		    print CMDFILE "set output \"${figFileName}\"\n";
		    print CMDFILE "set data style lines\n";
		    print CMDFILE "plot " .
			"\"${datafile}unused_${introExp}_${carExp}_${version}_src_${srcNum}\" title \"Unused Bytes\" with lines linetype 4, " .
			    "\"${datafile}dead_${introExp}_${carExp}_${version}_src_${srcNum}\" title \"Dead Bytes\" with lines linetype 5, " .
				"\"${datafile}live_${introExp}_${carExp}_${version}_src_${srcNum}\" title \"Live Bytes\" with lines linetype 2\n";
		    print CMDFILE "exit\n";
		    close CMDFILE;
		    
		    print `gnuplot gnuplot_cmds`;
		    $convertCmd = "fig2dev -L ps -m 2 -s 6 -f Helvetica "
			. "$figFileName $epsFileName";
		    print `$convertCmd`;
		    print `rm $figFileName`;
		    
		}
	    }
	}
    }
}
