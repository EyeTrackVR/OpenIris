FILENAME_VERSION = 'version.txt'  
build_no = 'v0.0.0'
try:
    with open(FILENAME_VERSION) as f:
        build_no = f.readline()
        if build_no == '':
            raise Exception("Empty file")
        version = build_no.split('v')
        version = version[1].split('.')
        first_num = int(version[0])
        second_num = int(version[1])
        last_num = int(version[2])
        if last_num <= 9:
            last_num += 1
        elif second_num <= 9 and last_num >= 9:
            second_num += 1
        elif second_num and last_num >= 9:
            first_num += 1
            second_num = 0
            last_num = 0
        else:
            raise Exception("Invalid version number")
        build_no = 'v' + str(first_num) + '.' + str(second_num) + '.' + str(last_num)
except:
    build_no = "v0.0.1"
with open(FILENAME_VERSION, 'w+') as f:
    f.write(build_no)
print("-DVERSION={0}".format(build_no))
