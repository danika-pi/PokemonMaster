#!/usr/bin/perl
#use File::Copy qw(move);
#
#if (($#ARGV +1 ) < 1 ) 
#{
#    printf "Please provide a data file of latitude and lontitude\n";
#    exit;
#}
$targetFile_c="";
$targetFile_gpx="";

sub autoFileC
{
	my ($dataFile, $dataFileName, $defineTag, $extra) = @_;
	my $targetFileFullPath;

	$targetFile_c = $dataFile . $extra . ".c";
	#printf "targetFile_c = $targetFile_c\n";

	$cmd = "cat ./src/generator.c > $targetFile_c";
	system($cmd);

	if ($defineTag)
	{
		$cmd = "echo \"$defineTag\" >> $targetFile_c";
		system($cmd);
	}

	$cmd = "cat $dataFileName >> $targetFile_c";
	system($cmd);


	# find the pull path of gpx file
	$targetFile_gpx = $dataFile . $extra . ".gpx";

	#printf "dataFileName=$dataFileName\n";
	#printf "targetFile_gpx= $targetFile_gpx\n";
	if ($targetFile_gpx =~ /data/)
	{
		$targetFile_gpx =~ s/data/GPX/g;
		$targetFileFullPath = $targetFile_gpx;
	}
	else
	{
		my $currentDir =`pwd`;
		chomp($currentDir);
		my $myPath = "$currentDir" . "/";
		$myPath =~ s/DATA/GPX/g;
		$targetFileFullPath = "$myPath" . "$targetFile_gpx";
	}
	#printf "targetFileFullPath=$targetFileFullPath\n";

	$addTargetFile = "char targetFile[256] = \\\"$targetFileFullPath\\\"\;";
	#printf "addTargetFile= $addTargetFile\n";

	$cmd = "echo \"$addTargetFile\" >> $targetFile_c";
	system($cmd);
}

sub createGpxFile
{
	my ($dataFileName, $others) =  @_;

	#printf "dataFileName=$dataFileName\n";
	chomp($dataFileName);
	$counter = 1;
	$multipleFile = 0;
	($dataFile, $rest) = split(/\.dat/, $dataFileName);  
	#printf "dataFile=$dataFile\n";

	if ($dataFile =~ /_AB_/)
	{
		($dataFileVar1, $var2) = split(/_AB_/, $dataFile);
		$counter = 2;
		$multipleFile = 1;
	}
	elsif ($dataFile =~ /_ABCD_/)
	{
		($dataFileVar1, $var2) = split(/_ABCD_/, $dataFile);
		$counter = 4;
		$multipleFile = 1;
	}
	else
	{
		$dataFileVar1 = $dataFile;
	}
	for my $i (1..$counter)
	{

		$defineTag = " ";

		if ($multipleFile == 0)
		{
			autoFileC($dataFileVar1, $dataFileName);
		}
		else
		{
			if ($i == 1)
			{
				$defineTag = "#define PART_A 1";
				autoFileC($dataFileVar1, $dataFileName, $defineTag, "_A");
			}
			elsif ($i == 2)
			{
				$defineTag = "#define PART_B 1";
				autoFileC($dataFileVar1, $dataFileName, $defineTag, "_B");
			}
			elsif ($i == 3)
			{
				$defineTag = "#define PART_C 1";
				autoFileC($dataFileVar1, $dataFileName, $defineTag, "_C");
			}
			elsif ($i == 4)
			{
				$defineTag = "#define PART_D 1";
				autoFileC($dataFileVar1, $dataFileName, $defineTag, "_D");
			}
		}

		$cmd = "gcc $targetFile_c -lm";
		system($cmd);

		$cmd = "./a.out";
		system($cmd);
		printf "Generate $targetFile_gpx\n";

		unlink("./a.out");
		#unlink("$targetFile_c");
	}
}

#main
{
my $dataFileNameInput;
my $cleanOnly = 0;

$inputParam = $ARGV[0];

if ($inputParam =~ /help/)
{
	printf "Usage:\n";
	printf "    gpxGenerator.pl                     // To create all gpx file for all .dat under this directory\n";
	printf "    gpxGenerator.pl SF_UnionSquare.dat  // To create one gpx file for this .dat file only\n";
	printf "    gpxGenerator.pl clean               // To remove all *.c, *.o and *.gpx under this directory.\n";

	exit 1;
}

if ($inputParam =~ /clean/)
{
	my @allFiles = `find ./ -type f -name '*.gpx'| sort -u`;

	foreach $fileName (@allFiles)
	{
		chomp($fileName);
		unlink("$fileName");
	} #endif foreach $fileName

	@allFiles = `find ./ -type f -name 'a.out'| sort -u`;
	foreach $fileName (@allFiles)
	{
		chomp($fileName);
		unlink("$fileName");
	} #endif foreach $fileName


	@allFiles = `find ./ -type f -name '*.c'| sort -u`;
	foreach $fileName (@allFiles)
	{
		if ($fileName =~ /generator.c/)
		{
			next;
		}
		chomp($fileName);
		unlink("$fileName");
	} #endif foreach $fileName
	
	exit 1;

}
elsif ($inputParam =~ /\.dat/)
{
	$dataFileNameInput = $inputParam;
}

if ($dataFileNameInput)
{
	$dataFileName = $dataFileNameInput;
	createGpxFile( $dataFileName);
}
else
{
	@allDataFiles = `find . -type f -name '*.dat'| sort -u`;
	my $size = @allDataFiles;
	
	if ($size == 0)
	{
		printf "Error: No .dat file under this directory\n";
		exit 1;
	}

	foreach $dataFileName (@allDataFiles)
	{
		createGpxFile( $dataFileName);
	} #endif foreach $dataFile
}

} #end of main
