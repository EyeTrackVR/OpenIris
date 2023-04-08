import re

import pytest
from aioresponses import aioresponses

from constants import WifiPowerPoint
from ..models import TrackerConfig
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
            re.compile(
                rf"{device_url}/control/builtin/command/setTxPower/\?txPower=\d+"
            ),
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
            re.compile(
                rf"{device_url}/control/builtin/command/restartCamera/\?mode=\d+"
            ),
            status=200,
            payload=response,
        )

        async with OpenIrisClient(device_url) as openiris_client:
            result = await openiris_client.restart_camera(payload)

    m.assert_called_once()
    assert await result.json() == response
