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
* @file    VmcTestClientAsProvider.cpp
* @author Puma Zhang <Puma.Zhang@harman.com>
********************************************************************************
*/

#include "VmcTestClientAsProvider.h"

#include "VehicleMediaControlLog.h"
#include "VmcTestClient.h"

void VmcTestClientAsProvider::sourceConnectTo(const std::shared_ptr<CommonAPI::ClientId> client,
                                              fidlTypes::VmcSourceId _sourceId,
                                              sourceConnectToReply_t reply) {
    VMC_LOGV("VmcTestClientAsProvider stub instance_:%s : sourceId:%s", mInstance.c_str(),
             _sourceId.c_str());
    auto result = FidlVmcConnectToSourceResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-sourceConnectTo", result);
}

void VmcTestClientAsProvider::playerFastForward(const std::shared_ptr<CommonAPI::ClientId> client,
                                                playerFastForwardReply_t reply) {
    auto result = FidlVmcMediaPlayerInteractionResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerFastForward", result);
}

void VmcTestClientAsProvider::playerPause(const std::shared_ptr<CommonAPI::ClientId> client,
                                          playerPauseReply_t reply) {
    auto result = FidlVmcMediaPlayerInteractionResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerPause", result);
}

void VmcTestClientAsProvider::playerPlay(const std::shared_ptr<CommonAPI::ClientId> client,
                                         playerPlayReply_t reply) {
    auto result = FidlVmcMediaPlayerInteractionResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerPlay", result);
}


void VmcTestClientAsProvider::playerRewind(const std::shared_ptr<CommonAPI::ClientId> client,
                                           playerRewindReply_t reply) {
    auto result = FidlVmcMediaPlayerInteractionResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerRewind", result);
}

void VmcTestClientAsProvider::playerSeekTo(const std::shared_ptr<CommonAPI::ClientId> client,
                                           const fidlTypes::VmcTimeInMs _newElapsedTime,
                                           const playerSeekToReply_t reply) {
    VMC_LOGI("VmcTestClientAsProvider _newElapsedTime :%u  ", _newElapsedTime);
    auto result = FidlVmcMediaPlayerInteractionResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerSeekTo", result);
}


void VmcTestClientAsProvider::playerSkipToNext(const std::shared_ptr<CommonAPI::ClientId> client,
                                               playerSkipToNextReply_t reply) {
    auto result = FidlVmcMediaPlayerInteractionResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerSkipToNext", result);
}

void VmcTestClientAsProvider::playerSkipToPrevious(
    const std::shared_ptr<CommonAPI::ClientId> client, playerSkipToPreviousReply_t reply) {
    auto result = FidlVmcMediaPlayerInteractionResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerSkipToPrevious", result);
}

void VmcTestClientAsProvider::playerStop(const std::shared_ptr<CommonAPI::ClientId> client,
                                         playerStopReply_t reply) {
    auto result = FidlVmcMediaPlayerInteractionResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerStop", result);
}

void VmcTestClientAsProvider::playerSkipToMediaId(const std::shared_ptr<CommonAPI::ClientId> client,
                                                  const fidlTypes::VmcMediaId _mediaId,
                                                  playerSkipToMediaIdReply_t reply) {
    VMC_LOGI("VmcTestClientAsProvider::playerSkipToMediaId called with mediaId: %s",
             _mediaId.c_str());
    auto result = fidlTypes::VmcSkipToMediaIdResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerSkipToMediaId", result);
}

void VmcTestClientAsProvider::playerPlayFromMediaId(
    const std::shared_ptr<CommonAPI::ClientId> client, const fidlTypes::VmcMediaId _mediaId,
    playerPlayFromMediaIdReply_t reply) {
    VMC_LOGI("VmcTestClientAsProvider::playerPlayFromMediaId called with mediaId: %s",
             _mediaId.c_str());
    auto result = fidlTypes::VmcPlayFromMediaIdResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-playerPlayFromMediaId", result);
}

void VmcTestClientAsProvider::browserAcknowledgeKeepAlive(
    const std::shared_ptr<CommonAPI::ClientId> client, const fidlTypes::VmcViewId _viewId,
    browserAcknowledgeKeepAliveReply_t reply) {
    VMC_LOGI("browserAcknowledgeKeepAlive called with viewId: %u",
             static_cast<unsigned int>(_viewId));
    auto result = fidlTypes::VmcAcknowledgeKeepAliveResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-browserAcknowledgeKeepAlive", result);
}

void VmcTestClientAsProvider::browserDestroyView(const std::shared_ptr<CommonAPI::ClientId> client,
                                                 const fidlTypes::VmcViewId _viewId,
                                                 browserDestroyViewReply_t reply) {
    VMC_LOGI("browserDestroyView called with viewId: %u", static_cast<unsigned int>(_viewId));
    auto result = fidlTypes::VmcDestroyViewResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-browserDestroyView", result);
}

void VmcTestClientAsProvider::browserMoveViewToItem(
    const std::shared_ptr<CommonAPI::ClientId> client, const fidlTypes::VmcViewId viewId,
    ::fidlTypes::VmcMediaId _mediaId, ::fidlTypes::VmcSelectedMediaItemUnion _selectedMediaItem,
    browserMoveViewToItemReply_t reply) {
    VMC_LOGI("browserMoveViewToItem called with viewId: %u, mediaId: %s",
             static_cast<unsigned int>(viewId), _mediaId.c_str());

    if (_selectedMediaItem.isType<fidlTypes::VmcPosition>()) {
        auto val = _selectedMediaItem.get<fidlTypes::VmcPosition>();
        VMC_LOGI("_selectedMediaItem: %u", val);
    } else if (_selectedMediaItem.isType<fidlTypes::VmcMediaId>()) {
        auto val = _selectedMediaItem.get<fidlTypes::VmcMediaId>();
        VMC_LOGI("_selectedMediaItem: %s", val.c_str());
    }

    auto result = fidlTypes::VmcMoveViewToItemResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-browserMoveViewToItem", result);
}

void VmcTestClientAsProvider::browserResizeWindow(const std::shared_ptr<CommonAPI::ClientId> client,
                                                  const fidlTypes::VmcViewId _viewId,
                                                  const fidlTypes::VmcWindowSize _newWindowSize,
                                                  browserResizeWindowReply_t reply) {
    VMC_LOGI("browserResizeWindow called with viewId: %u, newWindowSize: %u",
             static_cast<unsigned int>(_viewId), static_cast<unsigned int>(_newWindowSize));
    auto result = fidlTypes::VmcResizeWindowResult::NO_ERROR;
    genericFunction(client, reply, "ConsumerTestCase-browserResizeWindow", result);
}

void VmcTestClientAsProvider::browserMoveWindow(
    const std::shared_ptr<CommonAPI::ClientId> client, const fidlTypes::VmcViewId _viewId,
    fidlTypes::VmcSelectedMediaItemUnion _selectedMediaItem, browserMoveWindowReply_t reply) {
    VMC_LOGI("browserMoveWindow called with viewId: %u", static_cast<unsigned int>(_viewId));

    if (_selectedMediaItem.isType<fidlTypes::VmcPosition>()) {
        auto val = _selectedMediaItem.get<fidlTypes::VmcPosition>();
        VMC_LOGI("_selectedMediaItem: %u", val);
    } else if (_selectedMediaItem.isType<fidlTypes::VmcMediaId>()) {
        auto val = _selectedMediaItem.get<fidlTypes::VmcMediaId>();
        VMC_LOGI("_selectedMediaItem: %s", val.c_str());
    }

    auto result = fidlTypes::VmcMoveWindowResult::NO_ERROR;

    genericFunction(client, reply, "ConsumerTestCase-browserMoveWindow", result);
}

void VmcTestClientAsProvider::generalGetIcon(const std::shared_ptr<CommonAPI::ClientId> client,
                                             const fidlTypes::VmcIconInformation _iconInformation,
                                             generalGetIconReply_t reply) {
    (void)_iconInformation;

    auto result = FidlVmcGetIconResult::NO_ERROR;

    std::vector<uint8_t> rawData = {0x99, 0x99};
    FidlVmcIcon icon(rawData, _iconInformation);
    VMC_LOGI("ConsumerTestCase-generalGetIcon     =================== PASS.");
    reply(icon, result);
}

void VmcTestClientAsProvider::generalGetParameters(
    const std::shared_ptr<CommonAPI::ClientId> client, std::vector<std::string> _keys,
    generalGetParametersReply_t reply) {
    auto parameterSize = _keys.size();
    VMC_LOGI(
        "ConsumerTestCase-generalGetParameters : %s    =================== PASS. with size %zu ",
        __FUNCTION__, parameterSize);

    auto result = FidlVmcGetParametersResult::NO_ERROR;

    FidlVmcKeyValuePair valuePair;

    std::vector<FidlVmcKeyValuePair> vmcKeyValuePairVector;

    for (auto i = 0; i < parameterSize; i++) {
        valuePair.setKey(_keys[i]);
        valuePair.setValue(_keys[i]);
        vmcKeyValuePairVector.push_back(valuePair);
        VMC_LOGV("valuePair  %s,  %s", vmcKeyValuePairVector[i].getKey().c_str(),
                 vmcKeyValuePairVector[i].getValue().c_str());
    }

    reply(vmcKeyValuePairVector, result);
}

void VmcTestClientAsProvider::generalSetParameters(
    const std::shared_ptr<CommonAPI::ClientId> client,
    const std::vector<fidlTypes::VmcKeyValuePair> _inputParameters,
    const generalSetParametersReply_t reply) {
    size_t inPutVectorSize = _inputParameters.size();
    VMC_LOGI("ConsumerTestCase-generalSetParameters  : %s   =================== PASS.",
             __FUNCTION__);
    auto result = FidlVmcSetParametersResult::NO_ERROR;
    std::vector<FidlVmcKeyValuePair> vmcKeyValuePairVector = (_inputParameters);

    VMC_LOGV("generalSetParameters vector val start:{");
    for (auto i = 0; i < inPutVectorSize; i++) {
        VMC_LOGV(" value pairs %s, %s", vmcKeyValuePairVector[i].getKey().c_str(),
                 vmcKeyValuePairVector[i].getValue().c_str());
    }
    VMC_LOGV("} generalSetParameters vector val end");

    reply(vmcKeyValuePairVector, result);
}

void VmcTestClientAsProvider::browserCreateView(
    const std::shared_ptr<CommonAPI::ClientId> client, ::fidlTypes::VmcSourceId _sourceId,
    ::fidlTypes::VmcWindowSize _windowSize,
    ::fidlTypes::VmcSelectedMediaItemUnion _selectedMediaItem, browserCreateViewReply_t reply) {
    VMC_LOGI("ConsumerTestCase18 : %s    =================== PASS.", __FUNCTION__);
    VMC_LOGI("VmcTestClientAsProvider::browserCreateView called with sourceId: %s,windowSize: %u",
             _sourceId.c_str(), static_cast<unsigned int>(_windowSize));
    if (_selectedMediaItem.isType<fidlTypes::VmcPosition>()) {
        auto val = _selectedMediaItem.get<fidlTypes::VmcPosition>();
        VMC_LOGI("_selectedMediaItem: %u", val);
    } else if (_selectedMediaItem.isType<fidlTypes::VmcMediaId>()) {
        auto val = _selectedMediaItem.get<fidlTypes::VmcMediaId>();
        VMC_LOGI("_selectedMediaItem: %s", val.c_str());
    }

    // std::srand(std::time(nullptr));
    // auto generatedViewId = static_cast<fidlTypes::VmcViewId>(std::rand());

    auto generatedViewId =  static_cast<fidlTypes::VmcViewId>(129);  // use a magic number for easy test.


    auto result = fidlTypes::VmcCreateViewResult::NO_ERROR;

    reply(generatedViewId, result);

    VMC_LOGI("VmcTestClientAsProvider::browserCreateView completed with viewId: %u, result: %d",
             static_cast<unsigned int>(generatedViewId), static_cast<int>(result));
}