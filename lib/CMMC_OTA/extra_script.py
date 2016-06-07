from SCons.Script import DefaultEnvironment
import glob, shutil

env = DefaultEnvironment()
# uncomment line below to see environment variables
# print env.Dump()
# print ARGUMENTS

for file in glob.iglob('src/*.*'):
    print 'Copied %s' % (file)
    shutil.copy2(file, "lib/CMMC_OTA")
