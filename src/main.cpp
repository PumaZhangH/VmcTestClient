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
* @file main.cpp
* @author Puma Zhang <puma.zhang@harman.com>
********************************************************************************
*/

#include <sys/types.h>
#include <unistd.h>

#include <CommonAPI/CommonAPI.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "VehicleMediaControlRemoteService.hpp"
#include "VmcTestClient.h"
#include "VmcTestClientAsConsumer.h"
#include "VmcTestClientAsProvider.h"
#include "v3/BMW/INFOTAINMENT/ENTERTAINMENT/VMC/VehicleMediaControlInstanceIds.hpp"

namespace po = boost::program_options;

std::mutex exitMutex;
std::condition_variable exitCondition;
bool ready_to_exit = false;

static const constexpr int kExit = 999;

static std::shared_ptr<VmcTestClientAsConsumer> gTestClientProx;
static std::shared_ptr<VmcTestClientAsProvider> gTestClientStub;

struct Command {
    std::string id;
    std::string name;
    std::function<void(const po::variables_map&)> func;
    std::string description;
};

#define SAFE_CALL_STUB(method, ...)           \
    if (gTestClientStub != nullptr) {         \
        gTestClientStub->method(__VA_ARGS__); \
    } else {                                  \
        VMC_LOGI("unable to " #method);       \
    }

void runStub(const std::string& instanceID) {
    std::string instance = instanceID;
    VMC_LOGI("VmcTestClient registerService As Provider with instance ID : %s", instance.c_str());
    gTestClientStub = std::make_shared<VmcTestClientAsProvider>(instance);
    auto stubService =
        vendor::bmw::vehiclemediacontrol::v1_0::impl::VehicleMediaControlRemoteService(
            gTestClientStub, "local", instance);
    VMC_LOGE_IF(!stubService.isRegistered(), "registerService(instance:%s) error",
                instance.c_str());

    // Lock mutex before waiting
    std::unique_lock<std::mutex> lock(exitMutex);
    exitCondition.wait(lock,
                       [] { return ready_to_exit; });  // Wait until 'ready_to_exit' becomes true

    VMC_LOGI("Exiting the Thread. ");
}

static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time);

    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &now_tm);
    return std::string(buffer);
}
int main(int argc, char* argv[]) {
    std::string inputInstance;

    std::string buildMessage =
        "Welcome to test Vmc, Build on " + getCurrentTimestamp() + " Allowed options:";
    po::options_description desc(buildMessage);

    desc.add_options()("help,h", "Show help message. Ex: ./VmcTestClient --RSE ")(
        "HU,1", "Set provider as HU")("RSE,2", "Set provider as RSE")("CDE,3",
                                                                      "Set provider as CDE");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    if (vm.count("HU")) {
        std::cout << "VehicleMediaControl_1 is used by consumer." << std::endl;
        inputInstance = FidlVMC::VehicleMediaControl_1;
    } else if (vm.count("RSE")) {
        inputInstance = FidlVMC::VehicleMediaControl_2;
    } else if (vm.count("CDE")) {
        inputInstance = FidlVMC::VehicleMediaControl_3;
    } else {
        std::cerr << "Unknown option. Use --help to see available options." << std::endl;
        return 1;
    }

    auto stubTask = std::async(std::launch::async, runStub, inputInstance);

    // Bind the instanceID argument to primaryLoop
    auto taskFunc = [inputInstance]() -> uint32_t { return primaryLoop(inputInstance); };

    // Wrap the bound function in a packaged_task
    std::packaged_task<void()> task(taskFunc);

    // Get the future associated with the packaged_task
    auto primaryLoopFuture = task.get_future();

    // Start the task in a separate thread
    std::thread primaryLoopThread(std::move(task));

    // Use the future to wait for the task to complete
    primaryLoopFuture.wait();

    signalExit();

    // Join the thread (always check if joinable to avoid exceptions)
    if (primaryLoopThread.joinable()) {
        primaryLoopThread.join();
    }

    stubTask.wait();

    VMC_LOGI("Exiting the main thread. ");
    return 0;
}

static Command cmdList[] = {
    {"0", "isAvailable", [](const po::variables_map& vm) { gTestClientProx->isAvailable(); },
     "Check if provider is available"},
    {"1", "sourceConnectTo",
     [](const po::variables_map& vm) {
         VMC_LOGI(
             "VmcTestClientAsConsumer:: input the source id. Empty String is reserved as "
             "INVALID sourceId. MaxLength = 3232 Bit (UTF16D incl. special characters)");
         std::string sourceId;
         std::cin >> sourceId;
         gTestClientProx->sourceConnectTo(sourceId, "1", "sourceConnectTo");
     },
     "Allows connecting to a specific media source."},
    {"2", "playerFastForward",
     [](const po::variables_map& vm) {
         gTestClientProx->playerFastForward("2", "playerFastForward");
     },
     "Forward action \"fastForward\" directly to source"},
    {"3", "generalGetIcon",
     [](const po::variables_map& vm) { gTestClientProx->generalGetIcon("3", "generalGetIcon"); },
     "Fetch image from media module cache"},
    {"4", "generalGetParameters",
     [](const po::variables_map& vm) {
         gTestClientProx->generalGetParameters("4", "generalGetParameters");
     },
     "Generic method for retrieving vendor-specific parameter values"},
    {"5", "playerPause",
     [](const po::variables_map& vm) { gTestClientProx->playerPause("5", "playerPause"); },
     "Forward action \"pause\" directly to source."},
    {"6", "playerPlay",
     [](const po::variables_map& vm) { gTestClientProx->playerPlay("6", "playerPlay"); },
     "Forward action \"play\" directly to source."},
    {"7", "playerRewind",
     [](const po::variables_map& vm) { gTestClientProx->playerRewind("7", "playerRewind"); },
     "Forward action \"rewind\" directly to source."},
    {"8", "playerSeekTo",
     [](const po::variables_map& vm) {
         VMC_LOGI("playerSeekTo: ValidRange: [0, UInt32_MAX]. Unit: milliseconds [ms]");
         uint32_t val;
         std::cin >> val;
         gTestClientProx->playerSeekTo(val, "8", "playerSeekTo");
     },
     "Allows to modify the elapsedTime of the currently active media."},
    {"9", "generalSetParameters",
     [](const po::variables_map& vm) {
         VMC_LOGI("generalSetParameters: ValidRange: [0, 1] (@0 for Lady Gaga, @others for 보아)");
         uint32_t index;
         std::cin >> index;
         gTestClientProx->generalSetParameters(index, "9", "generalSetParameters");
     },
     "Generic method for setting vendor-specific parameter values."},
    {"10", "playerSkipToPrevious",
     [](const po::variables_map& vm) {
         gTestClientProx->playerSkipToPrevious("10", "playerSkipToPrevious");
     },
     "Forward action \"skipToPrevious\" directly to source."},
    {"11", "playerSkipToNext",
     [](const po::variables_map& vm) {
         gTestClientProx->playerSkipToNext("11", "playerSkipToNext");
     },
     "Forward action \"skipToNext\" directly to source."},
    {"12", "playerStop",
     [](const po::variables_map& vm) { gTestClientProx->playerStop("12", "playerStop"); },
     "Forward action \"stop\" directly to source."},
    {"13", "playerPlayFromMediaId",
     [](const po::variables_map& vm) {
         VMC_LOGI("playerPlayFromMediaId: input the mediaId by string:");
         std::string mediaId;
         std::cin >> mediaId;
         gTestClientProx->playerPlayFromMediaId(mediaId, "13", "playerPlayFromMediaId");
     },
     "Request active source to play specific mediaId."},
    {"14", "browserAcknowledgeKeepAlive",
     [](const po::variables_map& vm) {
         VMC_LOGI("browserAcknowledgeKeepAlive: input the viewId by uint8_t:");
         uint32_t viewId;
         std::cin >> viewId;
         if (viewId < std::numeric_limits<uint8_t>::min() ||
             viewId > std::numeric_limits<uint8_t>::max()) {
             VMC_LOGW("browserDestroyView: use the default viewId = 1 as input.");
             viewId = 1;
         }
         gTestClientProx->browserAcknowledgeKeepAlive(viewId, "14", "browserAcknowledgeKeepAlive");
     },
     "Informs the server that the provided viewId has to be kept alive."},
    {"15", "browserCreateView",
     [](const po::variables_map& vm) {
         VMC_LOGI("browserCreateView: select the windowSize by uint16_t:");
         uint32_t windowSizeIn;
         std::cin >> windowSizeIn;
         if (windowSizeIn >= std::numeric_limits<uint16_t>::min() &&
             windowSizeIn <= std::numeric_limits<uint16_t>::max()) {
             windowSizeIn = static_cast<uint16_t>(windowSizeIn);
         } else {
             VMC_LOGW("browserCreateView: use the default windowSize = 1 as input.");
             windowSizeIn = 1;
         }
         std::string sourceId = "15";           // Example source ID
         std::string selectedMediaItem = "15";  // Example selected media item
         gTestClientProx->browserCreateView(sourceId, windowSizeIn, selectedMediaItem, "15",
                                            "browserCreateView");
     },
     "Creates an empty view with a (sliding) window on top of it."},
    {"16", "browserDestroyView",
     [](const po::variables_map& vm) {
         VMC_LOGI("browserDestroyView: input the viewId by uint8_t:");
         uint32_t viewId;
         std::cin >> viewId;
         if (viewId < std::numeric_limits<uint8_t>::min() ||
             viewId > std::numeric_limits<uint8_t>::max()) {
             VMC_LOGW("browserDestroyView: use the default viewId = 1 as input.");
             viewId = 1;
         }
         gTestClientProx->browserDestroyView(viewId, "16", "browserDestroyView");
     },
     "Destroys an existing view."},
    {"17", "browserMoveWindow",
     [](const po::variables_map& vm) {
         VMC_LOGI("browserMoveWindow: input the viewId by uint8_t:");
         uint32_t viewId;
         std::cin >> viewId;
         if (viewId >= std::numeric_limits<uint8_t>::min() &&
             viewId <= std::numeric_limits<uint8_t>::max()) {
             viewId = static_cast<uint8_t>(viewId);
         } else {
             VMC_LOGW("browserMoveWindow: use the default viewId = 1 as input.");
             viewId = 1;
         }
         std::string selectedMediaItem = "17";  // Example selected media item
         gTestClientProx->browserMoveWindow(viewId, selectedMediaItem, "17", "browserMoveWindow");
     },
     "Move the (sliding) window to a specific media item."},
    {"18", "browserResizeWindow",
     [](const po::variables_map& vm) {
         VMC_LOGI(
             "browserResizeWindow: input the viewId by uint8_t, windowSize with default value 128");
         uint32_t viewId;
         std::cin >> viewId;
         static const constexpr int kDefaultWindowSize = 128U;
         if (viewId >= std::numeric_limits<uint8_t>::min() &&
             viewId <= std::numeric_limits<uint8_t>::max()) {
             viewId = static_cast<uint8_t>(viewId);
         } else {
             VMC_LOGW("browserResizeWindow: use the default viewId = 1 as input.");
             viewId = 1;
         }
         gTestClientProx->browserResizeWindow(viewId, kDefaultWindowSize, "18",
                                              "browserResizeWindow");
     },
     "Resize the (sliding) window of the specified view."},
    {"19", "browserMoveViewToItem",
     [](const po::variables_map& vm) {
         VMC_LOGI("browserMoveViewToItem: input the viewId by uint8_t:");
         uint32_t viewId;
         std::cin >> viewId;
         if (viewId >= std::numeric_limits<uint8_t>::min() &&
             viewId <= std::numeric_limits<uint8_t>::max()) {
             viewId = static_cast<uint8_t>(viewId);
         } else {
             VMC_LOGW("browserMoveViewToItem: use the default viewId = 1 as input.");
             viewId = 1;
         }
         fidlTypes::VmcMediaId mediaId = "19";  // Example media ID
         fidlTypes::VmcSelectedMediaItemUnion selectedMediaItem;
         uint32_t position = 19;
         selectedMediaItem = static_cast<fidlTypes::VmcPosition>(position);
         gTestClientProx->browserMoveViewToItem(viewId, mediaId, selectedMediaItem, "19",
                                                "browserMoveViewToItem");
     },
     "Moves an existing view to a new parent in the same source as the existing view"},
    {"20", "playerSkipToMediaId",
     [](const po::variables_map& vm) {
         VMC_LOGI("playerSkipToMediaId: input the mediaId by string:");
         std::string mediaId;
         std::cin >> mediaId;
         gTestClientProx->playerSkipToMediaId(mediaId, "20", "playerSkipToMediaId");
     },
     "Skip to a specific media ID."},
    {"21", "setPlayerPlaybackStateAttribute",
     [](const po::variables_map& vm) {
         VMC_LOGI("setPlayerPlaybackStateAttribute: ValidRange: [0, 3]");
         int playbackState;
         std::cin >> playbackState;
         SAFE_CALL_STUB(setPlayerPlaybackStateAttribute,
                        FidlVmcPlaybackState::Literal(playbackState % 4));
     },
     "Set playback state attribute"},

    {"22", "setPlayerActiveMediaItemAttribute",
     [](const po::variables_map& vm) {
         VMC_LOGI(
             "setPlayerActiveMediaItemAttribute: Collection of all metadata information used for "
             "describing a media item.");

         // Get displayIconId from user input
         std::string displayIconId;
         std::cout << "Enter displayIconId: ";
         std::cin >> displayIconId;

         FidlVmcMediaItemMetadata mediaItem;
         gTestClientProx->buildVmcMediaItemMetadata(mediaItem, displayIconId);
         SAFE_CALL_STUB(setPlayerActiveMediaItemAttribute, mediaItem);
     } ,"Set active media item attribute"},

    {"23", "setPlayerActiveMediaItemTimeAttribute",
     [](const po::variables_map& vm) {
         VMC_LOGI(
             "setPlayerActiveMediaItemTimeAttribute: input the elapsed time [MIN = 0, MAX = "
             "UInt32_MAX, OFFSET = 0, RESOLUTION = 1]. Unit: milliseconds [ms].");
         float elapsedTime;
         std::cin >> elapsedTime;
         FidlVmcMediaItemTime mediaItemTime;
         mediaItemTime.setElapsedTime(elapsedTime);
         mediaItemTime.setTotalTime(UINT32_MAX);
         SAFE_CALL_STUB(setPlayerActiveMediaItemTimeAttribute, mediaItemTime);
     },
     "Set active media item time attribute"},
    {"24", "setSourceActiveSourcePropertiesAttribute",
     [](const po::variables_map& vm) {
         VMC_LOGI("setSourceActiveSourcePropertiesAttribute: only set browseRoot.");
         std::string browseRoot;
         std::cout << "Enter browseRoot: ";
         std::cin >> browseRoot;
         FidlVmcMediaSourceProperties mediaSourceProperties;
         gTestClientProx->buildVmcMediaSourceProperties(mediaSourceProperties, browseRoot);
         SAFE_CALL_STUB(setSourceActiveSourcePropertiesAttribute, mediaSourceProperties);
     },
     "Set source active source properties attribute"},
    {"25", "setSourceListAttribute",
     [](const po::variables_map& vm) {
         VMC_LOGI(
             "setSourceListAttribute: set metadata information used for describing any available "
             "media source. input a display name by string.");
         std::string displayName;
         std::cin >> displayName;
         std::vector<FidlVmcMediaSourceItem> mediaSourceItemVector;
         gTestClientProx->buildVmcMediaSourceItemByName(displayName, mediaSourceItemVector);
         SAFE_CALL_STUB(setSourceListAttribute, mediaSourceItemVector);
     },
     "Set source list attribute"},
    {"26", "broadcastGeneralParametersUpdated",
     [](const po::variables_map& vm) {
         VMC_LOGI(
             "broadcastGeneralParametersUpdated: please set index ValidRange: [0, 1] (@0 for "
             "Lady Gaga, @others for 보아).");
         uint32_t index;
         std::cin >> index;
         std::vector<FidlVmcKeyValuePair> vmcKeyValuePairVector;
         gTestClientProx->buildPairVectorWithIndex(index, vmcKeyValuePairVector);
         SAFE_CALL_STUB(fireGeneralParametersUpdatedEvent, vmcKeyValuePairVector);
     },
     "Broadcast general parameters updated"},
    {"27", "broadcastBrowserQueryKeepAliveEvent",
     [](const po::variables_map& vm) {
         VMC_LOGI(
             "broadcastBrowserQueryKeepAliveEvent: input the Unique view identifier provided from "
             "Server side by uint8_t.");
         uint32_t viewId;
         std::cin >> viewId;
         if (viewId < std::numeric_limits<uint8_t>::min() ||
             viewId > std::numeric_limits<uint8_t>::max()) {
             VMC_LOGW("broadcastBrowserQueryKeepAliveEvent: use the default viewId = 1 as input.");
             viewId = 1;
         }
         SAFE_CALL_STUB(fireBrowserQueryKeepAliveEvent, viewId);
     },
     "Broadcast browser query keep alive event"},
    {"28", "broadcastBrowserWindowChangedEvent",
     [](const po::variables_map& vm) {
         VMC_LOGI("broadcastBrowserWindowChangedEvent: input the event: [0, 5]");

         // Read event input from the user
         uint32_t eventIn;
         std::cin >> eventIn;

         // Create VmcWindowChangedEvent using the user input
         fidlTypes::VmcWindowChangedEvent event(
             fidlTypes::VmcWindowChangedEvent::Literal(eventIn % 6));

         VMC_LOGI("set vmcWindowChangedEvent: %s", event.toString());

         // Prepare the VmcWindowChangedResult
         fidlTypes::VmcWindowChangedResult result;

         // Set the event to result
         result.setWindowChangedEvent(event);

         // Set additional fields if needed
         result.setTitle("Example Title");
         result.setIndexOfActiveMedia(fidlTypes::VmcPosition(1));

         // Fire the browser window changed event
         SAFE_CALL_STUB(fireBrowserWindowChangedEvent, result);
     },
     "Broadcast browser window change event"},

     {
        "29", "setMediaRestrictedModeAttribute",
        [](const po::variables_map& vm) {
            VMC_LOGI(
                "setMediaRestrictedModeAttribute: ValidRange: [0, 1] "
                "(0 for unrestricted, 1 for restricted)");

            // Prompt user for input
            uint32_t in_mediaRestrictedMode;
            std::cout << "Enter media restricted mode (0 or 1): ";
            std::cin >> in_mediaRestrictedMode;

            // Set the media restricted mode attribute
            FidlVmcMediaRestrictedMode restrictedMode =
                FidlVmcMediaRestrictedMode(
                    static_cast<FidlVmcMediaRestrictedMode::Literal>(in_mediaRestrictedMode));

            // Safely call the stub method
            SAFE_CALL_STUB(setMediaRestrictedModeAttribute, restrictedMode);
        },
        "Set media restricted mode attribute"
    }
};

uint32_t primaryLoop(const std::string& instanceID) {
    gTestClientProx = std::make_unique<VmcTestClientAsConsumer>();

    std::unordered_map<std::string, Command> cmdmap;
    for (auto& cmd : cmdList) {
        cmdmap[cmd.id] = cmd;
        cmdmap[cmd.name] = cmd;
    }

    displayPrompt(instanceID, Prompt);

    bool exitFlag = false;
    while (!exitFlag && gTestClientProx != nullptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Read a line of input
        std::string inputLine;
        std::getline(std::cin, inputLine);

        // Use a stringstream to split the input line into words
        std::istringstream iss(inputLine);
        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }

        // Convert the vector of strings to argc and argv format
        int argc = args.size();
        std::vector<const char*> argv(argc + 1);  // +1 for the program name
        argv[0] = "vmc";                          // Dummy program name
        for (int i = 0; i < argc; ++i) {
            argv[i + 1] = args[i].c_str();
        }

        // Define the options
        po::options_description desc("Allowed options (ex : vmc -o 3)");
        desc.add_options()("help,h", "produce help message")(
            "menu,m", "show the Classic style Menu")("option,o", po::value<uint32_t>(), "option")(
            "sourceId", po::value<std::string>(),
            "User will set sourceId like parameters in separate options for convenience.")(
            "list,l", "list all available commands with descriptions.")("exit,e",
                                                                        "exit the program");

        // Parse the options
        po::variables_map vm;
        po::store(po::parse_command_line(argc + 1, argv.data(), desc), vm);
        po::notify(vm);

        // Handle the options
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            continue;
        }

        if (vm.count("menu")) {
            displayPrompt(instanceID, Prompt);
            continue;
        }

        if (vm.count("exit")) {
            exitFlag = true;
            continue;
        }

        if (vm.count("list")) {
            std::cout << "Available commands(vmc -o ):" << std::endl;
            for (const auto& cmd : cmdList) {
                std::cout << "ID: " << cmd.id << ", Name: " << cmd.name << " - " << cmd.description
                          << std::endl;
            }
            continue;
        }

        if (vm.count("option")) {
            uint32_t optionValue = vm["option"].as<uint32_t>();
            std::string option = std::to_string(optionValue);

            auto it = cmdmap.find(option);
            if (it != cmdmap.end()) {
                it->second.func(vm);
            } else {
                std::cout << "Unknown command: " << option
                          << "    find available commands by $ vmc -l or $ vmc -m" << std::endl;
            }

            if (optionValue == kExit) {
                exitFlag = true;
            }
        }
    }

    if (exitFlag) {
        std::cout << "Exiting primaryLoop" << std::endl;
    }

    return 0;
}

void signalExit() {
    {
        std::lock_guard<std::mutex> lock(exitMutex);
        ready_to_exit = true;
    }
    exitCondition.notify_one();
}

void displayPrompt(const std::string& instanceID, const std::string& prompt) {
    std::cout << "\n"
              << "           @@@@@@@  Testing with provider instance ID : " << instanceID
              << "  @@@@@@@" << "\n\n"
              << prompt << std::endl;
}