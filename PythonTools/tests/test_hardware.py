import pytest

from ..OpenIrisClient import OpenIrisClient


@pytest.mark.asyncio
async def test_ping(device_url):
    """tests whether the device will respond with a ping"""
    async with OpenIrisClient(device_url) as openiris_client:
        result = await openiris_client.ping()
    assert result.status == 200
