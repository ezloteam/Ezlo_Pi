#include "mac_uuid.h"
#include "trace.h"

#define MAC_ADDR_SIZE 6


static void print_mac(const char *mac) {
	TRACE_B("%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}

// Function to generate a UUID from a given WiFi MAC address
void ezlopi_generate_UUID(char* uuid) {

    char ezlopi_namespace[20] = "EzloPi";

    char mac_base[MAC_ADDR_SIZE + 1] = {0};
    esp_efuse_mac_get_default((uint8_t *)mac_base);
    mac_base[MAC_ADDR_SIZE] = 0;
    
    // print_mac(mac_base);

    strcat(ezlopi_namespace, mac_base);

    // Generate a SHA-1 hash of the namespace identifier
    unsigned char hash[20];
    mbedtls_sha1((unsigned char*)ezlopi_namespace, strlen(ezlopi_namespace), hash);

    // Format the hash as a UUID
    sprintf(uuid, "%08x-%04x-%04x-%04x-%012llx",
            (unsigned int)hash[0] << 24 | (unsigned int)hash[1] << 16 | (unsigned int)hash[2] << 8 | (unsigned int)hash[3],
            (unsigned int)hash[4] << 8 | (unsigned int)hash[5],
            ((unsigned int)hash[6] & 0x0FFF) | 0x4000,
            ((unsigned int)hash[7] & 0x3FFF) | 0x8000,
            (unsigned long long)hash[8] << 56 | (unsigned long long)hash[9] << 48 | (unsigned long long)hash[10] << 40 | (unsigned long long)hash[11] << 32 | (unsigned long long)hash[12] << 24 | (unsigned long long)hash[13] << 16 | (unsigned long long)hash[14] << 8 | (unsigned long long)hash[15]);

}