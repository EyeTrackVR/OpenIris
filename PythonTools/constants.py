from enum import IntEnum


class FrameSize(IntEnum):
    FRAMESIZE_96X96 = (1,)  # // 96x96
    FRAMESIZE_QQVGA = (2,)  # // 160x120
    FRAMESIZE_QCIF = (3,)  # // 176x144
    FRAMESIZE_HQVGA = (4,)  # // 240x176
    FRAMESIZE_240X240 = (5,)  # // 240x240
    FRAMESIZE_QVGA = (6,)  # // 320x240
    FRAMESIZE_CIF = (7,)  # // 400x296
    FRAMESIZE_HVGA = (8,)  # // 480x320
    FRAMESIZE_VGA = (9,)  # // 640x480


class WifiPowerPoint(IntEnum):
    WIFI_POWER_19_5dBm = (78,)  # 19.5dBm
    WIFI_POWER_19dBm = (76,)  # 19dBm
    WIFI_POWER_18_5dBm = (74,)  # 18.5dBm
    WIFI_POWER_17dBm = (68,)  # 17dBm
    WIFI_POWER_15dBm = (60,)  # 15dBm
    WIFI_POWER_13dBm = (52,)  # 13dBm
    WIFI_POWER_11dBm = (44,)  # 11dBm
    WIFI_POWER_8_5dBm = (34,)  # 8.5dBm
    WIFI_POWER_7dBm = (28,)  # 7dBm
    WIFI_POWER_5dBm = (20,)  # 5dBm
    WIFI_POWER_2dBm = (8,)  # 2dBm
    WIFI_POWER_MINUS_1dBm = -4  # -1dBm
