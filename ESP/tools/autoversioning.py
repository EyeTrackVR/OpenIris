FILENAME_BUILDNO = 'tools/versioning'  
version = 'v0.1.'
build_no = 0

try:
    with open(FILENAME_BUILDNO) as f:
        build_no = int(f.readline()) + 1
except:
    build_no = 1
with open(FILENAME_BUILDNO, 'w+') as f:
    f.write(str(build_no))
    
#print("-DVERSION=\"%s\"" % version+str(build_no))
print("-DVERSION={0}".format(version+str(build_no)))
