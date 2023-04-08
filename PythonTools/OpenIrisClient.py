from typing import Optional, Callable

import aiohttp

from constants import WifiPowerPoint
from .models import (
    DeviceConfig,
    CameraConfig,
    WiFiConfig,
    TrackerConfig,
    WiFiTXPower,
    MDNSConfig,
)


class BaseAPIClient:
    def __init__(self, tracker_address: str):
        self.tracker_address = tracker_address
        self.base_endpoint = "control/builtin/command"

    async def __aenter__(self):
        self.session = aiohttp.ClientSession()
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        await self.session.close()

    async def get(
        self,
        command: str,
        params: Optional[dict] = None,
        validator: Optional[Callable] = None,
    ):
        clean_params = await self._clean_params(params)

        if validator and not validator(clean_params):
            raise ValueError(
                f"Params for command {command} are required, none provided"
            )

        async with self.session.get(
            f"{self.tracker_address}/{self.base_endpoint}/{command}/",
            params=clean_params,
        ) as request:
            await request.read()
            return request

    async def post(
        self, command: str, data: dict, validator: Optional[Callable] = None
    ):
        clean_params = await self._clean_params(data)

        if validator and not validator(clean_params):
            raise ValueError(
                f"Params for command {command} are required, none provided"
            )

        async with self.session.post(
            f"{self.tracker_address}/{self.base_endpoint}/{command}/",
            params=clean_params,
        ) as request:
            await request.read()
            return request

    async def delete(
        self, command: str, data: dict, validator: Optional[Callable] = None
    ):
        clean_params = await self._clean_params(data)

        if validator and not validator(clean_params):
            raise ValueError(
                f"Params for command {command} are required, none provided"
            )

        async with self.session.delete(
            f"{self.tracker_address}/{self.base_endpoint}/{command}/",
            params=clean_params,
        ) as request:
            await request.read()
            return request

    @staticmethod
    async def _clean_params(params) -> dict:
        if params:
            return {k: v for k, v in params.items() if v not in (None, "")}
        return {}


class OpenIrisClient(BaseAPIClient):
    async def upsert_wifi_settings(self, network: WiFiConfig):

        params = {
            "networkName": network.name,
            "ssid": network.ssid,
            "password": network.password,
            "channel": network.channel,
            "power": network.power,
            "adhoc": network.adhoc,
        }

        return await self.post("setWiFi", data=params, validator=all)

    async def remove_wifi_network(self, name: str):
        params = {
            "networkName": name,
        }

        return await self.delete("setWiFi", data=params, validator=all)

    async def get_stored_config(self) -> TrackerConfig:
        response = await self.get("getStoredConfig")
        return TrackerConfig(**await response.json())

    async def reset_config(self):
        return await self.get("resetConfig")

    # we should split this into two separate configs and endpoints and clean them up
    async def update_device_settings(
        self, mdns_config: MDNSConfig, device_config: DeviceConfig
    ):
        params = {
            "hostname": mdns_config.hostname,
            "service": mdns_config.service,
            "ota_login": device_config.OTALogin,
            "ota_password": device_config.OTAPassword,
        }

        return await self.get(command="setDevice", params=params)

    async def reboot_device(self):
        return await self.get(command="rebootDevice")

    async def set_tx_power(self, power_level: WifiPowerPoint):
        params = {
            "txPower": power_level,
        }
        return await self.get(command="setTxPower", params=params)

    async def get_wifi_strength(self):
        return await self.get(command="wifiStrength")

    async def update_camera_settings(self, camera_config: CameraConfig):
        params = {
            "framesize": camera_config.framesize,
            "vflip": camera_config.vflip,
            "hflip": camera_config.hflip,
            "quality": camera_config.quality,
            "brightness": camera_config.brightness,
        }

        return await self.get(command="setCamera", params=params)

    async def restart_camera(self, hard_reset: bool = False):
        params = {"mode": int(hard_reset)}
        return await self.get(command="restartCamera", params=params)

    async def ping(self):
        return await self.get(command="ping")

    async def save_config(self):
        return await self.get(command="save")
