SDK := $(shell xcrun --show-sdk-path --sdk iphoneos)
ARCH := arm64
CC := clang -arch $(ARCH) -isysroot $(SDK) -miphoneos-version-min=15.0

all:
	@echo "============================================"
	@echo " JackAndroid - by digson-kirky-CE"
	@echo " Building iOS .Mac daemon + KEXT (Xcode needed for KEXT)"
	@echo "============================================"
	$(CC) -o mac_daemon_ios mac_daemon_ios.c
	@echo "✅ mac_daemon_ios built"

kext:
	xcodebuild -project ../../JackAndroidKext.xcodeproj \
	    -sdk iphoneos -arch arm64 -configuration Debug

clean:
	rm -f mac_daemon_ios
