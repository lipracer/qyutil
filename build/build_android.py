import os

def check_env():
    out = os.popen('ndk-build')
    result = out.read()
    version = result.strip().split('\n')[0]
    if not version.startswith('GUn Make'):
        raise Exception("env error!")
    pass

def build():
    #check_env()
    os.chdir('../src/jni')
    print(os.getcwd())
    out = os.popen('echo $PATH')
    while True:
        line = out.readline()
        if not line:
            break
        print(line)
        
def build_clear():
    pass

if __name__ == "__main__":
    build()
