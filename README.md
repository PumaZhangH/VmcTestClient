# Prerequisites
Before the actual test, Make Sure
   1.  vendor.bmw.vehiclemediacontrolhal@1.0-service and com.bmwgroup.idnext.vehiclemediacontrol.service running on the target.
   2.  The folder idcevo-samsung_v9x0-mainline/Android/vendor/bmw/hardware/implementations/vehiclemediacontrol/ is available.

# How to Build and Run
Put the "hal-test" folder into source code Android/vendor/bmw/hardware/implementations/vehiclemediacontrol/1.0/default/tests
The same folder with "unit-tests".
## specified the provider id
Run testClient by ./VmcTestClient [-option]
User need input the "instance id " first before test. Since "1" is used by  MockApp side. User can choose "2" or "3" as the provider instance ID.

## Run test case
   1. lookup available test case by $ vmc -l.
   2. run the test case ex : vmc -o 3.

## Binary Path on Target

adb push out/target/product/bmw_idcevo/vendor/bin/hw/VmcTestClient /vendor/bin/hw

# Basic Information
## Log Tag
   Filter the tag "VehicleMediaControlHal" for this process.
   Filter the tag "VehicleMediaControlHalConsumer" and "VmcTestClientAsProvider" when works as a provider.
   Filter the tag "VehicleMediaControlHalProvider" and "VmcTestClientAsConsumer" when works as a consumer.


## Dump Icon datat Path on Target
   "/vendor/bin/hw/iconRaw.bin"
# Test Client Run As Consumer
## Test Steps
When Test Client worked as Consumer. We need connect the Provider first by run " vmc -o 1" to check the provider status.

  1. The message   *Waiting availability!* means we not connect to a Provider. You need the set it on the Test APP side.

  2. The message  *isAvailable: VmcTestClient::isAvailable returned 1* means you can go next test case.

The first Test Case is " 2. connect Source" , And other test can test radom.

## Test Data

### 1. setParameters with index 0
Format                                   : MPEG Audio
File size                                : 6.29 MiB
Duration                                 : 3 min 24 s
Overall bit rate mode                    : Constant
Overall bit rate                         : 256 kb/s
Album                                    : The Fame
Track name                               : i Like It Rough
Performer                                : Lady Gaga
Genre                                    : Electronic
Recorded date                            : 2008-10-28

### 2. setParameters with other index

Performer                                : 보아
Genre                                    : Ballad
Recorded date                            : 2002
Copyright                                : LOEN Entertainment, Inc.
Cover                                    : Yes
Cover type                               : Cover (front)
Cover MIME                               : image/jpeg
Comment                                  : http://www.melon.com/
Format                                   : MPEG Audio
Format version                           : Version 1
Format profile                           : Layer 3
Format settings                          : Joint stereo
Duration                                 : 3 min 13 s
Bit rate mode                            : Constant
Bit rate                                 : 320 kb/s
Channel(s)                               : 2 channels
Sampling rate                            : 44.1 kHz
Frame rate                               : 38.281 FPS (1152 SPF)

### 3. browser
  Fixed default test parameters:
   52 browserCreateView
         fidlTypes::VmcSourceId sourceId = "52";  // Example source ID
         selectedMediaItem = static_cast<fidlTypes::VmcMediaId>("52");
   54 browserMoveWindow
         selectedMediaItem = static_cast<fidlTypes::VmcMediaId>("54");
   56 browserMoveViewToItem
         selectedMediaItem = static_cast<fidlTypes::VmcPosition>(56);

# Test Client Run As Provider

## setActiveSourcePropertiesAttribute
   Only setIsBrowsable true or false. other value set the to false.

## Test Data
### setSourcesAttribute
     Only display name  can be set. Other value are fixed, ex : we set display name "Andy"

Format                                   : MPEG Audio
File size                                : 6.29 MiB
Duration                                 : 3 min 24 s
Overall bit rate mode                    : Constant
Overall bit rate                         : 256 kb/s
Album                                    : The Fame
Track name                               : i Like It Rough
Performer                                : Lady Gaga
Genre                                    : Electronic
Recorded date                            : 2008-10-28
sourceId: 008
displayName:***Andy***
iconIds: []

Performer                                : 보아
Genre                                    : Ballad
Recorded date                            : 2002
Copyright                                : LOEN Entertainment, Inc.
Cover                                    : Yes
Cover type                               : Cover (front)
Cover MIME                               : image/jpeg
Comment                                  : http://www.melon.com/
Format                                   : MPEG Audio
Format version                           : Version 1
Format profile                           : Layer 3
Format settings                          : Joint stereo
Duration                                 : 3 min 13 s
Bit rate mode                            : Constant
Bit rate                                 : 320 kb/s
Channel(s)                               : 2 channels
Sampling rate                            : 44.1 kHz
Frame rate                               : 38.281 FPS (1152 SPF)
sourceId: 009,
displayName: ***Andy2***

### broadcastParametersUpdated
see the setParameters.

### setSourceListAttribute
There is a modify for the input displayName.
    sourceItem2.setDisplayName(inputName + "2");


### getIcon
RawData value is : {0x99,0x99};

export SOONG_GEN_COMPDB=1
export SOONG_GEN_COMPDB_DEBUG=1

OUT_DIR 和 TARGET_PRODUCT 你根据自己的来

OUT_DIR=out

prebuilts/build-tools/linux-x86/bin/ninja -f ${OUT_DIR}/combined-"${TARGET_PRODUCT}".ninja -t compdb g.cc.cc > compile_commands.json.ninja



2.

ln -sf  compile_commands.json.ninja compile_commands.json



