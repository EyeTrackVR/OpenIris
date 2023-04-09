import pytest_asyncio


@pytest_asyncio.fixture()
async def device_url():
    return "http://openiristracker.local:81/"
