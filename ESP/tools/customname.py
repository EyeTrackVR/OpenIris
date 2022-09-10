# Description: Custom name for firmware
     
Import("env")

my_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = dict()
for x in my_flags.get("CPPDEFINES"):
    if type(x) is tuple:
        (k,v) = x
        defines[k] = v
    elif type(x) is list:
        k = x[0]
        v = x[1]
        defines[k] = v
    else:
        defines[x] = "" # empty value
# defines.get("PIO_SRC_TAG") - tag name
# strip quotes needed for shell escaping
s = lambda x: x.replace('"', "")
env.Replace(
    PROGNAME="%s-%s-%s-%s-%s-%s" %
    (s(defines.get("PIO_SRC_NAM")), s(defines.get("VERSION")), s(defines.get("PIO_SRC_TAG")), str(env["BOARD"]),
     s(defines.get("PIO_SRC_REV")), s(defines.get("PIO_SRC_BRH"))))
