FILENAME_VERSION = 'version.txt'  
build_no = 'v0.0.0'

try:
    with open(FILENAME_VERSION) as f:
        build_no = f.readline()
        if build_no == '':
            raise Exception("Empty file")
        
        version = build_no.split('v')
        version = version[1].split('.')
        ## check the last number in the version
        first_num = int(version[0])
        second_num = int(version[1])
        last_num = int(version[2])
#
        ## use sematic versioning to increment the version numbers and join them back together
        if last_num <= 9:
            last_num += 1
            build_no = 'v' + str(first_num) + '.' + str(second_num) + '.' + str(last_num)
        elif second_num <= 9 and last_num >= 9:
            second_num += 1
            build_no = 'v' + str(first_num) + '.' + str(second_num) + '.' + str(last_num)
        elif second_num and last_num >= 9:
            # increment the first number and reset the second and last numbers
            first_num += 1
            second_num = 0
            last_num = 0
            build_no = 'v' + str(first_num) + '.' + str(second_num) + '.' + str(last_num)
        else:
            raise Exception("Invalid version number")
except:
    build_no = "v0.0.1"

# create a file with the version number
with open(FILENAME_VERSION, 'w+') as f:
    f.write(build_no)

#print("-DVERSION=\"%s\"" % version+str(build_no))
print("-DVERSION={0}".format(build_no))
