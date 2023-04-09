import re

import pytest
from aioresponses import aioresponses

from constants import WifiPowerPoint, FrameSize
from ..models import TrackerConfig, CameraConfig, MDNSConfig, DeviceConfig
from ..OpenIrisClient import OpenIrisClient


@pytest.mark.asyncio
async def test_ping(device_url):
    with aioresponses() as m:
        m.get(
            f"{device_url}/control/builtin/command/ping/",
            status=200,
        )
        async with OpenIrisClient(device_url) as openiris_client:
            response = await openiris_client.ping()

        m.assert_called_once()
        assert response.status == 200


@pytest.mark.asyncio
@pytest.mark.parametrize(
    "response_payload,expected_result",
    [
        (
            {
                "device_config": {
                    "OTALogin": "openiris",
                    "OTAPassword": "12345678",
                },
                "camera_config": {
                    "vflip": 0,
                    "framesize": 4,
                    "href": 0,
                    "quality": 7,
                    "brightness": 2,
                },
                "networks": [],
                "mdns_config": {"hostname": "openiristracker", "service": ""},
                "ap_wifi_config": {
                    "ssid": "",
                    "password": "",
                    "channel": 0,
                    "adhoc": False,
                },
            },
            TrackerConfig(
                **{
                    "device_config": {
                        "OTALogin": "openiris",
                        "OTAPassword": "12345678",
                    },
                    "camera_config": {
                        "vflip": 0,
                        "framesize": 4,
                        "href": 0,
                        "quality": 7,
                        "brightness": 2,
                    },
                    "networks": [],
                    "mdns_config": {"hostname": "openiristracker", "service": ""},
                    "ap_wifi_config": {
                        "ssid": "",
                        "password": "",
                        "channel": 0,
                        "adhoc": False,
                    },
                },
            ),
        )
    ],
)
async def test_get_stored_config(device_url, response_payload, expected_result):

    with aioresponses() as m:
        m.get(
            f"{device_url}/control/builtin/command/getStoredConfig/",
            status=200,
            payload=response_payload,
        )
        async with OpenIrisClient(device_url) as openiris_client:
            result = await openiris_client.get_stored_config()

        m.assert_called_once()
        assert result.dict() == expected_result.dict()


@pytest.mark.asyncio
@pytest.mark.parametrize("payload", {"msg": "Factory Reset"})
async def test_reset_config(device_url, payload):

    with aioresponses() as m:
        m.get(
            f"{device_url}/control/builtin/command/resetConfig/",
            status=200,
            payload=payload,
        )

        async with OpenIrisClient(device_url) as openiris_client:
            result = await openiris_client.reset_config()

    m.assert_called_once()
    assert await result.json() == payload


@pytest.mark.asyncio
@pytest.mark.parametrize("payload", {"msg": "config saved"})
async def test_save_config(device_url, payload):

    with aioresponses() as m:
        m.get(
            f"{device_url}/control/builtin/command/save/",
            status=200,
            payload=payload,
        )

        async with OpenIrisClient(device_url) as openiris_client:
            result = await openiris_client.save_config()

    m.assert_called_once()
    assert await result.json() == payload


@pytest.mark.asyncio
@pytest.mark.parametrize("payload", [{"msg": "wifi TX power set"}])
async def test_set_tx_power(device_url, payload):
    with aioresponses() as m:
        m.get(
            re.compile(rf"{device_url}/control/builtin/command/setTxPower/\?txPower=\d+"),
            status=200,
            payload=payload,
        )

        async with OpenIrisClient(device_url) as openiris_client:
            result = await openiris_client.set_tx_power(WifiPowerPoint.WIFI_POWER_5dBm)

    m.assert_called_once()
    assert await result.json() == payload


@pytest.mark.asyncio
@pytest.mark.parametrize(
    "payload, response",
    [
        (
            True,
            {"msg": "camera reset"},
        ),
        (
            False,
            {"msg": "camera reset"},
        ),
    ],
)
async def test_restart_camera(device_url, payload, response):
    with aioresponses() as m:
        m.get(
            re.compile(rf"{device_url}/control/builtin/command/restartCamera/\?mode=\d+"),
            status=200,
            payload=response,
        )

        async with OpenIrisClient(device_url) as openiris_client:
            result = await openiris_client.restart_camera(payload)

    m.assert_called_once()
    assert await result.json() == response


@pytest.mark.asyncio
@pytest.mark.parametrize(
    "parameters,query_params,payload",
    [
        (
            {"framesize": FrameSize.FRAMESIZE_240X240},
            "framesize=5",
            {"msg": "Done. Camera Settings have been set."},
        ),
        (
            {"vflip": False},
            "vflip=0",
            {"msg": "Done. Camera Settings have been set."},
        ),
        (
            {"hflip": False},
            "hflip=0",
            {"msg": "Done. Camera Settings have been set."},
        ),
        (
            {"quality": 11},
            "quality=11",
            {"msg": "Done. Camera Settings have been set."},
        ),
        (
            {"brightness": 2},
            "brightness=2",
            {"msg": "Done. Camera Settings have been set."},
        ),
        (
            {
                "framesize": FrameSize.FRAMESIZE_240X240,
                "vflip": True,
                "hflip": True,
                "quality": 9,
                "brightness": 1,
            },
            "brightness=1&framesize=5&hflip=1&quality=9&vflip=1",
            {"msg": "Done. Camera Settings have been set."},
        ),
    ],
)
async def test_update_camera_settings(device_url, parameters, query_params, payload):
    with aioresponses() as m:
        m.get(
            f"{device_url}/control/builtin/command/setCamera/?{query_params}",
            status=200,
            payload=payload,
        )
        async with OpenIrisClient(device_url) as openiris_client:
            result = await openiris_client.update_camera_settings(CameraConfig(**parameters))

    m.assert_called_once()
    assert await result.json() == payload


@pytest.mark.asyncio
@pytest.mark.parametrize(
    "payload",
    [{"msg": "rebooting device"}],
)
async def test_reboot_device(device_url, payload):
    with aioresponses() as m:
        m.get(
            f"{device_url}/control/builtin/command/rebootDevice/",
            status=200,
            payload=payload,
        )

        async with OpenIrisClient(device_url) as openiris_client:
            result = await openiris_client.reboot_device()

    m.assert_called_once()
    assert await result.json() == payload


@pytest.mark.asyncio
@pytest.mark.parametrize(
    "parameters,query_params,payload",
    [
        (
            {"hostname": "someTestName"},
            "hostname=someTestName",
            {"msg": "Done. Device Settings have been set."},
        ),
        (
            {"service": "someService"},
            "service=someService",
            {"msg": "Done. Device Settings have been set."},
        ),
        (
            {"ota_login": "otaLogin"},
            "ota_login=otaLogin",
            {"msg": "Done. Device Settings have been set."},
        ),
        (
            {"ota_password": "123456798"},
            "ota_password=123456798",
            {"msg": "Done. Device Settings have been set."},
        ),
        (
            {
                "hostname": "someTestName",
                "service": "someService",
                "ota_login": "otaLogin",
                "ota_password": "123456798",
            },
            "hostname=someTestName&service=someService&ota_login=otaLogin&ota_password=123456798",
            {"msg": "Done. Device Settings have been set."},
        ),
    ],
)
async def test_update_device_settings(device_url, parameters, query_params, payload):
    with aioresponses() as m:
        m.get(
            f"{device_url}/control/builtin/command/setDevice/?{query_params}",
            status=200,
            payload=payload,
        )
        async with OpenIrisClient(device_url) as openiris_client:
            keys = parameters.keys()

            device_config = DeviceConfig()
            mdns_config = MDNSConfig()

            if "ota_login" in keys or "ota_password" in keys:
                device_config.OTALogin = parameters.get("ota_login")
                device_config.OTAPassword = parameters.get("ota_password")
            if "service" in keys or "hostname" in keys:
                mdns_config.hostname = parameters.get("hostname")
                mdns_config.service = parameters.get("service")

            result = await openiris_client.update_device_settings(device_config=device_config, mdns_config=mdns_config)

    m.assert_called_once()
    assert await result.json() == payload


@pytest.mark.asyncio
@pytest.mark.parametrize(
    "payload",
    [{"wifi_power": 20}],
)
async def test_get_wifi_strength(device_url, payload):
    with aioresponses() as m:
        m.get(
            f"{device_url}/control/builtin/command/wifiStrength/",
            status=200,
            payload=payload,
        )

        async with OpenIrisClient(device_url) as openiris_client:
            result = await openiris_client.get_wifi_strength()

        m.assert_called_once()
        assert await result.json() == payload
