from pydantic import BaseModel
from typing import Optional, List

from .constants import FrameSize, WifiPowerPoint


class DeviceConfig(BaseModel):
    OTALogin: Optional[str] = None
    OTAPassword: Optional[str] = None


class MDNSConfig(BaseModel):
    hostname: Optional[str] = None
    service: Optional[str] = None


class CameraConfig(BaseModel):
    framesize: Optional[FrameSize] = None
    vflip: Optional[bool] = None
    hflip: Optional[bool] = None
    quality: Optional[int] = None
    brightness: Optional[int] = None


class WiFiConfig(BaseModel):
    name: str
    ssid: str
    password: str
    channel: int
    power: int
    adhoc: bool


class WiFiTXPower(BaseModel):
    power: WifiPowerPoint


class APWiFiConfig(BaseModel):
    ssid: str
    password: str
    channel: int
    adhoc: bool


class TrackerConfig(BaseModel):
    device_config: DeviceConfig
    camera_config: CameraConfig
    networks: List[WiFiConfig]
    ap_wifi_config: APWiFiConfig
    mdns_config: MDNSConfig
