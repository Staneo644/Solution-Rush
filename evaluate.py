import sys
import collections

class Region:
	def __init__(self, line):
		self.name = line.split(':')[0].strip()
		self.pib = int(line.split(':')[1].strip())
		self.next_regions = line.split(':')[2].strip().split('-')

	def __str__(self):
		return self.name + ' :' + str(self.pib) + ' :' + '-'.join(self.next_regions)

	def touches(self, name):
		return name in self.next_regions


def doWeStop(listOfInt):
	for num in listOfInt:
		if num == 0:
			return True
	return False

class FusedRegion:
	def __init__(self, line, regions):
		self.regions = line.strip().split('-')
		self.pib = sum([region.pib for region in regions if region.name in self.regions])

		listOfInt = [0] * len(self.regions)
		listOfInt[0] = 1

		stop = True
		while stop and doWeStop(listOfInt):
			j = 0
			for region1 in self.regions:
				i = 0	
				stop = False
				for region2 in self.regions:
					if listOfInt[i] == 1 and [x for x in regions if x.name == region2][0].touches(region1):
						listOfInt[j] = 1
						stop = True
					i += 1
				j += 1


		if doWeStop(listOfInt):
			print(f'Some regions are not touching')
			exit()


	def __str__(self):
		return '-'.join(self.regions) + ' :' + str(self.pib)

def get_regions():
	regions = []
	with open(sys.argv[1], 'r') as f:
		for line in f.readlines():
			if line.strip():
				regions.append(Region(line))

	return regions

def get_fused_regions(regions, filename):
	try:
		fused = []
		with open(filename, 'r') as f:
			for line in f.readlines():
				if line.strip():
					fused.append(FusedRegion(line, regions))
		return fused
	except Exception as e:
		print(f'A critical error occured in parsing of {filename}: {e}')
		exit()

def compare_files(output_regions, reference_regions):
	try:
		x = [j for i in [region.regions for region in output_regions] for j in i]
		y = [j for i in [region.regions for region in reference_regions] for j in i]
		if collections.Counter(x) != collections.Counter(y):
			print('Wrong number/name of regions')
			exit()

	except Exception as e:
		print(f'A critical error occured in compare_files: {e}')
		exit()

def calculate_standard_deviation(fused_regions):
	average = sum([region.pib for region in fused_regions])
	return (sum([pow(region.pib - average, 2) for region in fused_regions]))


def main():
	regions = get_regions()
	output_regions = get_fused_regions(regions, sys.argv[2])
	reference_regions = get_fused_regions(regions, sys.argv[3])
	compare_files(output_regions, reference_regions)
	for region in output_regions:
		print(region)
	print(f'Reference standard deviation: {calculate_standard_deviation(reference_regions)}')
	print(f'Output standard deviation: {calculate_standard_deviation(output_regions)}')
	if calculate_standard_deviation(output_regions) <= calculate_standard_deviation(reference_regions):
		print('OK')
	else:
		print('KO')

if __name__ == '__main__':
	if (len(sys.argv) != 4):
		print('Usage: python3 evaluate.py <regions_file> <output_file> <reference_file>')
		exit()
	main()