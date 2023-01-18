# Description: Custom name for firmware
     
Import("env")

# Dump global construction environment (for debug purpose)
#print(env.Dump())

my_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = dict()
for x in my_flags.get("CPPDEFINES"):
    if type(x) is tuple:
        (k,v) = x
        defines[k] = v
        #print("Type Tuple: %s" % x)
    elif type(x) is list:
        k = x[0]
        v = x[1]
        defines[k] = v
        #print("Type List: %s" % x)
    else:
        defines[x] = "" # empty value
        #print("Warning: unknown type for %s" % x)

# strip quotes needed for shell escaping
s = lambda x: x.replace('"', "")

env.Replace(
    PROGNAME="%s-%s-%s-%s-%s" %
    (s(defines.get("PIO_SRC_NAM")), s(defines.get("PIO_SRC_TAG")), str(env["PIOENV"]),
     s(defines.get("PIO_SRC_REV")), s(defines.get("PIO_SRC_BRH"))))

#detect if there is a forward slash in the PROGNAME and replace it with an underscore
if "/" in env["PROGNAME"]:
    env.Replace(
        PROGNAME="%s" %
        (env["PROGNAME"].replace("/", "-")))

# create a file with the name of the firmware
env.Execute("echo %s > %s" % (env["PROGNAME"], env.subst("./tools/firmware_name.txt")))