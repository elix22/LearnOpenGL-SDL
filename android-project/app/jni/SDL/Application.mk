
# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
# APP_STL := stlport_static 

APP_ABI :=  armeabi-v7a arm64-v8a x86 x86_64

# Min SDK level
APP_PLATFORM=android-16
APP_CPPFLAGS += -std=c++14
APP_STL      := c++_static

