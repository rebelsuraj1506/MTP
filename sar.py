input_file = 'test.txt'
output_file = 'memory_addresses.txt'
count = 0

with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
    for line in infile:
        if line.startswith(('I', 'S', 'L')):
            parts = line.split()
            if len(parts) > 1:
                parts = parts[1].split(',')
                parts = parts[0]
                outfile.write(parts + '\n')
                count += 1

print(count)
