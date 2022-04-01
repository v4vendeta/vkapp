import sys
import os


path, _ = os.path.split(os.path.abspath(sys.argv[0]))

def glslc(shaderPath):
    input = os.path.join(path, shaderPath)
    output = os.path.join(os.path.join(path, "spirv"), shaderPath) + ".spv"
    cmd="glslc"+ " " + input + " -o " + output
    os.system(cmd)

def main():
    glslc("defaultlit.vert")
    glslc("defaultlit.frag")
    glslc("postprocess.vert")
    glslc("postprocess.frag")
    
if __name__ == '__main__':
    main()
