# Integrating MDM Support on the reMarkable 2 via mini-snmpd

Integrating Mobile Device Management (MDM) support into the reMarkable 2 e-ink reader marks a significant advancement in efficient real-time monitoring and remote device management. This project focuses on porting the Mini-SNMPD agent, a lightweight [SNMP server](https://www.manageengine.com/network-monitoring/what-is-snmp.html), into the reMarkable OS. This integration enables seamless communication with MDM platforms like ManageEngine.

[Watch the video](https://raw.githubusercontent.com/99x-incubator/remarkable-2-mdm/refs/heads/main/resources/doc/remarkable-manageengine-integration.webm)

## Why mini-snmpd?
The [mini-snmpd](https://github.com/troglobit/mini-snmpd) agent was selected for its simplicity and adaptability, making it ideal for resource-constrained environments such as the reMarkable 2. To customize the agent for the device's specific capabilities, we enhanced its functionality by incorporating custom Object Identifiers (OIDs) that reflect parameters unique to the reMarkable 2. These custom OIDs include:

- **Battery Level**: Provides real-time updates on the device's power status.
- **Document Statistics**: Reports the number of documents stored on the device.
- **Storage Utilization and Health**: Monitors storage usage and system performance.

With these enhancements, administrators can query device-specific information and monitor performance remotely. The mini-snmpd agent offers a standardized SNMP interface, allowing tools like ManageEngine to effectively communicate with the device. This ensures that the  reMarkable 2 can be smoothly integrated into existing enterprise IT infrastructures with minimal effort.

## Modifications done to mini-snmpd
To integrate mini-snmpd into the reMarkable 2 e-ink reader, we created a custom OID range, 1.3.6.1.10.1.1, specifically designated for operations and monitoring parameters unique to the reMarkable 2. This OID range ensures that all device-specific metrics are logically organized and easily accessible.

Key metrics, including battery level, document count, and storage information, were implemented using standard BER (Basic Encoding Rules) data types, primarily as counters and gauges. Counters are used for accumulating values, such as document counts, while gauges provide real-time metrics like battery percentage and storage utilization. This approach follows SNMP best practices for defining metrics and  ensuring compatibility with existing SNMP tools.

To ensure modularity and clarity in the codebase, all functionality specific to the reMarkable 2 has been organized into a new source file named [`remarkable.c`](https://github.com/99x-incubator/remarkable-2-mdm/blob/main/mini-snmpd/remarkable.c). This file is responsible for managing the logic related to fetching, updating, and encoding the custom Object Identifiers (OIDs) associated with the device. In contrast, general system-level monitoring functions, such as CPU usage and network statistics, have been integrated into the existing [`linux.c`](https://github.com/99x-incubator/remarkable-2-mdm/blob/main/mini-snmpd/linux.c) file. This separation of concerns simplifies code maintenance and guarantees that reMarkable-specific extensions do not disrupt the core mini-snmpd functionality.

## Compiling and running modified mini-snmpd on reMarkable2 device

The reMarkable device at 99x comes with reMarkable OS version 3.3.2.1666. The appropriate toolchain can be found at the [https://remarkable.guide/devel/toolchains.html#official-toolchain](https://remarkable.guide/devel/toolchains.html#official-toolchain).

To compile mini-snmpd for the reMarkable 2, we set up a cross-compilation environment using the SDK's GCC toolchain and sysroot. The sysroot  contains the necessary libraries, headers, and runtime environment for the device, ensuring binary compatibility. Development takes place on a host machine, where the GCC toolchain compiles the code specifically for the ARM architecture of the reMarkable 2.

*arm-remarkable-linux-gnueabi-gcc* toolchain is used to compile C/C++ code for the ARM Cortex-A7 architecture with appropriate compiler flags:

```bash
arm-remarkable-linux-gnueabi-gcc -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=$SDKTARGETSYSROOT
```

To compile the project clone the modified mini-snmpd from the Git repository and issue the following set of commands:

```bash
cd mini-snmpd
./autogen.sh
./configure
make –j8
```
After compiling, transfer the mini-snmpd file to the reMarkable 2 using SCP via the emulated network interface over USB. Once the customized version of mini-snmpd is deployed on the device, it can be launched with the following command:

```bash
mini-snmpd -n -p 161 -D "reMarkable 2" -L "99x-65" -C "user@99x.io" -d '/' -i usb1
```

![Running mini-snmpd on reMarkable2](https://raw.githubusercontent.com/99x-incubator/remarkable-2-mdm/refs/heads/main/resources/doc/minisnmpd-rm2.png)

The detailed explanation about each parameter in given below:

| Key   | Parameter           | Example            | Description                                                                                                                                                                                                  |
|-------|---------------------|--------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `-n`  | No Fork             | `-n`               | Prevents the mini-snmpd agent from running in the background. This is particularly useful for debugging and testing the agent during initial deployment.                                                     |
| `-p`  | SNMP Port           | `-p 161`           | Specifies the SNMP agent's listening port. In this case, we use 161 because it is the standard SNMP port.                                                                                                    |
| `-D`  | Device Name         | `-D "reMarkable2"` | Sets the device name as "reMarkable2", which will appear in SNMP queries and provide a clear identifier for the monitored device.                                                                            |
| `-L`  | Location            | `-L "99x-65"`      | Indicates the physical or logical location of the device. This helps network administrators easily identify the device within a larger deployment.                                                           |
| `-C`  | Contact Information | `-C "user@99x.io"` | Specifies the contact details (e.g., email) of the administrator or owner of the device. This information is valuable for troubleshooting and device management.                                             |
| `-d`  | Disk to Monitor     | `-d '/'`           | Points to the disk or volume that the SNMP agent will  monitor . In this case, the root volume (/) is selected to provide metrics such as storage  utilization  and health.                                  |
| `-i`  | Network Interface   | `-i usb1`          | Defines the network interface the agent will use for communication. On the reMarkable 2, usb1 typically represents the device's USB networking interface, enabling SNMP communication over a USB connection. |

## About ManageEngine
[ManageEngine](https://www.manageengine.com/about-us.html) is a powerful IT management platform aimed at streamlining and improving enterprise device management. It offers a comprehensive suite of tools for monitoring, configuring, and controlling devices throughout an organization, ensuring security, performance, and compliance. As a Mobile Device Management (MDM) solution, ManageEngine enables IT administrators to manage various devices, including tablets, smartphones, and specialized hardware like the reMarkable 2, all from a centralized console.

ManageEngine is highly effective for managing SNMP-based devices due to its strong support for the SNMP protocol and its capability to process and visualize SNMP data in real time. The platform simplifies the integration of custom SNMP agents, such as the Mini-SNMPD deployed on the reMarkable 2, by offering:

- **Ease of Configuration**: ManageEngine enables administrators to quickly add custom OIDs (Object Identifiers), making it easy to monitor device-specific metrics like document counts and battery levels.
    
-  **Advanced Data Analysis**: Its powerful reporting and alerting features provide actionable insights from SNMP data, helping organizations maintain device uptime and optimize performance.
    
-  **Seamless Integration**: ManageEngine’s modular architecture allows for integration with broader IT ecosystems, ensuring that the reMarkable 2 and other devices can be monitored alongside traditional enterprise hardware.

## Device discovery and monitoring
When ManageEngine begins its Device Discovery phase, it scans the network for connected devices using protocols such as SNMP, ICMP, and ARP. Devices that respond to these queries are automatically identified and added to the management console. For SNMP-enabled devices, ManageEngine initiates communication by sending a GET request to retrieve the device's base Object Identifier (OID). The response to this request provides essential information, including the device type, vendor, and supported metrics.

![reMarkable2 on ManageEngine monitoring dashboard](https://raw.githubusercontent.com/99x-incubator/remarkable-2-mdm/refs/heads/main/resources/doc/manageengine-rm2.png)

In the case of the reMarkable 2, the customized mini-snmpd agent retrieves data from a pre-configured range of Object Identifiers (OIDs), such as 1.3.6.1.10.1.1. This data includes device-specific information, such as battery level, document count, and storage status. ManageEngine processes this information by parsing the reMarkable 2-specific MIB file and displays it on the dashboard with relevant details, including device name, location, and contact information, all of which were set up during the Mini-SNMPD configuration process.

![reMarkable 2 active /response time.](https://raw.githubusercontent.com/99x-incubator/remarkable-2-mdm/refs/heads/main/resources/doc/rm2-response-sample.png)

This automated discovery process eliminates manual configuration, significantly reducing administrative overhead. Once added, the reMarkable 2 can be monitored continuously, with SNMP polls providing real-time updates on device metrics. Alerts can also be configured to notify administrators of critical changes, such as low battery levels or high storage usage, enabling proactive device management and ensuring the reliability of deployed devices.
