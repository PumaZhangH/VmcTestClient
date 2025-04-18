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
* @file VmcTestClientAsProvider.h
* @author Puma Zhang <Puma.Zhang@harman.com>
********************************************************************************
*/

#pragma once

#ifndef LOG_TAG
#    define LOG_TAG "VmcTest"
#endif

#include <v3/BMW/INFOTAINMENT/ENTERTAINMENT/VMC/VehicleMediaControlProxy.hpp>

#include "VehicleMediaControlLog.h"
#include "VmcTestClient.h"
#include "v3/BMW/INFOTAINMENT/ENTERTAINMENT/VMC/VehicleMediaControlStubDefault.hpp"

class VmcTestClientAsProvider : public FidlVMC::VehicleMediaControlStubDefault {
public:
    VmcTestClientAsProvider(std::string instance)
        : mInstance(std::move(instance)){};
    ~VmcTestClientAsProvider() override = default;

    template <typename ResultType, typename ReplyCallback, typename... ReplyArgs>
    void genericFunction(const std::shared_ptr<CommonAPI::ClientId> client, ReplyCallback reply,
                         const std::string& logMessage, ResultType result, ReplyArgs... replyArgs) {
        VMC_LOGI("%s    =================== PASS.", logMessage.c_str());
        reply(result, replyArgs...);
    }

    void sourceConnectTo(const std::shared_ptr<CommonAPI::ClientId> client,
                         fidlTypes::VmcSourceId _sourceId, sourceConnectToReply_t reply) override;

    void playerFastForward(const std::shared_ptr<CommonAPI::ClientId> client,
                           playerFastForwardReply_t reply) override;

    void generalGetIcon(const std::shared_ptr<CommonAPI::ClientId> client,
                        const fidlTypes::VmcIconInformation _iconInformation,
                        generalGetIconReply_t reply) override;

    void generalGetParameters(const std::shared_ptr<CommonAPI::ClientId> client,
                              std::vector<std::string> _keys,
                              generalGetParametersReply_t reply) override;
    void playerPause(const std::shared_ptr<CommonAPI::ClientId> client,
                     playerPauseReply_t reply) override;

    void playerPlay(const std::shared_ptr<CommonAPI::ClientId> client,
                    playerPlayReply_t reply) override;

    void playerRewind(const std::shared_ptr<CommonAPI::ClientId> client,
                      playerRewindReply_t reply) override;

    void playerSeekTo(const std::shared_ptr<CommonAPI::ClientId> client,
                      const fidlTypes::VmcTimeInMs _newElapsedTime,
                      const playerSeekToReply_t reply) override;

    void generalSetParameters(const std::shared_ptr<CommonAPI::ClientId> client,
                              const std::vector<fidlTypes::VmcKeyValuePair> _inputParameters,
                              const generalSetParametersReply_t reply) override;
    void playerSkipToNext(const std::shared_ptr<CommonAPI::ClientId> client,
                          playerSkipToNextReply_t reply) override;
    void playerSkipToPrevious(const std::shared_ptr<CommonAPI::ClientId> client,
                              playerSkipToPreviousReply_t reply) override;
    void playerStop(const std::shared_ptr<CommonAPI::ClientId> client,
                    playerStopReply_t reply) override;

    void playerPlayFromMediaId(const std::shared_ptr<CommonAPI::ClientId> client,
                               const fidlTypes::VmcMediaId _mediaId,
                               playerPlayFromMediaIdReply_t reply) override;
    void playerSkipToMediaId(const std::shared_ptr<CommonAPI::ClientId> client,
                             const fidlTypes::VmcMediaId _mediaId,
                             playerSkipToMediaIdReply_t reply) override;

    void browserAcknowledgeKeepAlive(const std::shared_ptr<CommonAPI::ClientId> client,
                                     const fidlTypes::VmcViewId _viewId,
                                     browserAcknowledgeKeepAliveReply_t reply) override;

    void browserCreateView(const std::shared_ptr<CommonAPI::ClientId> client,
                           ::fidlTypes::VmcSourceId _sourceId,
                           ::fidlTypes::VmcWindowSize _windowSize,
                           ::fidlTypes::VmcSelectedMediaItemUnion _selectedMediaItem,
                           browserCreateViewReply_t reply) override;
    void browserDestroyView(const std::shared_ptr<CommonAPI::ClientId> client,
                            const fidlTypes::VmcViewId _viewId,
                            browserDestroyViewReply_t reply) override;

    void browserMoveViewToItem(const std::shared_ptr<CommonAPI::ClientId> client,
                               const fidlTypes::VmcViewId viewId, ::fidlTypes::VmcMediaId _mediaId,
                               ::fidlTypes::VmcSelectedMediaItemUnion _selectedMediaItem,
                               browserMoveViewToItemReply_t reply) override;
    void browserResizeWindow(const std::shared_ptr<CommonAPI::ClientId> client,
                             const fidlTypes::VmcViewId _viewId,
                             const fidlTypes::VmcWindowSize _newWindowSize,
                             browserResizeWindowReply_t reply) override;

    void browserMoveWindow(const std::shared_ptr<CommonAPI::ClientId> client,
                           const fidlTypes::VmcViewId _viewId,
                           fidlTypes::VmcSelectedMediaItemUnion _selectedMediaItem,
                           browserMoveWindowReply_t reply) override;

public:
    std::string mInstance;
};
