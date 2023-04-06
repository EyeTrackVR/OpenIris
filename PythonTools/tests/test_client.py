import pytest
from aioresponses import aioresponses

from ..OpenIrisClient import OpenIrisClient


@pytest.mark.asyncio
async def test_ping(device_url):
    response_payload = {"message": "ok"}

    with aioresponses() as m:
        m.get(f"{device_url}/control/builtin/command/ping/", status=200, payload=response_payload)
        async with OpenIrisClient(device_url) as openiris_client:
            response = await openiris_client.ping()

        m.assert_called_once()
        assert response.status == 200
        assert await response.json() == {"message": "ok"}
