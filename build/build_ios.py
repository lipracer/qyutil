import os
import shutil

xcode_build_cmd = '''xcodebuild -project "{xcodeproj}" -target '{projname}' -configuration '{Release}' -sdk 'iphoneos' clean build \
ARCHS='armv7 arm64' IPHONEOS_DEPLOYMENT_TARGET='9.0' ONLY_ACTIVE_ARCH=NO BUILD_PRODUCTS_DIR="{product_dir}"
'''

def check_env():
    '''
    检查环境变量、Xcodebuild
    '''
    out = os.popen("xcodebuild -version")
    version_str = out.read()
    versions = version_str.split('\n')
    #print(len(versions), versions[0].split(' ')[0], versions[1].split(' ')[0])
    if len(versions) == 2 and versions[0].split(' ')[0] == "Xcode" and versions[1].split(' ')[0] == "Build":
        print(version_str)
    else:
        #raise Exception("env error")
        pass
def build_target():
    build_cmd = xcode_build_cmd.format(xcodeproj="./ios/qyutil/qyutil.xcodeproj",
                                        projname="qyutil",
                                        Release="Release",
                                        product_dir="./")
    print(build_cmd)
    out = os.popen(build_cmd)
    while True:
        line = out.readline()
        if not line:
            break
        print(line)

def clean_build_fold():
    build_path = './ios/qyutil/build'
    if os.path.exists(build_path):
        shutil.rmtree(build_path)    

def main():
    check_env()
    clean_build_fold()
    build_target()


if __name__ == "__main__":
    main()
