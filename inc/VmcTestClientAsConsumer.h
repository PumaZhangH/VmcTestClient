/**
********************************************************************************
* COPYRIGHT (c) 2024 Harman International Industries, Inc                      *
*                                                                              *
* All rights reserved                                                          *
*                                                                              *
* This software embodies materials and concepts which are                      *
* confidential to Harman International Industries, Inc. and is                 *
* made available solely pursuant to the terms of a written license             *
* agreement with Harman International Industries, Inc.                         *
*                                                                              *
* Designed and Developed by Harman International Industries, Inc.              *
*------------------------------------------------------------------------------*
* @file VmcTestClientAsConsumer.h
* @author Puma Zhang <Puma.Zhang@harman.com>
********************************************************************************
*/

#pragma once

#include <cstdint>
#include <v3/BMW/INFOTAINMENT/ENTERTAINMENT/VMC/VehicleMediaControlProxy.hpp>

#include "VmcTestClient.h"

using vmcProx = std::shared_ptr<FidlVMC::VehicleMediaControlProxyBase>;

class VmcTestClientAsConsumer {
public:
    VmcTestClientAsConsumer();
    ~VmcTestClientAsConsumer() = default;
    /**
     * isAvailable - Checks if the Proxy is available
     *
     * @return true  - Available
     * false - Unavailable
     */
    bool isAvailable();
    void generalGetIcon(const std::string& testId, const std::string& testName);
    void generalGetParameters(const std::string& testId, const std::string& testName);
    void playerPlayFromMediaId(const std::string& mediaId, const std::string& testId,
                               const std::string& testName);
    void browserAcknowledgeKeepAlive(fidlTypes::VmcViewId viewId, const std::string& testId,
                                     const std::string& testName);
    void browserCreateView(const fidlTypes::VmcSourceId& sourceId,
                           const fidlTypes::VmcWindowSize& windowSize,
                           const fidlTypes::VmcSelectedMediaItemUnion& selectedMediaItem,
                           const std::string& testId, const std::string& testName);
    void browserDestroyView(fidlTypes::VmcViewId viewId, const std::string& testId,
                            const std::string& testName);
    void browserMoveViewToItem(const fidlTypes::VmcViewId& viewId,
                               const fidlTypes::VmcMediaId& mediaId,
                               const fidlTypes::VmcSelectedMediaItemUnion& selectedMediaItem,
                               const std::string& testId, const std::string& testName);
    void browserResizeWindow(const fidlTypes::VmcViewId& viewId,
                             const fidlTypes::VmcWindowSize& newWindowSize,
                             const std::string& testId, const std::string& testName);
    void browserMoveWindow(const fidlTypes::VmcViewId& viewId,
                           const fidlTypes::VmcSelectedMediaItemUnion& selectedMediaItem,
                           const std::string& testId, const std::string& testName);
    void buildPairVectorWithIndex(uint32_t index,
                                  std::vector<FidlVmcKeyValuePair>& keyValuePairVector);
    void buildVmcMediaSourceProperties(FidlVmcMediaSourceProperties& mediaSourceProperties, const std::string& browseRoot);
    void buildVmcMediaItemMetadata(FidlVmcMediaItemMetadata& mediaItemMetadata, const std::string& displayIconId);
    void buildVmcMediaSourceItemByName(std::string inputName,
                                       std::vector<FidlVmcMediaSourceItem>& mediaSourceItemVector);
    void playerPause(const std::string& testId, const std::string& testName);
    void playerPlay(const std::string& testId, const std::string& testName);

    void playerRewind(const std::string& testId, const std::string& testName);
    void playerSeekTo(uint32_t newElapsedTime, const std::string& testId,
                      const std::string& testName);
    void generalSetParameters(uint32_t index, const std::string& testId,
                              const std::string& testName);

    void playerSkipToPrevious(const std::string& testId, const std::string& testName);
    void playerSkipToNext(const std::string& testId, const std::string& testName);
    void playerStop(const std::string& testId, const std::string& testName);
    void sourceConnectTo(std::string _sourceId, const std::string& testId,
                         const std::string& testName);
    void playerFastForward(const std::string& testId, const std::string& testName);
    void playerSkipToMediaId( const std::string& testId,const std::string& testName,const std::string& mediaId);


private:
    void runProxy(std::string name);

public:
    vmcProx mVehicleMediaControlProxy;
    CommonAPI::CallStatus mCallStatus;
};

static const char* DumpIconFilePath = "/vendor/bin/hw/iconRaw.bin";

#define DUMP_TO_FILE(m_filename, m_buf, m_size)              \
    {                                                        \
        FILE* fp = fopen(m_filename, "ab");                  \
        if (fp != NULL && m_buf != NULL) {                   \
            int i;                                           \
            ALOGD("Dump to file!");                          \
            i = fwrite(m_buf, 1, m_size, fp);                \
            if (i != (int)m_size) {                          \
                ALOGD("Error in fwrite, returned %d", i);    \
                perror("Error in write to file");            \
            }                                                \
            fclose(fp);                                      \
        } else {                                             \
            ALOGD("Could not write to file %s", m_filename); \
            if (fp != NULL) fclose(fp);                      \
        }                                                    \
    }

#define READ_FROM_FILE(m_filename, m_buf, m_size)            \
    {                                                        \
        FILE* fp = fopen(m_filename, "rb");                  \
        if (fp != NULL && m_buf != NULL) {                   \
            int i;                                           \
            ALOGD("Read from file!");                        \
            i = fread(m_buf, 1, m_size, fp);                 \
            if (i != (int)m_size) {                          \
                ALOGD("Error in fwrite, returned %d", i);    \
                perror("Error in write to file");            \
            }                                                \
            fclose(fp);                                      \
        } else {                                             \
            ALOGD("Could not write to file %s", m_filename); \
            if (fp != NULL) fclose(fp);                      \
        }                                                    \
    }
