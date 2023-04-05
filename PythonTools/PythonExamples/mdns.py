from zeroconf import Zeroconf, ServiceListener, ServiceBrowser


class Listener(ServiceListener):
    def add_service(self, zc: "Zeroconf", type_: str, name: str) -> None:
        info = zc.get_service_info(type_, name)
        print(f"Service {name} added, service info: {info}")
        print(f"Address {info.parsed_addresses()}")
        print(f"Port {info.port}")

    def remove_service(self, zc: "Zeroconf", type_: str, name: str) -> None:
        print(f"Service {name} removed")

    def update_service(self, zc: "Zeroconf", type_: str, name: str) -> None:
        print(f"Service {name} updated")


if __name__ == "__main__":
    zconf = Zeroconf()
    cameraListener = Listener()
    browser = ServiceBrowser(zconf, "_openIrisTracker._tcp.local.", cameraListener)
    input("Press enter to close... \n")
    zconf.close()
