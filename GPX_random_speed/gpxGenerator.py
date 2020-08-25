#!/usr/bin/python3.7
import subprocess, os, re, sys

targetFile_c=""
targetFile_gpx=""

def autoFileC(dataFile, dataFileName, defineTag, extra):
	global targetFile_c, targetFile_gpx
	#print("dataFile=" + dataFile)
	targetFile_c = dataFile + extra + ".c"
	#print("targetFile_c=" + targetFile_c)
	cmd = ['cat', 'src/generator.c']
	fout = open(targetFile_c, "w")
	subprocess.run(cmd, stdout=fout) 

	if defineTag:
		with open(targetFile_c, "a") as fout:
			fout.write(defineTag + "\n")

	with open(dataFileName, "r") as fin:
		readFile=fin.read()
	with open(targetFile_c, "a") as fout:
		fout.write(readFile)

	# find full path of gpx file
	targetFile_gpx = dataFile + extra + ".gpx"

	#print("dataFileName = " + dataFileName)
	#print("targetFile_gpx = " + targetFile_gpx)

	if re.search('data', targetFile_gpx):
		targetFile_gpx = re.sub('data', 'GPX', targetFile_gpx)
		targetFileFullPath = targetFile_gpx
	else:
		currentDir = subprocess.run(["pwd"], check=True, capture_output=True)
		currentDir.strip()
		myPath = currentDir + "/"
		mydataFile, dataFileName, defineTag, extraPath = re.sub('DATA', 'GPX', targetFile_gpx)
		targetFileFullPath = myPath + targetFile_gpx
	#print("targetFileFullPath = " + targetFileFullPath + "\n")

	addTargetFile = "char targetFile[256] = "+ '"' + targetFileFullPath +'"' + ";"

	with open(targetFile_c, "a") as fout:
		fout.write(addTargetFile)

def createGpxFile(dataFileName):
	#print("dataFileName=" + dataFileName)
	dataFileName.strip()
	counter = 1
	multipleFile = 0
	dataFile, rest = dataFileName.split('.dat')
	#print("dataFile=" + dataFile);

	if re.search("_AB_", dataFile):
		dataFileVar1, var2 = dataFile.split('_AB_')
		counter = 2
		multipleFile = 1
	elif re.search("_ABCD_", dataFile):
		dataFileVar1, var2 = dataFile.split('_ABCD_')
		counter = 4
		multipleFile = 1
	else:
		dataFileVar1 = dataFile
		#print("dataFileVar1=" + dataFileVar1)
	
	for i in range(1, counter + 1):
		defineTag = ""
		if multipleFile == 0:
			autoFileC(dataFileVar1, dataFileName,"","")
		else:
			if i == 1:
			    defineTag = "#define PART_A 1"
			    autoFileC(dataFileVar1, dataFileName, defineTag, "_A")
			elif i == 2:
			    defineTag =  "#define PART_B 1"
			    autoFileC(dataFileVar1, dataFileName, defineTag, "_B")
			elif i == 3:
			    defineTag = "#define PART_C 1"
			    autoFileC(dataFileVar1, dataFileName, defineTag, "_C")
			elif i == 4:
			    defineTag = "#define PART_D 1"
			    autoFileC(dataFileVar1, dataFileName, defineTag, "_D")
		subprocess.run(['gcc', targetFile_c, '-lm'])
		subprocess.run(['./a.out'], shell=True)

		print("Generate " + targetFile_gpx)

		os.unlink("./a.out")
		os.unlink(targetFile_c)

def main(argv):
	dataFileNameInput="" 
	cleanOnly = 0
	#print("length of argv ="+ str(len(argv)))

	if len(argv) >= 1: 
		inputParam = ''.join(argv[0]) 
		#print(inputParam)
		if re.search('help', inputParam):
			print("Usage:\n")
			print("	gpxGenerator.pl                     // To create all gpx file for all .dat under this directory\n")
			print("	gpxGenerator.pl SF_UnionSquare.dat  // To create one gpx file for this .dat file only\n")
			print("	gpxGenerator.pl clean               // To remove all *.c, *.o and *.gpx under this directory.\n")
			exit(1)

		if re.search('clean', inputParam):
			cmd = ['find', './', '-type','f', '-name', '*.gpx']
			find = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			end_of_pipe = find.stdout
			for line in end_of_pipe:
				lineStr = line.strip().decode('utf-8')
				os.unlink(lineStr)

			cmd = ['find', './', '-type','f', '-name', '*.c']
			find = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			end_of_pipe = find.stdout
			for line in end_of_pipe:
				lineStr = line.strip().decode('utf-8')
				if re.search('generator.c', lineStr):
					continue
				os.unlink(lineStr)

			cmd = ['find', './', '-type','f', '-name', 'a.out']
			find = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			end_of_pipe = find.stdout
			for line in end_of_pipe:
				lineStr = line.strip().decode('utf-8')
				os.unlink(lineStr)
			exit(1)

		if re.search('\.dat', inputParam):
			dataFileNameInput= inputParam
		if dataFileNameInput:
			dataFileName = dataFileNameInput
			createGpxFile(dataFileName)

	else: # excute for all *.dat
		allDataFiles = []
		cmd = ['find', './', '-type', 'f', '-name', '*.dat']
		find = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		end_of_pipe = find.stdout
		for line in end_of_pipe:
			lineStr = line.strip().decode('utf-8')
			#print(lineStr)
			allDataFiles.append(lineStr)
		size = len(allDataFiles)

		if size == 0:
			print("Error: No .dat file under this directory\n")
			exit(1)
		for dataFileName in allDataFiles:
			#print(dataFileName)
			createGpxFile(dataFileName)

if __name__ == "__main__":
	main(sys.argv[1:])
