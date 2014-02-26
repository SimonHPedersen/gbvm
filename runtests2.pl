#!/usr/bin/perl -w
#
# script for making our experiments
#

$sourceFileRuns = 3;
$outFileNameAvg    = "testresults/data_avg";
$outFileNameMax    = "testresults/data_max";
$outFileNameTot    = "testresults/data_tot";

$outFileNameLiveO   = "testresults/data_live_old";
$outFileNameDeadO   = "testresults/data_dead_old";
$outFileNameUnusedO = "testresults/data_unused_old";

$outFileNameLiveN   = "testresults/data_live_new";
$outFileNameDeadN   = "testresults/data_dead_new";
$outFileNameUnusedN = "testresults/data_unused_new";

@sourceFiles = ("src/nodist/beta-test-file/tst-norep3.gbc",
		"src/nodist/gbeta_programs/cruncher3.gbc",
		"src/nodist/gbeta_programs/allocator.gbc",
		"src/nodist/gbeta_programs/constAlloc.gbc",
		"src/nodist/gbeta_programs/dfstree.gbc",
		"src/nodist/gbeta_programs/derive.gbc",
		"src/nodist/gbeta_programs/simple.gbc"
		);

@gbetaBenchmarks_java = (#"arithmetic",
			 "assignment",
			 "objRefAndIns",
			 "object",
			 "runtimePath0",
			 "runtimePath1",
			 "runtimePath2",
			 "runtimePath3",
			 "runtimePath4",
			 "runtimePath5",
			 "virtual",
			 "simple",
			 );

@gbetaBenchmarks_gbc = (#"gbetai-benchmarks/arithmetic.gbc",
			 "gbetai-benchmarks/assignment.gbc",
			 "gbetai-benchmarks/objRefAndIns.gbc",
			 "gbetai-benchmarks/object.gbc",
			 "gbetai-benchmarks/runtimePath0.gbc",
			 "gbetai-benchmarks/runtimePath1.gbc",
			 "gbetai-benchmarks/runtimePath2.gbc",
			 "gbetai-benchmarks/runtimePath3.gbc",
			 "gbetai-benchmarks/runtimePath4.gbc",
			 "gbetai-benchmarks/runtimePath5.gbc",
			 "gbetai-benchmarks/virtual.gbc",
			 "src/nodist/gbeta_programs/simple.gbc",
			 );

@gbetaBenchmarks_gb = (#"gbetai-benchmarks/arithmetic.gb",
			 "gbetai-benchmarks/assignment.gb",
			 "gbetai-benchmarks/objRefAndIns.gb",
			 "gbetai-benchmarks/object.gb",
			 "gbetai-benchmarks/runtimePath0.gb",
			 "gbetai-benchmarks/runtimePath1.gb",
			 "gbetai-benchmarks/runtimePath2.gb",
			 "gbetai-benchmarks/runtimePath3.gb",
			 "gbetai-benchmarks/runtimePath4.gb",
			 "gbetai-benchmarks/runtimePath5.gb",
			 "gbetai-benchmarks/virtual.gb",
			 "src/nodist/gbeta_programs/simple.gb",
			 );


#12-20, 12-18

$introMin      = 12;
#$introMin      = 18;
#$introMax      = 12;
$introMax      = 20;

$carMin        = 12;
#$carMin        = 17;
$carMax        = 18;

$execPath      = "src/vm";
$execName      = "gbvm";
$execWithPath  = "$execPath/$execName";
#$globalOptions = "DEBUG=n OPTIMIZE=0";
$globalOptions = "DEBUG=n OPTIMIZE=9";
#$globalOptions = "INTEGRITYCHECK=y OPTIMIZE=0";
$os            = `uname`;
print "\nOperating system is: $os\n";
if($os eq "SunOS\n") {
    $globalOptions = $globalOptions . " -j4 ";
}

#SET THESE BEFORE BUILDING
$is_size = 16;
$cs_size = 15;

$doProfileBuild = 0;
$doExecBuild = 0;
$doWBBuild   = 0;
$doSpeedBuild = 0;
$doExecTest  = 0;
$doWBExecTest  = 0;
$doWBparse = 0;
$doSpeedparse = 0;
$doLangExecTest = 0;
$doLangparse = 0;
$langIterations = 100;
$wbIterations = 100;
$speedIterations = 100;
$speedIterationsGbeta = 5;
$doSpeedExecTest = 0;
$doSpeedExecTestJava = 0;
$doSpeedExecTestJavaGbvm = 0;
$forReal     = 0;
$makeGraphs  = 0;


#parse input options
foreach (@ARGV) {
    if($_ eq "build") {
	$doExecBuild = 1;
	next;
    } elsif ($_ eq "wbbuild") {
	$doWBBuild = 1;
	next;
    } elsif ($_ eq "wbtest") {
	$doWBExecTest = 1;
	next;
    } elsif ($_ eq "wbparse") {
	$doWBparse = 1;
	next;
    } elsif ($_ eq "speedbuild") {
	$doSpeedBuild = 1;
	next;
    } elsif ($_ eq "speedtest") {
	$doSpeedExecTest = 1;
	next;
    } elsif ($_ eq "speedtestjava") {
	$doSpeedExecTestJava = 1;
	next;
    } elsif ($_ eq "speedtestjavagbvm") {
	$doSpeedExecTestJavaGbvm = 1;
	next;
    } elsif ($_ eq "speedtestjavagbeta") {
	$doSpeedExecTestJavaGbeta = 1;
	next;
    } elsif ($_ eq "speedtestorai") {
	$doSpeedExecTestObjRefAndIns = 1;
	next;
    } elsif ($_ eq "speedparse") {
	$doSpeedparse = 1;
	next;
    } elsif ($_ eq "speedparsejava") {
	$doSpeedparseJava = 1;
	next;
    } elsif ($_ eq "speedparsegbeta") {
	$doSpeedparseJavaGbeta = 1;
	next;
    } elsif ($_ eq "langtest") {
	$doLangExecTest = 1;
	next;
    } elsif ($_ eq "langparse") {
	$doLangparse = 1;
	next;
    } elsif ($_ eq "profilebuild") {
	$doProfileBuild = 1;
	next;
    } elsif ($_ eq "test") {
	$doExecTest = 1;
	next;
    } elsif ($_ eq "forreal") {
	$forReal = 1;
	next;
    } elsif ($_ eq "graph") {
	$makeGraphs = 1;
	next;
    } else {
	print "unrecognized option: $_\n";
	exit(0);
    }
}

#function to build executable
sub buildExecutable {
    my($options, $newExecName) = @_;
    if($forReal == 1) {
	print "cd src/gc_new; make clean\n" . `cd src/gc_new; make clean`;
	print "make $globalOptions $options \n";
	print `make $globalOptions $options`;
	#print `strip $execWithPath`;
	print `mv $execWithPath bin/$newExecName`;
    } else {
	print "make $globalOptions $options\n";
	print "should now have build executable if forreal was specified\n";
    }
}

#sub runTest {
#    my($executable) = @_;
#    #print "executing test...\n";
#    foreach (@sourceFiles) {
#	my($cmdline) = "bash -c \"\\time bin/$executable $_ &> testoutput/out_$executable\"\n";
#	if($forReal == 1) {
#	    print `$cmdline`;
#	} else {
#	    print $cmdline;
#	}
#    }
#}

##### main #####



if($doExecBuild) {
    print "*** BUILDING DISRUPT STAT EXECS ***\n";
    #executables for disruption time test
    for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
	    print `make clean`; #setVMReference() is inlined so we have to start over...
	    my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_dsrpt";
	    buildExecutable("DISRUPT_STAT=y NEW_STACK_RESCUE=0.8 INTROSP_EXP=$introExp CAR_EXP=$carExp SETVMREF=m", $newExecName);
	}
    }
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
	    buildExecutable("LIVE=y NEW_STACK_RESCUE=0.8 INTROSP_EXP=$introExp CAR_EXP=$carExp", $newExecName);
	}
    }
}

# HEMPE START

if($doSpeedBuild == 1){
#    print `make clean`;
#    buildExecutable("NEW_STACK_RESCUE=0.8 INTROSP_EXP=$is_size CAR_EXP=$cs_size SETVMREF=m GC=n", gbvm_speed_nogc);
    print `make clean`;
    buildExecutable("NEW_STACK_RESCUE=0.8 INTROSP_EXP=$is_size CAR_EXP=$cs_size SETVMREF=m", gbvm_speed);    
}

if($doProfileBuild == 1){
    print `make clean`;
    buildExecutable("PROFILE=y NEW_STACK_RESCUE=0.8 INTROSP_EXP=$is_size CAR_EXP=$cs_size SETVMREF=m", gbvm_profile);
}


if($doSpeedExecTest) {
#    print `bash -c \"cd /pack/gbetai\"`;
#    print `java -cp "/pack/gbetai" /pack/gbetai/gvm`;
#    my($pwd) = `bash -c \"pwd\"`;
#    print $pwd;
#    my($cmdpwd) = "cd ". $pwd;
    my($gbFile) = "";
    if($forReal == 1){
	for(my($j)=0; $j<$speedIterations; $j++){
		my($i) = 0;
                foreach(@sourceFiles){
		$gbFile = $_;
		$gbFile =~ s/gbc/gb/;
		print "********" . $gbFile . "\n";
		if($os eq "SunOS\n") {
		    print "gbvm !With GC! " . "$_" . "\n";
		    print `bash -c \"if ! /user/hempe/f10/bin/timexx bin/gbvm_speed $_ &> testoutput/speed_gbvm_gc$i$j; then echo Gik ned; fi\"`;
		    print "gbvm !Without GC! " . "$_" . "\n";
		    print `bash -c \"if ! /user/hempe/f10/bin/timexx bin/gbvm_speed_nogc $_ &> testoutput/speed_gbvm_nogc$i$j; then echo Gik ned; fi\"`;
		    print "gbeta -r -l " . "$_" . "\n";
		    print `bash -c \"/user/hempe/f10/bin/timexx gbeta -r -l $gbFile &> testoutput/speed_gbeta$i$j\"`;
		    if($_ =~ /simple.gbc/){
			print "java gvm -c -os (speed optimized:))" . "$_" . "\n";
			print `cd /user/ricki/projekt/gbetai/;/user/hempe/f10/bin/timexx java gvm -c -os /user/hempe/gbvm/$_ &> testoutput/speed_igbeta$i$j`;	    
		    }
		} else {
		    print "gbvm !With GC! " . "$_" . "\n";
		    print `bash -c \"if (! \\time bin/gbvm_speed $_ &> testoutput/speed_gbvm_gc$i$j); then echo Gik ned; fi\"`;
		    print "gbvm !Without GC! " . "$_" . "\n";
		    print `bash -c \"if (! \\time bin/gbvm_speed_nogc $_ &> /home/hempe/gbvm/testoutput/speed_gbvm_nogc$i$j); then echo Gik ned; fi\"`;
		    print "gbeta -r -l " . "$_" . "\n";
		    print `\\time gbeta -r -l $gbFile &> testoutput/speed_gbeta$i$j`;
		    if($_ =~ /simple.gbc/){
			print "cd /pack/gbetai`;java gvm -c -os (speed optimized:)) " . "$_" . "\n";
			print `cd /pack/gbetai;\\time java gvm -c -os /home/hempe/gbvm/$_ &> /home/hempe/gbvm/testoutput/speed_igbeta$i$j`;
		    }
		}
		$i++;
	    }
	}
    }
}

if($doSpeedExecTestJava) {
    if($forReal == 1){
	my($i) = 0;	
	foreach(@gbetaBenchmarks_java){
	    for(my($j)=0; $j<$speedIterations; $j++){
		if($os eq "SunOS\n") {
		    print "not implemented \n";
#		print "java gvm -c -os (speed optimized:))" . "$_" . "\n";
#			print `cd /user/hempe/f10/bin/timexx java gvm -c -os /user/hempe/gbvm/$_ &> testoutput/speed_igbeta$i$j`;	    
		} else {
		    print "cd /pack/gbetai;\\time java $_ &> /home/hempe/gbvm/testoutput/bm_speed_igbeta$i$j\n";	
		    print `cd /pack/gbetai;\\time java $_ &> /home/hempe/gbvm/testoutput/bm_speed_igbeta$i$j`;
		}
	    }
	    $i++;
	}
    }
}

if($doSpeedExecTestJava || $doSpeedExecTestJavaGbvm) {
    if($forReal == 1){
	my($i) = 0;	
	foreach(@gbetaBenchmarks_gbc){
	    for(my($j)=0; $j<$speedIterations; $j++){
	    if($os eq "SunOS\n") {
	    print "not implemented \n";
#		print "java gvm -c -os (speed optimized:))" . "$_" . "\n";
#			print `cd /user/hempe/f10/bin/timexx java gvm -c -os /user/hempe/gbvm/$_ &> testoutput/speed_igbeta$i$j`;	    
		} else {
		    print "\\time bin/gbvm_speed $_ &> /home/hempe/gbvm/testoutput/bm_speed_gbvm$i$j\n";
		    print `\\time bin/gbvm_speed $_ &> /home/hempe/gbvm/testoutput/bm_speed_gbvm$i$j`;
		}
	    }
	    $i++;
	}
    }
}

if($doSpeedExecTestObjRefAndIns) {
    if($forReal == 1){
	my($i) = 1;
	for(my($j)=0; $j<$speedIterations; $j++){
	    if($os eq "SunOS\n") {
		print "not implemented \n";
	    } else {
		print "\\time bin/gbvm_speed gbetai-benchmarks/objRefAndIns.gbc &> /home/hempe/gbvm/testoutput/bm_speed_gbvm$i$j\n";
		print `\\time bin/gbvm_speed gbetai-benchmarks/objRefAndIns.gbc &> /home/hempe/gbvm/testoutput/bm_speed_gbvm$i$j`;
	    }
	}
    }
}


if($doSpeedExecTestJava || $doSpeedExecTestJavaGbeta) {
    if($forReal == 1){
	my($i) = 0;	
	foreach(@gbetaBenchmarks_gb){
	    for(my($j)=0; $j<$speedIterationsGbeta; $j++){
		if($os eq "SunOS\n") {
		    print "not implemented \n";
#		print "java gvm -c -os (speed optimized:))" . "$_" . "\n";
#			print `cd /user/hempe/f10/bin/timexx java gvm -c -os /user/hempe/gbvm/$_ &> testoutput/speed_igbeta$i$j`;	    
		} else {
		    print "\\time gbeta -r -l $_ &> testoutput/bm_speed_gbeta$i$j\n";
		    print `\\time gbeta -r -l $_ &> testoutput/bm_speed_gbeta$i$j`;
		}
	    }
	    $i++;
	}
    }
}

if($doSpeedparseJava) {
    print "parsing speed comparison results\n";
    my(@avgusertime_gbvm_gc);
    my(@avgusertime_gbvm_nogc);
    my(@avgusertime_gbeta);
    my(@avgusertime_gbetai);
    for(my($i) = 0; $i<@gbetaBenchmarks_gbc; $i++){    #each source file
	my($usertime_avg_gbvm_gc) = 0;
	my($usertime_avg_igbeta) = 0;	
	for(my($j) = 0; $j<$speedIterations; $j++){ #tried wbItereations nof times
#	    print "Parsing: " . "testoutput/speed_gbvm_gc".$j.$i . "\n";
	    open(MYFILE, "testoutput/bm_speed_gbvm".$i.$j) or die "\n $0 Cannot open testoutput/speed_gbvm_gc$i$j $! \n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		    if($myline =~ m/user/) {
			@results = split(/\s+/, $myline); #match whitespace chars as dividers
			$usertime = $results[1];			    
			if($usertime =~ m/:/){
			    @subresults = split(/:/, $usertime);
			    $usertime = 60*$subresults[0] + $subresults[1];
#			    print $usertime . "\n";
			}
			$usertime_avg_gbvm_gc = $usertime_avg_gbvm_gc + $usertime;
#			print "usertime_avg_gbvm_gc:" . $usertime_avg_gbvm_gc . "\n";
			last; #no need to search more lines
		    }
		} else {
		    if($myline =~ m/\d+.\d+user/) {
			@results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[0];
			$usertime =~ s/user//;
			$usertime_avg_gbvm_gc = $usertime_avg_gbvm_gc + $usertime;
#			print "u: " . $usertime . " ";
#			print "usertime_avg_gbvm_gc:" . $usertime_avg_gbvm_gc . "\n";
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search more lines
		    } 
		}
	    }
	    close(MYFILE);
#	    print "Parsing: " . "testoutput/speed_gbvm_nogc".$j.$i . "\n";
	    open(MYFILE, "testoutput/bm_speed_igbeta".$i.$j) or die "\n $0 Cannot open testoutput/speed_gbvm_nogc$i.$j $! \n";
#	    print "Parsing: " ."testoutput/WBresult".$j.$i . "\n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		    if($myline =~ m/user/) {
			@results = split(/\s+/, $myline); #match whitespace chars as dividers
			$usertime = $results[1];
			if($usertime =~ m/:/){
			    @subresults = split(/:/, $usertime);
			    $usertime = 60*$subresults[0] + $subresults[1];
			    print $usertime . "\n";
			}
			$usertime_avg_igbeta = $usertime_avg_igbeta + $usertime;
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search more lines
		    }
		} else {
		    if($myline =~ m/\d+.\d+user/) {
			@results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[0];
			$usertime =~ s/user//;
			$usertime_avg_igbeta = $usertime_avg_igbeta + $usertime;
#			print "usertime_avg_gbvm_nogc:" . $usertime_avg_gbvm_nogc . "\n";
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search more lines
		    } 
		}
	    }
	    close(MYFILE);
	}
#	$avgusertime_gbvm_nogc[$i] = $usertime_avg_gbvm_nogc / $speedIterations;
#	print $avgusertime_gbvm_nogc[$i] . "\n"; #"avgusertime_wb[$i]" . 
	$avgusertime_gbvm_gc[$i] = $usertime_avg_gbvm_gc / $speedIterations;
	print $avgusertime_gbvm_gc[$i] . "\n"; #"avgusertime_wb[$i]" . 
#	$avgusertime_gbeta[$i] = $usertime_avg_gbeta / $speedIterations;
#	print $avgusertime_gbeta[$i] . "\n"; #"avgusertime_wb[$i]" . 
	$avgusertime_igbeta[$i] = $usertime_avg_igbeta / $speedIterations;
	print $avgusertime_igbeta[$i] . "\n"; #"avgusertime_wb[$i]" . 
	print "\n";
    }
}

if($doSpeedparseJavaGbeta) {
    print "parsing speed comparison results\n";
    my(@avgusertime_gbeta);
    for(my($i) = 0; $i<@gbetaBenchmarks_gb; $i++){    #each source file
	my($usertime_avg_gbeta) = 0;
	for(my($j) = 0; $j<$speedIterationsGbeta; $j++){ #tried wbItereations nof times
	    open(MYFILE, "testoutput/bm_speed_gbeta".$i.$j) or die "\n $0 Cannot open testoutput/speed_gbvm_gc$i$j $! \n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		       print "not implemented (yet) \n";
		       last; #no need to search more lines
		} else {
		  if($myline =~ m/Measured/) {
		        @results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[4] / 100;
			$usertime_avg_gbeta = $usertime_avg_gbeta + $usertime;
		        last; #no need to search more lines 
		    }
		}
	    }
	    close(MYFILE);
	}
#	$avgusertime_gbvm_nogc[$i] = $usertime_avg_gbvm_nogc / $speedIterations;
#	print $avgusertime_gbvm_nogc[$i] . "\n"; #"avgusertime_wb[$i]" . 
	$avgusertime_gbeta[$i] = $usertime_avg_gbeta / $speedIterationsGbeta;
	print $avgusertime_gbeta[$i] . "\n"; #"avgusertime_wb[$i]" . 
#	$avgusertime_gbeta[$i] = $usertime_avg_gbeta / $speedIterations;
#	print $avgusertime_gbeta[$i] . "\n"; #"avgusertime_wb[$i]" . 
	print "\n";
    }
}

if($doSpeedparse) {
    print "parsing speed comparison results\n";
    my(@avgusertime_gbvm_gc);
    my(@avgusertime_gbvm_nogc);
    my(@avgusertime_gbeta);
    my(@avgusertime_gbetai);
    for(my($i) = 0; $i<@sourceFiles; $i++){    #each source file
	my($usertime_avg_gbvm_gc) = 0;
	my($usertime_avg_gbvm_nogc) = 0;
	my($usertime_avg_gbeta) = 0;
	my($usertime_avg_igbeta) = 0;	
	for(my($j) = 0; $j<$speedIterations; $j++){ #tried wbItereations nof times
#	    print "Parsing: " . "testoutput/speed_gbvm_gc".$j.$i . "\n";
	    open(MYFILE, "testoutput/speed_gbvm_gc".$i.$j) or die "\n $0 Cannot open testoutput/speed_gbvm_gc$i.$j $! \n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		    if($myline =~ m/user/) {
			@results = split(/\s+/, $myline); #match whitespace chars as dividers
			$usertime = $results[1];			    
			if($usertime =~ m/:/){
			    @subresults = split(/:/, $usertime);
			    $usertime = 60*$subresults[0] + $subresults[1];
#			    print $usertime . "\n";
			}
			$usertime_avg_gbvm_gc = $usertime_avg_gbvm_gc + $usertime;
#			print "usertime_avg_gbvm_gc:" . $usertime_avg_gbvm_gc . "\n";
			last; #no need to search more lines
		    }
		} else {
		    if($myline =~ m/\d+.\d+user/) {
			@results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[0];
			$usertime =~ s/user//;
			$usertime_avg_gbvm_gc = $usertime_avg_gbvm_gc + $usertime;
#			print "u: " . $usertime . " ";
#			print "usertime_avg_gbvm_gc:" . $usertime_avg_gbvm_gc . "\n";
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search more lines
		    } 
		}
	    }
	    close(MYFILE);
#	    print "Parsing: " . "testoutput/speed_gbvm_nogc".$j.$i . "\n";
	    open(MYFILE, "testoutput/speed_gbvm_nogc".$i.$j) or die "\n $0 Cannot open testoutput/speed_gbvm_nogc$i.$j $! \n";
#	    print "Parsing: " ."testoutput/WBresult".$j.$i . "\n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		    if($myline =~ m/user/) {
			@results = split(/\s+/, $myline); #match whitespace chars as dividers
			$usertime = $results[1];
			if($usertime =~ m/:/){
			    @subresults = split(/:/, $usertime);
			    $usertime = 60*$subresults[0] + $subresults[1];
			    print $usertime . "\n";
			}
			$usertime_avg_gbvm_nogc = $usertime_avg_gbvm_nogc + $usertime;
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search more lines
		    }
		} else {
		    if($myline =~ m/\d+.\d+user/) {
			@results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[0];
			$usertime =~ s/user//;
			$usertime_avg_gbvm_nogc = $usertime_avg_gbvm_nogc + $usertime;
#			print "usertime_avg_gbvm_nogc:" . $usertime_avg_gbvm_nogc . "\n";
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search more lines
		    } 
		}
	    }
	    close(MYFILE);
#	    print "Parsing: " . "testoutput/speed_gbeta".$j.$i . "\n";
	    open(MYFILE, "testoutput/speed_gbeta".$i.$j) or die "\n $0 Cannot open $! \n";
#	    print "Parsing: " ."testoutput/WBresult".$j.$i . "\n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		    if($myline =~ m/Measured/) {
			@results = split(/\s+/, $myline); #match whitespace chars as dividers
			$usertime = $results[4] / 100;			    
#			if($usertime =~ m/:/){
#			    @subresults = split(/:/, $usertime);
#			    $usertime =~ 60*$subresults[0] + $subresults[1];
#			    print $usertime . "\n";
#			}
			$usertime_avg_gbeta = $usertime_avg_gbeta + $usertime;
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search more lines
		    }
		} else {
		    if($myline =~ m/Measured/) {
			@results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[4] / 100;
			$usertime_avg_gbeta = $usertime_avg_gbeta + $usertime;
#			print "usertime_avg_gbeta:" . $usertime_avg_gbeta . "\n";
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search 
		    } 
		}
	    }
	    close(MYFILE);
#	    print "Parsing: " . "testoutput/speed_igbeta".$j.$i . "\n";
	    if($sourceFiles[$i]  =~ /simple.gbc/) {
		open(MYFILE, "testoutput/speed_igbeta".$i.$j) or die "\n $0 Cannot open $! \n";
#	        print "Parsing: " ."testoutput/WBresult".$j.$i . "\n";
		#print <myfile>;
		while (!eof(MYFILE)) {
		    $myline = <MYFILE>; 
		    #print $myline;
		    if($os eq "SunOS\n") {
			if($myline =~ m/user/) {
			    @results = split(/\s+/, $myline); #match whitespace chars as dividers
			    $usertime = $results[1];			    
			    if($usertime =~ m/:/){
				@subresults = split(/:/, $usertime);
				$usertime = 60*$subresults[0] + $subresults[1];
				print $usertime . "\n";
			    }
			    $usertime_avg_igbeta = $usertime_avg_igbeta + $usertime;
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			    last; #no need to search more lines
			}
		    } else {
			if($myline =~ m/\d+.\d+user/) {
			    @results = split(/\s/, $myline); #match whitespace chars as dividers
			    $usertime = $results[0];
			    $usertime =~ s/user//;
			    $usertime_avg_igbeta = $usertime_avg_igbeta + $usertime;
#			    print "usertime_avg_igbeta:" . $usertime_avg_igbeta . "\n";
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			    last; #no need to search more lines
			} 
		    }
		}
		close(MYFILE);
	    }
	}
#	$avgusertime_gbvm_nogc[$i] = $usertime_avg_gbvm_nogc / $speedIterations;
#	print $avgusertime_gbvm_nogc[$i] . "\n"; #"avgusertime_wb[$i]" . 
	$avgusertime_gbvm_gc[$i] = $usertime_avg_gbvm_gc / $speedIterations;
	print $avgusertime_gbvm_gc[$i] . "\n"; #"avgusertime_wb[$i]" . 
	$avgusertime_gbeta[$i] = $usertime_avg_gbeta / $speedIterations;
	print $avgusertime_gbeta[$i] . "\n"; #"avgusertime_wb[$i]" . 
	if($sourceFiles[$i]  =~ /simple.gbc/) {
#	    print $usertime_avg_igbeta . "\n";
	    $avgusertime_igbeta[$i] = $usertime_avg_igbeta / $speedIterations;
	    print $avgusertime_igbeta[$i] . "\n"; #"avgusertime_wb[$i]" . 
	}

	print "\n";
    }
}

if($doLangExecTest) {
    my($gbFile) = "";
    if($forReal == 1){
	for(my($j)=0; $j<$langIterations; $j++){
	    if($os eq "SunOS\n") {
		print "gbvm !With GC! cruncher3JAVA \n";
		print `bash -c \"if ! /user/hempe/f10/bin/timexx bin/gbvm_speed src/nodist/gbeta_programs/cruncher3JAVA.gbc &> testoutput/lang_gbvm$j; then echo Gik ned; fi\"`;
		print "java cruncher3.java\n";
		print `bash -c \"cd src/nodist/gbeta_programs/; /user/hempe/f10/bin/timexx java Main &> /user/hempe/gbvm/testoutput/lang_java$j\"`;
	    } else {
		print "gbvm !With GC! cruncher3JAVA \n";
		print `bash -c \"if (! \\time bin/gbvm_speed src/nodist/gbeta_programs/cruncher3JAVA.gbc &> testoutput/lang_gbvm$j); then echo Gik ned; fi\"`;
		print "java gbvm cruncher3.java \n";
		print `cd src/nodist/gbeta_programs; \\time java Main &> /home/hempe/gbvm/testoutput/lang_java$j`;
	    }
	}
    } else {
	print "you wimp!\n";
    }
}

if($doLangparse) {
    my($usertime_avg_gbvm) = 0;
    my($usertime_avg_java) = 0;
    for(my($j) = 0; $j<$langIterations; $j++){ #tried wbItereations nof times
	    open(MYFILE, "testoutput/lang_gbvm".$j) or die "\n $0 Cannot open $! \n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		    if($myline =~ m/user/) {
			@results = split(/\s+/, $myline); #match whitespace chars as dividers
			$usertime = $results[1];			    
			$usertime_avg_gbvm = $usertime_avg_gbvm + $usertime;
			last; #no need to search more lines
		    }
		} else {
		    if($myline =~ m/\d+.\d+user/) {
			@results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[0];
			$usertime =~ s/user//;
			$usertime_avg_gbvm = $usertime_avg_gbvm + $usertime;
			last; #no need to search more lines
		    } 
		}
	    }
	    close(MYFILE);
	    open(MYFILE, "testoutput/lang_java".$j) or die "\n $0 Cannot open $! \n";
#	    print "Parsing: " ."testoutput/2WBresult".$j.$i . "\n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		    if($myline =~ m/user/) {
			@results = split(/\s+/, $myline); #match whitespace chars as dividers
			$usertime = $results[1];
			$usertime_avg_java = $usertime_avg_java + $usertime;
			last; #no need to search more lines
		    }
		} else {
		    if($myline =~ m/\d+.\d+user/) {
			@results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[0];
			$usertime =~ s/user//;
			$usertime_avg_java = $usertime_avg_java + $usertime;
			last; #no need to search more lines
		    } 
		}
	    }
	    close(MYFILE);
	}
	print $usertime_avg_gbvm / $langIterations . "\n"; #"avgusertime_wb[$i]" . 
	print $usertime_avg_java / $langIterations . "\n"; #"avgusertime_2wb[$i] "
	#print "\n";
}



if($doWBBuild == 1){
    print `make clean`;
    buildExecutable("NEW_STACK_RESCUE=0.8 INTROSP_EXP=$is_size CAR_EXP=$cs_size SETVMREF=m", gbvm_wb);
    print `make clean`;
    buildExecutable("NEW_STACK_RESCUE=0.8 INTROSP_EXP=$is_size CAR_EXP=$cs_size SETVMREF=m WB2=y", gbvm_2wb);
}

if($doWBExecTest) {
    my($cmdline);
    $cmdline = "";
    if($forReal == 1){
 	my($i) = 0;
	foreach(@sourceFiles) {
	    print "gbvm_wb $_ $i \n";
	    for(my($itrere) = 0; $itrere<$wbIterations; $itrere++){
		if($os eq "SunOS\n") {
		    print `bash -c \"if ! /user/hempe/f10/bin/timexx bin/gbvm_wb $_ &> testoutput/WBresult$itrere$i ; then echo program terminated abnormally ; fi \"`;
		} else {
		    print `bash -c \"if (! \\time bin/gbvm_wb $_ &> testoutput/WBresult$itrere$i\n); then echo program terminated abnormally; fi\"`;
		}
	    }
	    print "gbvm_2wb $_ \n";
	    for($itrere = 0; $itrere<$wbIterations; $itrere++){
		if($os eq "SunOS\n") {
		    print `bash -c \"if ! /user/hempe/f10/bin/timexx bin/gbvm_2wb $_ &> testoutput/2WBresult$itrere$i ; then echo program terminated abnormally; fi\"`;
		} else {
		    print `bash -c \"if (! \\time bin/gbvm_2wb $_ &> testoutput/2WBresult$itrere$i\n); then echo program terminated abnormally; fi\"`;
		}
	    }
	    $i++;
	}
#	print `$cmdline`;
    } else {
	if($doWBBuild == 1){
	    print "make clean";
	    print "buildExecutable(LIVE=y NEW_STACK_RESCUE=0.8 INTROSP_EXP=$is_size CAR_EXP=$cs_size, gbvm_wb);";
	    print "buildExecutable(LIVE=y NEW_STACK_RESCUE=0.8 INTROSP_EXP=$is_size CAR_EXP=$cs_size 2WB=y, gbvm_2wb);";
	}
	foreach(@sourceFiles) {
	    $cmdline = $cmdline . "time bin/gbvm_wb $_ >> testoutput/WBresult\n";
	    $cmdline = $cmdline . "time bin/gbvm_2wb $_ >> testoutput/WBresult\n";
	}
	print "$cmdline";
    }
}

if($doWBparse) {
    my(@avgusertime_wb);
    my(@avgusertime_2wb);
    my($usertime_avg_wb) = 0;
    my($usertime_avg_2wb) = 0;
    for(my($i) = 0; $i<@sourceFiles; $i++){    #each source file
	$usertime_avg_wb = 0;
	$usertime_avg_2wb = 0;
	for(my($j) = 0; $j<$wbIterations; $j++){ #tried wbItereations nof times
	    open(MYFILE, "testoutput/WBresult".$j.$i) or die "\n $0 Cannot open $! \n";
#	    print "Parsing: " ."testoutput/WBresult".$j.$i . "\n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		    if($myline =~ m/user/) {
			@results = split(/\s+/, $myline); #match whitespace chars as dividers
			$usertime = $results[1];		
			$usertime_avg_wb = $usertime_avg_wb + $usertime;
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search more lines
		    }
		} else {
		    if($myline =~ m/\d+.\d+user/) {
			@results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[0];
			$usertime =~ s/user//;
			$usertime_avg_wb = $usertime_avg_wb + $usertime;
#			print "usertime_avg_wb:" . $usertime_avg_wb . "\n";
			last; #no need to search more lines
		    } 
		}
	    }
	    close(MYFILE);
	    open(MYFILE, "testoutput/2WBresult".$j.$i) or die "\n $0 Cannot open $! \n";
#	    print "Parsing: " ."testoutput/2WBresult".$j.$i . "\n";
	    #print <myfile>;
	    while (!eof(MYFILE)) {
		$myline = <MYFILE>; 
		#print $myline;
		if($os eq "SunOS\n") {
		    if($myline =~ m/user/) {
			@results = split(/\s+/, $myline); #match whitespace chars as dividers
			$usertime = $results[1];
			$usertime_avg_2wb = $usertime_avg_2wb + $usertime;
#			print "usertime_avg_2wb:". $usertime_avg_2wb . "\n";
			last; #no need to search more lines
		    }
		} else {
		    if($myline =~ m/\d+.\d+user/) {
			@results = split(/\s/, $myline); #match whitespace chars as dividers
			$usertime = $results[0];
			$usertime =~ s/user//;
			$usertime_avg_2wb = $usertime_avg_2wb + $usertime;
#			print "usertime_avg_2wb:" . $usertime_avg_2wb . "\n";
			last; #no need to search more lines
		    } 
		}
	    }
	    close(MYFILE);
	}
	$avgusertime_wb[$i] = $usertime_avg_wb / $wbIterations;
#	print $avgusertime_wb[$i] . "\n"; #"avgusertime_wb[$i]" . 
	$avgusertime_2wb[$i] = $usertime_avg_2wb / $wbIterations;
	print  $avgusertime_wb[$i] . " " . $avgusertime_2wb[$i] . "\n"; #"avgusertime_2wb[$i] "
	#print "\n";
#	my($cmd) = "echo $avgusertime \n > /testresult/wbres
    }
}

# HEMPE END

if($doExecTest) {
#    if(0) {
    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
	open(OUTFILEAVG, ">$outFileNameAvg" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	open(OUTFILEMAX, ">$outFileNameMax" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	open(OUTFILETOT, ">$outFileNameTot" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	    for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
		my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_dsrpt";
		$avgtime = 0; $avgTotalTime = 0; $avgworsttime = 0;
		for($run = 0; $run < $sourceFileRuns; ++$run) {
		    my($fileName) = "testoutput/out_" . $newExecName . "_src_$srcNum" . "_run$run";
		    my($cmdline) = "bash -c \"if (! bin/$newExecName $sourceFiles[$srcNum] &> $fileName ); then echo BAD_TERMINATION; fi \"";
#		    my($cmdline) = "bash -c \"\\timex bin/$newExecName $sourceFiles[$srcNum] &> $fileName\"";
#		    print $cmdline . "\n";
		    #	    print `$cmdline`;
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
#			    print "av: $avgtime av_max: $avgworsttime av_tot: $avgTotalTime\n";
			    last; #no need to search more lines
			}
		    }
		    close(MYFILE);
		}
		print OUTFILEAVG "$introExp $carExp $avgtime\n";
		print OUTFILEMAX "$introExp $carExp $avgworsttime\n";
		print OUTFILETOT "$introExp $carExp $avgTotalTime\n";
		print "\n";
		if($carExp == $carMax) {
		    print OUTFILEAVG "\n";
		    print OUTFILEMAX "\n";
		    print OUTFILETOT "\n";
		    print "\n";
		}
	    }
	}
	close(OUTFILEAVG);
	close(OUTFILEMAX);
	close(OUTFILETOT);
    }
#    } #if(0)

    #if(0) {
    print "***** space waste old *****\n";
    
    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
	print "source $srcNum \n";
	open(OUTFILELIVE,   ">$outFileNameLiveO"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	open(OUTFILEDEAD,   ">$outFileNameDeadO"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	open(OUTFILEUNUSED, ">$outFileNameUnusedO" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	    for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
		my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_waste_old";
		my($statLine) = -1; #skip the first line since no gc has taken place
		my($run) = 0;
		my($fileName) = "testoutput/out_" . $newExecName . "_src_$srcNum" . "_run$run";
		my($cmdline) = "bash -c \"if (! bin/$newExecName $sourceFiles[$srcNum] &> $fileName ); then echo BAD_TERMINATION; fi\"";
		#my($cmdline) = "bash -c \"\\time bin/$newExecName $sourceFiles[$srcNum] &> $fileName\n";
		#print `$cmdline`;
		
		$avgLive = 0; $avgDead = 0; $avgUnused = 0;
		open(MYFILE, $fileName) or die "\n $0 Cannot open $! \n";
		#print <myfile>;
		while (!eof(MYFILE)) {
		    $myline = <MYFILE>; 
		    #print $myline;
		    if($myline =~ m/MEMORY STAT/) {
			if($statLine >= 0) { #excluding the first line, starts at -1
			    @results = split(/\s/, $myline); #match whitespace chars as dividers
			    my($live)   = $results[3];
			    my($dead)   = $results[5];
			    my($unused) = $results[7];
			    my($total)  = $live + $dead + $unused;
			    if($total != 0) {
				$live   = $live/$total;
				$dead   = $dead/$total;
				$unused = $unused/$total;
			    } else {
				die "TOTAL NUMBER OF BYTES WAS 0!!!, INTERNAL ERROR!!!\n";
			    }
			    $avgLive      = (($avgLive   * $statLine) + $live  )/($statLine + 1);
			    $avgDead      = (($avgDead   * $statLine) + $dead  )/($statLine + 1);
			    $avgUnused    = (($avgUnused * $statLine) + $unused)/($statLine + 1);
#			    print "live: $results[3] $live $avgLive dead: $results[5] $dead $avgDead unused: $results[7] $unused $avgUnused\n";
			    
#			        print "av live: $avgLive av_dead: $avgDead av_unused: $avgUnused\n";
			}
			++$statLine;
		    }
		}
		close(MYFILE);
		
		print OUTFILELIVE "  $introExp $carExp $avgLive\n";
		print OUTFILEDEAD   "$introExp $carExp $avgDead\n";
		print OUTFILEUNUSED "$introExp $carExp $avgUnused\n";
		print "\n";
		if($carExp == $carMax) {
		    print OUTFILELIVE "\n";
		    print OUTFILEDEAD "\n";
		    print OUTFILEUNUSED "\n";
		    print "\n";
		}
	    }
	}
	close(OUTFILELIVE);
	close(OUTFILEDEAD);
	close(OUTFILEUNUSED);
    }

    print "***** space waste new *****\n";

    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
	print "source $srcNum \n";
	open(OUTFILELIVE,   ">$outFileNameLiveN"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	open(OUTFILEDEAD,   ">$outFileNameDeadN"   . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	open(OUTFILEUNUSED, ">$outFileNameUnusedN" . "_src_$srcNum") or die "\n $0 Cannot open $! \n";
	for($introExp = $introMin; $introExp <= $introMax; ++$introExp) {
	    for($carExp = $carMin; $carExp <= $carMax; ++$carExp) {
		my($newExecName) = $execName . "_" . $introExp . "_" . $carExp . "_waste_new";
		my($statLine) = -1;
		my($run) = 0;
		my($fileName) = "testoutput/out_" . $newExecName . "_src_$srcNum" . "_run$run";
		my($cmdline) = "bash -c \"if (! bin/$newExecName $sourceFiles[$srcNum] &> $fileName ); then echo BAD_TERMINATION; fi \"";
		#my($cmdline) = "bash -c \"\\time bin/$newExecName $sourceFiles[$srcNum] &> $fileName\n"; #out_" . $newExecName . "_run$run\"\n";
		#print `$cmdline`;

		$avgLive = 0; $avgDead = 0; $avgUnused = 0;
		open(MYFILE, $fileName) or die "\n $0 Cannot open $! \n";
		#print <myfile>;
		while (!eof(MYFILE)) {
		    $myline = <MYFILE>; 
		    #print $myline;
		    if($myline =~ m/MEMORY STAT/) {
			if($statLine >= 0) {
			    @results = split(/\s/, $myline); #match whitespace chars as dividers
			    my($live)   = $results[3];
			    my($dead)   = $results[5];
			    my($unused) = $results[7];
			    my($total)  = $live + $dead + $unused;
			    if($total != 0) {
				$live   = $live/$total;
				$dead   = $dead/$total;
				$unused = $unused/$total;
			    } else {
				die "TOTAL NUMBER OF BYTES WAS 0!!!, INTERNAL ERROR!!!\n";
			    }
			    $avgLive      = (($avgLive   * $statLine) + $live  )/($statLine + 1);
			    $avgDead      = (($avgDead   * $statLine) + $dead  )/($statLine + 1);
			    $avgUnused    = (($avgUnused * $statLine) + $unused)/($statLine + 1);
#			    print "live: $results[3] $live $avgLive dead: $results[5] $dead $avgDead unused: $results[7] $unused $avgUnused\n";
			    
#			    print "av live: $avgLive av_dead: $avgDead av_unused: $avgUnused\n";
			}
			++$statLine;
		    }
		}
		close(MYFILE);
		
		print OUTFILELIVE "  $introExp $carExp $avgLive\n";
		print OUTFILEDEAD   "$introExp $carExp $avgDead\n";
		print OUTFILEUNUSED "$introExp $carExp $avgUnused\n";
		print "\n";
		if($carExp == $carMax) {
		    print OUTFILELIVE "\n";
		    print OUTFILEDEAD "\n";
		    print OUTFILEUNUSED "\n";
		    print "\n";
		}
	    }
	}
	close(OUTFILELIVE);
	close(OUTFILEDEAD);
	close(OUTFILEUNUSED);
    }
    #} #if(0)
}

if($makeGraphs) {
#    @ext       = ("_dsrpt", "_waste_old", "_waste_new");
    @datafiles = ($outFileNameAvg, $outFileNameMax, $outFileNameTot,
		  $outFileNameLiveO, $outFileNameDeadO, $outFileNameUnusedO,
		  $outFileNameLiveN, $outFileNameDeadN, $outFileNameUnusedN);
    #foreach $extension (@ext) {
    #foreach (@sourceFiles) {
    for($srcNum = 0; $srcNum < @sourceFiles; ++$srcNum) {
	foreach $datafile (@datafiles){
	    
	    
	    open(CMDFILE, ">gnuplot_cmds");
	    print CMDFILE "set nokey\n";
	    print CMDFILE "set xtics 1\n";
	    print CMDFILE "set ytics 1\n";
	    print CMDFILE "set ticslevel 0\n";
#	        print CMDFILE "set view 90,310\n"; #sideview
#	        print CMDFILE "set view 60,310\n";
	    print CMDFILE "set view 45,310\n";
#	        print CMDFILE "set view 30,310\n";
#	        print CMDFILE "set view 10,310\n"; #from above
#	        print CMDFILE "set title \"Average Disruption Times\"\n";
	    print CMDFILE "set xlabel \"Introductory Space Size (2^x)\"\n";
	    print CMDFILE "set ylabel \"Car Size (2^x)\"\n";

	    #a kind of a switch-case statement
	    for($datafile){
		/$outFileNameAvg/ and do {
		    print CMDFILE "set zlabel \"Avg Disruption Time/s\"\n";
		    print CMDFILE "set zrange [0:]\n";
		    last;
		};
		/$outFileNameMax/ and do {
		    print CMDFILE "set zlabel \"Avg max Disruption Time/s\"\n";
		    print CMDFILE "set zrange [0:]\n";
		    last;
		};
		/$outFileNameTot/ and do {
		    print CMDFILE "set zlabel \"Total Disruption Time/s\"\n";
		    print CMDFILE "set zrange [0:]\n";
		    last;
		};
		(/$outFileNameLiveO/ or /$outFileNameLiveN/) and do {
		    print CMDFILE "set zlabel \"Avg Live Fraction/bytes\"\n";
		    print CMDFILE "set zrange [0:1]\n";
		    last;
		};
		(/$outFileNameDeadO/ or /$outFileNameDeadN/) and do {
		    print CMDFILE "set zlabel \"Avg Dead Fraction/bytes\"\n";
		    print CMDFILE "set zrange [0:1]\n";
		    last;
		};
		(/$outFileNameUnusedO/ or /$outFileNameUnusedN/) and do {
		    print CMDFILE "set zlabel \"Avg Unused Fraction/bytes\"\n";
		    print CMDFILE "set zrange [0:1]\n";
		    last;
		};
		die "Unknown datafile: " . $_;
	    }

	    print CMDFILE "set xrange [" . ($introMin-1) . ":" . ($introMax+1) . "]\n";
	    print CMDFILE "set yrange [" . ($carMin-1)   . ":" . ($carMax+1)   . "]\n";
	    print CMDFILE "set terminal fig\n";
	    #enhanced allows superscript, number is the fontsize
#	        print CMDFILE "set terminal postscript eps enhanced 12\n";
	    print CMDFILE "set output \"$datafile" . "_src_$srcNum" . ".fig\"\n";
#	        print CMDFILE "set output \"$datafile" . "_src_$srcNum" . ".eps\"\n";
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
#}





