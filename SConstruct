import os
env = Environment()
env['CC'] = 'avr-gcc'

env.Append(CCFLAGS = ['-mmcu=atmega644p', '-Os', '-ffast-math', '-Wall'])
env.Append(LINKFLAGS = ['-mmcu=atmega644p'])


#BUILD
objs = Glob('src/*.c')

prog = env.Program(target='prog', source=objs)
Default(prog)

#CLEAN
Clean(prog, Glob('src/*~') + ['config.log'] + Glob('*.hex') + Glob('*~'))
