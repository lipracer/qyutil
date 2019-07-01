import os
import shutil

ProjectDir = '/Users/lipracer/SourceCode/first/pizza-android/app/src/main/jniLibs/armeabi-v7a'

def rm_path(path):
    if os.path.exists(path):
        print("rm:" + path)
        shutil.rmtree(path)

def check_env():
    out = os.popen('ndk-build -v')
    result = out.read()
    version = result.strip().split('\n')[0]
    print("version:%s" %version)
    if not version.startswith('GNU Make'):
        raise Exception("env error!")
    pass

def build():
    check_env()
    os.chdir('../src/jni')
    build_clear()
    out = os.popen('ndk-build')
    while True:
        line = out.readline()
        if not line:
            break
        print(line.strip('\n'))
        
    shutil.move("../libs", "../../build/android")
    shutil.move("../obj", "../../build/android")
    if os.path.exists(ProjectDir):
        shutil.copy("../../build/android/libs/armeabi-v7a/libqyutil.so", ProjectDir)
        print("copy to project complete.")
    
def build_clear():
    rm_path("../libs")
    rm_path("../obj")
    rm_path("../../build/android/libs")
    rm_path("../../build/android/obj")

def main():
    build()

if __name__ == "__main__":
    main()
