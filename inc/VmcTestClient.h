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
* @file   VmcTestClient.h
* @author Puma Zhang <Puma.Zhang@harman.com>
********************************************************************************
*/

#pragma once

#include <BMW/INFOTAINMENT/ENTERTAINMENT/VMC/gen_BMW_INFOTAINMENT_ENTERTAINMENT_VMC__Types.hpp>
#include <v3/BMW/INFOTAINMENT/ENTERTAINMENT/VMC/VehicleMediaControlProxy.hpp>

namespace FidlVMC = v3::BMW::INFOTAINMENT::ENTERTAINMENT::VMC;

using fidlTypes =
    ::BMW::INFOTAINMENT::ENTERTAINMENT::VMC::gen_BMW_INFOTAINMENT_ENTERTAINMENT_VMC__Types;

using FidlVmcSourceId = fidlTypes::VmcSourceId;
using FidlVmcConnectToSourceResult = fidlTypes::VmcConnectToSourceResult;
using FidlVmcMediaPlayerInteractionResult = fidlTypes::VmcMediaPlayerInteractionResult;
using FidlVmcIconInformation = fidlTypes::VmcIconInformation;
using FidlVmcGetIconResult = fidlTypes::VmcGetIconResult;
using FidlVmcSkipToMediaIdResult = fidlTypes::VmcSkipToMediaIdResult;
using FidlVmcIcon = fidlTypes::VmcIcon;
using FidlVmcGetParametersResult = fidlTypes::VmcGetParametersResult;
using FidlVmcKeyValuePair = fidlTypes::VmcKeyValuePair;
using FidlVmcTimeInMs = fidlTypes::VmcTimeInMs;
using FidlVmcWindowChangedResult = fidlTypes::VmcWindowChangedResult;
using FidlVmcCreateViewResult = fidlTypes::VmcCreateViewResult;
using FidlVmcSetParametersResult = fidlTypes::VmcSetParametersResult;
using FidlVmcMediaItemMetadata = fidlTypes::VmcMediaItemMetadata;
using FidlVmcMetadata = fidlTypes::VmcMetadata;
using FidlVmcMediaItem = fidlTypes::VmcMediaItem;
using FidlVmcMediaSourceItem = fidlTypes::VmcMediaSourceItem;
using FidlVmcMediaSourceProperties = fidlTypes::VmcMediaSourceProperties;
using FidlVmcMediaBrowserActions = fidlTypes::VmcMediaBrowserActions;
using FidlVmcStandardSessionActions = fidlTypes::VmcStandardSessionActions;
using FidlVmcSourceActions = fidlTypes::VmcSourceActions;
using FidlVmcMetadataKeyValuePair = fidlTypes::VmcMetadataKeyValuePair;
using FidlVmcMetadataKey = fidlTypes::VmcMetadataKey;
using FidlVmcViewSize = fidlTypes::VmcViewSize;
using FidlVmcMediaItemTime = fidlTypes::VmcMediaItemTime;
using FidlVmcPlaybackState = fidlTypes::VmcPlaybackState;
using FidlVmcMediaId = fidlTypes::VmcMediaId;
using FidlVmcContentFormatIcons = fidlTypes::VmcContentFormatIcons;
using FidlVmcSourceDisplayPreferences = fidlTypes::VmcSourceDisplayPreferences;
using FidlVmcMediaRestrictedMode = fidlTypes::VmcMediaRestrictedMode;

uint32_t primaryLoop(const std::string& instanceID);
void signalExit();
void displayPrompt(const std::string& instanceID, const std::string& prompt);

static const std::string Prompt = R"(
----------------VmcTestClient as consumer ---------------------------------------------------------------------------
                        0.isAvailable                               13.playerPlayFromMediaId
                        1.sourceConnectTo                           14.browserAcknowledgeKeepAlive
                        2.playerFastForward                         15.browserCreateView
                        3.generalGetIcon                            16.browserDestroyView
                        4.generalGetParameters                      17.browserMoveWindow
                        5.playerPause                               18.browserResizeWindow
                        6.playerPlay                                19.browserMoveViewToItem
                        7.playerRewind                              20.playerSkipToMediaId
                        8.playerSeekTo
                        9.generalSetParameters
                        10.playerSkipToPrevious
                        11.playerSkipToNext
                        12.playerStop

----------------VmcTestClient as provider -------------------------------------------------------------------------
                        21.setPlayerPlaybackStateAttribute
                        22.setPlayerActiveMediaItemAttribute
                        23.setPlayerActiveMediaItemTimeAttribute
                        24.setSourceActiveSourcePropertiesAttribute
                        25.setSourceListAttribute
                        26.broadcastGeneralParametersUpdated
                        27.broadcastBrowserQueryKeepAliveEvent
                        28.broadcastBrowserWindowChangedEvent
                        29.setMediaRestrictedModeattribute

                        999.exit

----------------End of Menu  -----------------------------------------------------------------------------------

                        )";
