# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OpenCV_INSTALL_MODULES:=on
OPENCV_CAMERA_MODULES:=off

OPENCV_LIB_TYPE:=STATIC

ifeq ("$(wildcard $(OPENCV_MK_PATH))","")
include E:\OpenCV-2.4.10-android-sdk\sdk\native\jni\OpenCV.mk
else
include $(OPENCV_MK_PATH)
endif

LOCAL_MODULE    := VideoTest
LOCAL_SRC_FILES := DetectWithByte.cpp\
segmentation_1208_wf.cpp
LOCAL_LDLIBS    += -lm -llog

include $(BUILD_SHARED_LIBRARY)
