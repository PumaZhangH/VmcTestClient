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
* @file   VmcTestClientAsConsumer.cpp
* @author Puma Zhang <Puma.Zhang@harman.com>
********************************************************************************
*/

#define LOG_TAG "VmcTestClientAsConsumer"

#include "VmcTestClientAsConsumer.h"

#include <sys/types.h>
#include <unistd.h>

#include <CommonAPI/CommonAPI.hpp>

#include "VehicleMediaControlLog.h"
#include "VehicleMediaControlRemoteProxy.hpp"
#include "v3/BMW/INFOTAINMENT/ENTERTAINMENT/VMC/VehicleMediaControlInstanceIds.hpp"

#define RETURN_IF_NULL(ptr)            \
    do {                               \
        if ((ptr) == nullptr) {        \
            VMC_LOGE(#ptr " is null"); \
            return;                    \
        }                              \
    } while (0)

#define RETURN_VAL_IF_NULL(ptr, val)   \
    do {                               \
        if ((ptr) == nullptr) {        \
            VMC_LOGE(#ptr " is null"); \
            return (val);              \
        }                              \
    } while (0)

VmcTestClientAsConsumer::VmcTestClientAsConsumer()
    : mCallStatus(CommonAPI::CallStatus::UNKNOWN) {
    VMC_LOGI(" runProxy.");
    runProxy("prox1");
}

void VmcTestClientAsConsumer::runProxy(std::string name) {
    mVehicleMediaControlProxy =
        vendor::bmw::vehiclemediacontrol::v1_0::impl::VehicleMediaControlRemoteProxy().buildProxy(
            "local", FidlVMC::VehicleMediaControl_1);

    RETURN_IF_NULL(mVehicleMediaControlProxy);
    mVehicleMediaControlProxy->getProxyStatusEvent().subscribe([this, name](
                                                                   const CommonAPI::
                                                                       AvailabilityStatus status) {
        if (status == CommonAPI::AvailabilityStatus::AVAILABLE) {
            VMC_LOGI("ProviderTestCase %s is available", name.c_str());
            mVehicleMediaControlProxy->getBrowserQueryKeepAliveEvent().subscribe([name](auto val) {
                VMC_LOGI("ProviderTestCase recv broadcastBrowserQueryKeepAlive change(%d)",
                         static_cast<int32_t>(val));
            });

            mVehicleMediaControlProxy->getPlayerActiveMediaItemTimeAttribute()
                .getChangedEvent()
                .subscribe([name](auto val) {
                    VMC_LOGI(
                        "ProviderTestCase recv getActiveMediaItemTimeAttribute change(%s)  new "
                        "MediaItemTime is elapsed  %d "
                        "in total %d",
                        name.c_str(), val.getElapsedTime(), val.getTotalTime());
                });

            mVehicleMediaControlProxy->getPlayerActiveMediaItemAttribute()
                .getChangedEvent()
                .subscribe([name](auto val) {
                    VMC_LOGI("ProviderTestCase recv ActiveMediaItem change(%s)", name.c_str());
                    FidlVmcMediaItemMetadata vmcMediaItemMetadata = std::move(val);

                    VMC_LOGI(" vmcMediaItemMetadata  isValid   :  %d",
                             vmcMediaItemMetadata.getIsValid());
                    VMC_LOGI(" vmcMediaItemMetadata  iconId    :  %s",
                             vmcMediaItemMetadata.getIconId().c_str());
                    VMC_LOGI(" vmcMediaItemMetadata  displayIconId    :  %s",
                             vmcMediaItemMetadata.getDisplayIcon().c_str());
                    VMC_LOGI(" vmcMediaItemMetadata  contentFormatIcons (large)    :  %s",
                             vmcMediaItemMetadata.getContentFormatIcons().getLargeIcon().c_str());
                    VMC_LOGI(" vmcMediaItemMetadata  contentFormatIcons (small)    :  %s",
                             vmcMediaItemMetadata.getContentFormatIcons().getSmallIcon().c_str());

                    auto metadata = vmcMediaItemMetadata.getMetadata();
                    VMC_LOGI(" mediaItem  metadata  : ");
                    VMC_LOGI("  - isBrowsable: %d", metadata.getIsBrowsable());
                    VMC_LOGI("  - isPlayable : %d", metadata.getIsPlayable());
                    VMC_LOGI("  - isExplicit : %d", metadata.getIsExplicit());
                    VMC_LOGI("  - isPublicValue: %d", metadata.getIsPublicValue());

                    std::vector<FidlVmcKeyValuePair> keyValuePairVector =
                        vmcMediaItemMetadata.getKeyValueMap();
                    VMC_LOGI(" vmcMediaItemMetadata  keyValueMap ==");
                    for (const auto& valuePair : keyValuePairVector) {
                        VMC_LOGI("key value  %s    :     %s  ", valuePair.getKey().c_str(),
                                 valuePair.getValue().c_str());
                    }
                });

            mVehicleMediaControlProxy->getSourceListAttribute().getChangedEvent().subscribe(
                [name](const std::vector<FidlVmcMediaSourceItem>& sourceList) {
                    VMC_LOGI("ProviderTestCase recv sourceList change, size: %zu",
                             sourceList.size());
                    for (const auto& sourceItem : sourceList) {
                        VMC_LOGI("  sourceItem sourceId: %s", sourceItem.getSourceId().c_str());
                        VMC_LOGI("  sourceItem displayName: %s",
                                 sourceItem.getDisplayName().c_str());
                        VMC_LOGI("  sourceItem iconId: %s", sourceItem.getIconId().c_str());
                        VMC_LOGI("  sourceItem secondaryIconId: %s",
                                 sourceItem.getSecondaryIconId().c_str());
                        VMC_LOGI("  sourceItem hideSource: %d",
                                 sourceItem.getHideSource());  // Log hideSource property

                        const std::vector<FidlVmcKeyValuePair>& keyValuePairVector =
                            sourceItem.getKeyValueMap();
                        VMC_LOGI("  sourceItem keyValueMap:");
                        for (const auto& valuePair : keyValuePairVector) {
                            VMC_LOGI("    key: %s, value: %s", valuePair.getKey().c_str(),
                                     valuePair.getValue().c_str());
                        }
                    }
                });

            mVehicleMediaControlProxy->getPlayerPlaybackStateAttribute()
                .getChangedEvent()
                .subscribe([name](auto val) {
                    VMC_LOGI(
                        "ProviderTestCase recv getPlaybackStateAttribute change(%s): new "
                        "PlaybackState: "
                        "@ %s",
                        name.c_str(), val.toString());
                });

            mVehicleMediaControlProxy->getMediaRestrictedModeAttribute()
                .getChangedEvent()
                .subscribe([name](auto val) {
                    auto  restrictedMode = static_cast<int32_t>(val);
                    VMC_LOGI(
                        "ProviderTestCase recv mediaRestrictedMode change(%s): new RestrictedMode: "
                        "%d",
                        name.c_str(), restrictedMode);

                    // Additional logging for clarity
                    if (restrictedMode == 0) {
                        VMC_LOGI("Media is in unrestricted mode.");
                    } else if (restrictedMode == 1) {
                        VMC_LOGI("Media is in restricted mode.");
                    } else {
                        VMC_LOGW("Unexpected mediaRestrictedMode value: %d", restrictedMode);
                    }
                });

            mVehicleMediaControlProxy->getSourceActiveSourcePropertiesAttribute()
                .getChangedEvent()
                .subscribe([name](auto val) {
                    FidlVmcMediaSourceProperties mediaSourceProperties = std::move(val);

                    std::vector<FidlVmcKeyValuePair> keyValuePairVector =
                        mediaSourceProperties.getKeyValueMap();

                    VMC_LOGI(
                        "ProviderTestCase recv getActiveSourcePropertiesAttribute change(%s) new "
                        "ActiveSourceProperties: ",
                        name.c_str());

                    const auto& sourceActions = mediaSourceProperties.getSourceActions();
                    const auto& browserActions = sourceActions.getBrowserActions();
                    const auto& sessionActions = sourceActions.getStandardSessionActions();

                    VMC_LOGI("Browser Actions:");
                    VMC_LOGI("getBrowseRoot: %s", browserActions.getBrowseRoot().c_str());

                    VMC_LOGI("Standard Session Actions:");
                    VMC_LOGI("getIsActionFastForwardSupported: %d",
                             sessionActions.getIsActionFastForwardSupported());
                    VMC_LOGI("getIsActionPauseSupported: %d",
                             sessionActions.getIsActionPauseSupported());
                    VMC_LOGI("getIsActionPlaySupported: %d",
                             sessionActions.getIsActionPlaySupported());
                    VMC_LOGI("getIsActionRewindSupported: %d",
                             sessionActions.getIsActionRewindSupported());
                    VMC_LOGI("getIsActionSeekToSupported: %d",
                             sessionActions.getIsActionSeekToSupported());
                    VMC_LOGI("getIsActionSkipToNextSupported: %d",
                             sessionActions.getIsActionSkipToNextSupported());
                    VMC_LOGI("getIsActionSkipToPreviousSupported: %d",
                             sessionActions.getIsActionSkipToPreviousSupported());
                    VMC_LOGI("getIsActionStopSupported: %d",
                             sessionActions.getIsActionStopSupported());

                    const auto& sourceDisplayPreferences =
                        mediaSourceProperties.getSourceDisplayPreferences();
                    VMC_LOGI("Source Display Preferences:");
                    VMC_LOGI("showPlayPauseAsUnmuteMute: %d",
                             sourceDisplayPreferences.getShowPlayPauseAsUnmuteMute());

                    VMC_LOGI("SourceProperties valuePair ::");
                    for (auto& valuePair : keyValuePairVector) {
                        VMC_LOGI("key  value %s    :     %s  ", valuePair.getKey().c_str(),
                                 valuePair.getValue().c_str());
                    }
                });

            mVehicleMediaControlProxy->getGeneralParametersUpdatedEvent().subscribe(
                [name](std::vector<FidlVmcKeyValuePair> val) {
                    VMC_LOGD("ProviderTestCase recv  broadcastGeneralParametersUpdated(%s):{",
                             name.c_str());
                    for ([[maybe_unused]] auto& v : val) {
                        VMC_LOGD("key(%s),val:(%s)", v.getKey().c_str(), v.getValue().c_str());
                    }
                    VMC_LOGD("} broadcastGeneralParametersUpdated");
                });

            mVehicleMediaControlProxy->getBrowserWindowChangedEvent().subscribe(
                [name](const FidlVmcWindowChangedResult& result) {
                    VMC_LOGI(
                        "ProviderTestCase recv broadcastBrowserWindowChang change for viewID: %d",
                        static_cast<int32_t>(result.getViewID()));

                    VMC_LOGI("  - viewSize: %d", result.getViewSize());
                    VMC_LOGI("  - windowChangedEvent: %d",
                             static_cast<int32_t>(result.getWindowChangedEvent()));
                    VMC_LOGI("  - position: %d", result.getPosition());
                    VMC_LOGI("  - title: %s", result.getTitle().c_str());
                    VMC_LOGI("  - indexOfActiveMedia: %d", result.getIndexOfActiveMedia());

                    const std::vector<FidlVmcMediaItem>& mediaList = result.getMediaList();
                    VMC_LOGI("  - mediaList size: %zu", mediaList.size());
                    for (const auto& mediaItem : mediaList) {
                        VMC_LOGI("    mediaItem mediaId: %s", mediaItem.getMediaId().c_str());
                        const std::vector<FidlVmcKeyValuePair>& keyValuePairVector =
                            mediaItem.getKeyValueMap();
                        VMC_LOGI("    mediaItem keyValueMap:");
                        for (const auto& valuePair : keyValuePairVector) {
                            VMC_LOGI("      key: %s, value: %s", valuePair.getKey().c_str(),
                                     valuePair.getValue().c_str());
                        }
                    }
                });

        } else {
            VMC_LOGE("Proxy not available");
        }
    });
}

bool VmcTestClientAsConsumer::isAvailable() {
    bool vehicleMediaControlHalProxyIsAvailable = mVehicleMediaControlProxy->isAvailable();
    VMC_LOGI("VmcTestClientAsConsumer::isAvailable returned %d ",
             vehicleMediaControlHalProxyIsAvailable);
    return vehicleMediaControlHalProxyIsAvailable;
}

void VmcTestClientAsConsumer::playerSeekTo(uint32_t newElapsedTime, const std::string& testId,
                                           const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    FidlVmcMediaPlayerInteractionResult result;
    mVehicleMediaControlProxy->playerSeekTo(newElapsedTime, mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::playerSkipToPrevious(const std::string& testId,
                                                   const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    FidlVmcMediaPlayerInteractionResult result;
    mVehicleMediaControlProxy->playerSkipToPrevious(mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::playerSkipToNext(const std::string& testId,
                                               const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    FidlVmcMediaPlayerInteractionResult result;
    mVehicleMediaControlProxy->playerSkipToNext(mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::playerStop(const std::string& testId, const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    FidlVmcMediaPlayerInteractionResult result;
    mVehicleMediaControlProxy->playerStop(mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::sourceConnectTo(std::string _sourceId, const std::string& testId,
                                              const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    fidlTypes::VmcConnectToSourceResult result;
    std::string name = "VmcTestClientAsConsumerProx";
    mVehicleMediaControlProxy->sourceConnectTo(_sourceId, mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] Attempting to connect to source: %s. CallStatus: %d, Result: "
        "%s",
        testId.c_str(), testName.c_str(), _sourceId.c_str(), static_cast<int>(mCallStatus),
        result.toString());
}

void VmcTestClientAsConsumer::playerFastForward(const std::string& testId,
                                                const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    FidlVmcMediaPlayerInteractionResult result;
    mVehicleMediaControlProxy->playerFastForward(mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::playerSkipToMediaId(const std::string& testId,
                                                  const std::string& testName,
                                                  const std::string& mediaId) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    FidlVmcSkipToMediaIdResult result;
    mVehicleMediaControlProxy->playerSkipToMediaId(mediaId, mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::playerPause(const std::string& testId, const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    FidlVmcMediaPlayerInteractionResult result;
    mVehicleMediaControlProxy->playerPause(mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::playerPlay(const std::string& testId, const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    FidlVmcMediaPlayerInteractionResult result;
    mVehicleMediaControlProxy->playerPlay(mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::playerRewind(const std::string& testId, const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    FidlVmcMediaPlayerInteractionResult result;
    mVehicleMediaControlProxy->playerRewind(mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::browserMoveWindow(
    const fidlTypes::VmcViewId& viewId,
    const fidlTypes::VmcSelectedMediaItemUnion& selectedMediaItem, const std::string& testId,
    const std::string& testName) {
    VMC_LOGI("input viewId %d", viewId);
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    fidlTypes::VmcMoveWindowResult result;

    // Call the browserMoveWindow method on the proxy
    mVehicleMediaControlProxy->browserMoveWindow(viewId, selectedMediaItem, mCallStatus, result,
                                                 nullptr);
    VMC_LOGI("ProviderTestCase [%s - %s] Moving window CallStatus: %d, Result: %s\n",
             testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::playerPlayFromMediaId(const std::string& testId,
                                                    const std::string& mediaId,
                                                    const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    // Assuming that the media ID is of the type VmcMediaId
    fidlTypes::VmcMediaId vmcMediaId = mediaId;

    // Result type based on the context provided
    fidlTypes::VmcPlayFromMediaIdResult result;

    mVehicleMediaControlProxy->playerPlayFromMediaId(mediaId, mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase[%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::browserAcknowledgeKeepAlive(fidlTypes::VmcViewId viewId,
                                                          const std::string& testId,
                                                          const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    fidlTypes::VmcAcknowledgeKeepAliveResult result;

    const CommonAPI::CallInfo* info = nullptr;

    VMC_LOGI("input viewId %d", viewId);
    mVehicleMediaControlProxy->browserAcknowledgeKeepAlive(viewId, mCallStatus, result, info);
    VMC_LOGI(
        "ProviderTestCase[%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::browserCreateView(
    const fidlTypes::VmcSourceId& sourceId, const fidlTypes::VmcWindowSize& windowSize,
    const fidlTypes::VmcSelectedMediaItemUnion& selectedMediaItem, const std::string& testId,
    const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    fidlTypes::VmcViewId viewId{0};

    fidlTypes::VmcCreateViewResult result;

    const CommonAPI::CallInfo* info = nullptr;

    mVehicleMediaControlProxy->browserCreateView(sourceId, windowSize, selectedMediaItem,
                                                 mCallStatus, viewId, result, info);

    VMC_LOGI(
        "ProviderTestCase[%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::browserDestroyView(fidlTypes::VmcViewId viewId,
                                                 const std::string& testId,
                                                 const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    fidlTypes::VmcDestroyViewResult result;
    mVehicleMediaControlProxy->browserDestroyView(viewId, mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase[%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::browserMoveViewToItem(
    const fidlTypes::VmcViewId& viewId, const fidlTypes::VmcMediaId& mediaId,
    const fidlTypes::VmcSelectedMediaItemUnion& selectedMediaItem, const std::string& testId,
    const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    fidlTypes::VmcMoveViewToItemResult result;

    const CommonAPI::CallInfo* info = nullptr;

    mVehicleMediaControlProxy->browserMoveViewToItem(viewId, mediaId, selectedMediaItem,
                                                     mCallStatus, result, info);

    VMC_LOGI(
        "ProviderTestCase[%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::browserResizeWindow(const fidlTypes::VmcViewId& viewId,
                                                  const fidlTypes::VmcWindowSize& newWindowSize,
                                                  const std::string& testId,
                                                  const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    fidlTypes::VmcResizeWindowResult result;
    mVehicleMediaControlProxy->browserResizeWindow(viewId, newWindowSize, mCallStatus, result);
    VMC_LOGI(
        "ProviderTestCase[%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::generalSetParameters(uint32_t index, const std::string& testId,
                                                   const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    std::vector<FidlVmcKeyValuePair> inputParameters;
    buildPairVectorWithIndex(index, inputParameters);

    fidlTypes::VmcSetParametersResult result;
    std::vector<FidlVmcKeyValuePair> outputParameters;
    mVehicleMediaControlProxy->generalSetParameters(inputParameters, mCallStatus, outputParameters,
                                                    result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
}

void VmcTestClientAsConsumer::generalGetIcon(const std::string& testId,
                                             const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    fidlTypes::VmcGetIconResult result;

    FidlVmcIconInformation vmcIconInformation;
    FidlVmcIcon icon;
    mVehicleMediaControlProxy->generalGetIcon(vmcIconInformation, mCallStatus, icon, result);
    VMC_LOGI(
        "ProviderTestCase [%s - %s] CallStatus =================> : %d(0 is SUCCESS), return  "
        "result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());

    auto iconWidth = icon.getIconInformation().getIconResolution().getWidth();
    auto iconHeight = icon.getIconInformation().getIconResolution().getHeight();
    auto iconData = icon.getIconRawData();

    if (iconData.size() > 0) {
        DUMP_TO_FILE(DumpIconFilePath, iconData.data(), iconData.size());
    }

    VMC_LOGI("iconData size %zu and information width %u and heigh %u IconRawData size %zu",
             iconData.size(), iconWidth, iconHeight, iconData.size());
}

void VmcTestClientAsConsumer::generalGetParameters(const std::string& testId,
                                                   const std::string& testName) {
    RETURN_IF_NULL(mVehicleMediaControlProxy);

    fidlTypes::VmcGetParametersResult result;

    std::vector<std::string> keys;
    keys.emplace_back("key_001");
    keys.emplace_back("key_002");

    std::vector<FidlVmcKeyValuePair> vmcKeyValuePairVector;

    mVehicleMediaControlProxy->generalGetParameters(keys, mCallStatus, vmcKeyValuePairVector,
                                                    result);
    VMC_LOGI(
        "ProviderTestCase[%s - %s] CallStatus =================> with key: %d(0 is SUCCESS), "
        "return  result : %s\n",
        testId.c_str(), testName.c_str(), static_cast<int>(mCallStatus), result.toString());
    if (!vmcKeyValuePairVector.empty()) {
        VMC_LOGI(" generalGetParameters valuePair ::");
        for (auto& valuePair : vmcKeyValuePairVector) {
            VMC_LOGI("key  value   %s    :     %s  ", valuePair.getKey().c_str(),
                     valuePair.getValue().c_str());
        }
    }
}

void VmcTestClientAsConsumer::buildPairVectorWithIndex(
    uint32_t index, std::vector<FidlVmcKeyValuePair>& keyValuePairVector) {
    FidlVmcKeyValuePair pair;
    keyValuePairVector.push_back(pair);

    if (index == 0) {
        // Format                                   : MPEG Audio
        pair.setKey("Format");
        pair.setValue("MPEG Audio");
        keyValuePairVector.push_back(pair);

        // File size                                : 6.29 MiB
        pair.setKey("File size");
        pair.setValue("6.29 MiB");
        keyValuePairVector.push_back(pair);
        // Duration                                 : 3 min 24 s
        pair.setKey("Duration");
        pair.setValue("3 min 24 s");
        keyValuePairVector.push_back(pair);
        // Overall bit rate mode                    : Constant
        pair.setKey("Overall bit rate mode  ");
        pair.setValue("Constant");
        keyValuePairVector.push_back(pair);

        // Overall bit rate                         : 256 kb/s
        pair.setKey("Overall bit rate   ");
        pair.setValue("256 kb/s");
        keyValuePairVector.push_back(pair);

        // Album                                    : The Fame
        pair.setKey("Album   ");
        pair.setValue("The Fame");
        keyValuePairVector.push_back(pair);

        // Track name                               : i Like It Rough
        pair.setKey("Track name    ");
        pair.setValue("i Like It Rough");
        keyValuePairVector.push_back(pair);

        // Performer                                : Lady Gaga
        pair.setKey("Performer  ");
        pair.setValue("Lady Gaga");
        keyValuePairVector.push_back(pair);

        // Genre                                    : Electronic
        pair.setKey("Genre    ");
        pair.setValue("Electronic");
        keyValuePairVector.push_back(pair);

        // Recorded date                            : 2008-10-28
        pair.setKey("Recorded date      ");
        pair.setValue("2008-10-28");
        keyValuePairVector.push_back(pair);

    } else {
        // Performer                                : 보아
        pair.setKey("Performer");
        pair.setValue("보아");
        keyValuePairVector.push_back(pair);
        // Genre                                    : Ballad
        pair.setKey("Genre");
        pair.setValue("Ballad");
        keyValuePairVector.push_back(pair);

        // Recorded date                            : 2002
        pair.setKey("Recorded date");
        pair.setValue("2002");
        keyValuePairVector.push_back(pair);

        // Copyright                                : LOEN Entertainment, Inc.
        pair.setKey("Copyright");
        pair.setValue("LOEN Entertainment, Inc.");
        keyValuePairVector.push_back(pair);
        // Cover                                    : Yes
        pair.setKey("Cover");
        pair.setValue("Yes");
        keyValuePairVector.push_back(pair);
        // Cover type                               : Cover (front)
        pair.setKey("Cover type  ");
        pair.setValue("Cover (front)");
        keyValuePairVector.push_back(pair);
        // Cover MIME                               : image/jpeg
        pair.setKey("Cover MIME  ");
        pair.setValue("image/jpeg");
        keyValuePairVector.push_back(pair);
        // Comment                                  : http://www.melon.com/
        pair.setKey("Cover MIME  ");
        pair.setValue("image/jpeg");
        keyValuePairVector.push_back(pair);
        // Format                                   : MPEG Audio
        pair.setKey("Format");
        pair.setValue("MPEG Audio");
        keyValuePairVector.push_back(pair);
        // Format version                           : Version 1
        pair.setKey("Format version ");
        pair.setValue("Version 1");
        keyValuePairVector.push_back(pair);
        // Format profile                           : Layer 3
        pair.setKey("Format profile ");
        pair.setValue("Layer 3");
        keyValuePairVector.push_back(pair);
        // Format settings                          : Joint stereo
        pair.setKey("Format settings");
        pair.setValue("Joint stereo");
        keyValuePairVector.push_back(pair);
        // Duration                                 : 3 min 13 s
        pair.setKey("Duration");
        pair.setValue("3 min 13 s");
        keyValuePairVector.push_back(pair);
        // Bit rate mode                            : Constant
        pair.setKey("Bit rate mode  ");
        pair.setValue("Constant");
        keyValuePairVector.push_back(pair);
        // Bit rate                                 : 320 kb/s
        pair.setKey("Bit rate ");
        pair.setValue("320 kb/s");
        keyValuePairVector.push_back(pair);
        // Channel(s)                               : 2 channels
        pair.setKey("Channel(s)");
        pair.setValue(" 2 channels");
        keyValuePairVector.push_back(pair);
        // Sampling rate                            : 44.1 kHz
        pair.setKey("Sampling rate   ");
        pair.setValue("44.1 kHz");
        keyValuePairVector.push_back(pair);
        // Frame rate                               : 38.281 FPS (1152 SPF)
        pair.setKey("Frame rate ");
        pair.setValue("38.281 FPS (1152 SPF)");
        keyValuePairVector.push_back(pair);
    }
}

void VmcTestClientAsConsumer::buildVmcMediaSourceItemByName(
    std::string inputName, std::vector<FidlVmcMediaSourceItem>& mediaSourceItemVector) {
    // Create two media source items
    FidlVmcMediaSourceItem sourceItem1;
    FidlVmcMediaSourceItem sourceItem2;

    // Vector to hold key-value pairs
    std::vector<FidlVmcKeyValuePair> keyValuePairVector;

    // Populate the first media source item
    sourceItem1.setDisplayName(inputName);
    sourceItem1.setSourceId("008");
    buildPairVectorWithIndex(0, keyValuePairVector);
    sourceItem1.setKeyValueMap(keyValuePairVector);
    sourceItem1.setIconId("primaryIconId1");             // Example primary icon ID
    sourceItem1.setSecondaryIconId("secondaryIconId1");  // Example secondary icon ID
    sourceItem1.setHideSource(false);                    // Set hideSource to false

    // Clear the key-value pair vector for reuse
    keyValuePairVector.clear();

    // Populate the second media source item
    sourceItem2.setDisplayName(inputName + "2");
    sourceItem2.setSourceId("009");
    buildPairVectorWithIndex(1, keyValuePairVector);
    sourceItem2.setKeyValueMap(keyValuePairVector);
    sourceItem2.setIconId("primaryIconId2");             // Example primary icon ID
    sourceItem2.setSecondaryIconId("secondaryIconId2");  // Example secondary icon ID
    sourceItem2.setHideSource(true);                     // Set hideSource to true

    // Add the media source items to the vector
    mediaSourceItemVector.push_back(sourceItem1);
    mediaSourceItemVector.push_back(sourceItem2);
}

void VmcTestClientAsConsumer::buildVmcMediaSourceProperties(
    FidlVmcMediaSourceProperties& mediaSourceProperties, const std::string& browseRoot) {
    // Example data for VmcKeyValuePair
    std::vector<FidlVmcKeyValuePair> keyValuePairs = {FidlVmcKeyValuePair("key1", "value1"),
                                                      FidlVmcKeyValuePair("key2", "value2")};

    // Example data for VmcSourceId
    FidlVmcSourceId sourceId = "exampleSourceId";

    // Example data for VmcMediaBrowserActions
    FidlVmcMediaId vmcBrowseRoot = browseRoot;

    FidlVmcMediaBrowserActions mediaBrowserActions(vmcBrowseRoot,
                                                   true);  // Assuming containsPlaylist is true

    // Example data for VmcStandardSessionActions
    FidlVmcStandardSessionActions standardSessionActions;
    standardSessionActions.setIsActionFastForwardSupported(true);
    standardSessionActions.setIsActionPauseSupported(false);
    standardSessionActions.setIsActionPlaySupported(true);
    standardSessionActions.setIsActionRewindSupported(false);
    standardSessionActions.setIsActionSeekToSupported(true);
    standardSessionActions.setIsActionSkipToNextSupported(false);
    standardSessionActions.setIsActionSkipToPreviousSupported(true);
    standardSessionActions.setIsActionStopSupported(false);

    // Example data for VmcSourceActions
    FidlVmcSourceActions sourceActions(mediaBrowserActions, standardSessionActions);

    // Example data for VmcSourceDisplayPreferences
    FidlVmcSourceDisplayPreferences sourceDisplayPreferences;
    sourceDisplayPreferences.setShowPlayPauseAsUnmuteMute(true);  // Set the flag as needed

    // Populate the mediaSourceProperties struct
    mediaSourceProperties = FidlVmcMediaSourceProperties(keyValuePairs, sourceId, sourceActions,
                                                         sourceDisplayPreferences);
}

void VmcTestClientAsConsumer::buildVmcMediaItemMetadata(FidlVmcMediaItemMetadata& mediaItemMetadata,
                                                        const std::string& displayIconId) {
    // Example data for FidlVmcKeyValuePair
    std::vector<FidlVmcKeyValuePair> keyValuePairs = {FidlVmcKeyValuePair("key1", "value1"),
                                                      FidlVmcKeyValuePair("key2", "value2")};

    // Example data for validity flag
    bool isValid = true;

    std::string iconId = "iconId";

    // Create an instance of FidlVmcMetadata
    FidlVmcMetadata metadata;

    // Example data for metadata key-value pairs
    std::vector<FidlVmcMetadataKeyValuePair> metadataKeyValuePairs;

    // Create and populate FidlVmcMetadataKeyValuePair instances
    FidlVmcMetadataKeyValuePair titlePair;
    titlePair.setKey(FidlVmcMetadataKey(FidlVmcMetadataKey::TITLE));
    titlePair.setValue("Example Title");

    FidlVmcMetadataKeyValuePair artistPair;
    artistPair.setKey(FidlVmcMetadataKey(FidlVmcMetadataKey::ARTIST));
    artistPair.setValue("Example Artist");

    // Add the key-value pairs to the vector
    metadataKeyValuePairs.push_back(titlePair);
    metadataKeyValuePairs.push_back(artistPair);

    // Example data for boolean flags
    bool isBrowsable = true;
    bool isPlayable = true;
    bool isExplicit = false;
    bool isPublicValue = true;

    // Populate the metadata struct using set functions
    metadata.setMetadataKeyValue(metadataKeyValuePairs);
    metadata.setIsBrowsable(isBrowsable);
    metadata.setIsPlayable(isPlayable);
    metadata.setIsExplicit(isExplicit);
    metadata.setIsPublicValue(isPublicValue);

    // Example data for VmcContentFormatIcons
    FidlVmcContentFormatIcons contentFormatIcons("largeIconId", "smallIconId");

    // Populate the mediaItemMetadata struct
    mediaItemMetadata = FidlVmcMediaItemMetadata(keyValuePairs, isValid, iconId, metadata,
                                                 contentFormatIcons, displayIconId);
}