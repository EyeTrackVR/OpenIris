import pytest_asyncio

from .OpenIrisClient import OpenIrisClient


@pytest_asyncio.fixture()
async def device_url():
    return "http://openiristracker.local:81"


@pytest_asyncio.fixture()
async def openiris_client(device_url):
    return OpenIrisClient(tracker_address=device_url)
