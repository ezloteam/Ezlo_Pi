#include "ezlopi_wifi_err_reason.h"
#include "esp_wifi_types.h"

const char *ezlopi_wifi_err_reason_str(wifi_err_reason_t wifi_err)
{
    switch (wifi_err)
    {
    case WIFI_REASON_AUTH_EXPIRE:
    {
        return "WIFI_REASON_AUTH_EXPIRE";
    }
    case WIFI_REASON_AUTH_LEAVE:
    {
        return "WIFI_REASON_AUTH_LEAVE";
    }
    case WIFI_REASON_ASSOC_EXPIRE:
    {
        return "WIFI_REASON_ASSOC_EXPIRE";
    }
    case WIFI_REASON_ASSOC_TOOMANY:
    {
        return "WIFI_REASON_ASSOC_TOOMANY";
    }
    case WIFI_REASON_NOT_AUTHED:
    {
        return "WIFI_REASON_NOT_AUTHED";
    }
    case WIFI_REASON_NOT_ASSOCED:
    {
        return "WIFI_REASON_NOT_ASSOCED";
    }
    case WIFI_REASON_ASSOC_LEAVE:
    {
        return "WIFI_REASON_ASSOC_LEAVE";
    }
    case WIFI_REASON_ASSOC_NOT_AUTHED:
    {
        return "WIFI_REASON_ASSOC_NOT_AUTHED";
    }
    case WIFI_REASON_DISASSOC_PWRCAP_BAD:
    {
        return "WIFI_REASON_DISASSOC_PWRCAP_BAD";
    }
    case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
    {
        return "WIFI_REASON_DISASSOC_SUPCHAN_BAD";
    }
    case WIFI_REASON_BSS_TRANSITION_DISASSOC:
    {
        return "WIFI_REASON_BSS_TRANSITION_DISASSOC";
    }
    case WIFI_REASON_IE_INVALID:
    {
        return "WIFI_REASON_IE_INVALID";
    }
    case WIFI_REASON_MIC_FAILURE:
    {
        return "WIFI_REASON_MIC_FAILURE";
    }
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
    {
        return "WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT";
    }
    case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
    {
        return "WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT";
    }
    case WIFI_REASON_IE_IN_4WAY_DIFFERS:
    {
        return "WIFI_REASON_IE_IN_4WAY_DIFFERS";
    }
    case WIFI_REASON_GROUP_CIPHER_INVALID:
    {
        return "WIFI_REASON_GROUP_CIPHER_INVALID";
    }
    case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
    {
        return "WIFI_REASON_PAIRWISE_CIPHER_INVALID";
    }
    case WIFI_REASON_AKMP_INVALID:
    {
        return "WIFI_REASON_AKMP_INVALID";
    }
    case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
    {
        return "WIFI_REASON_UNSUPP_RSN_IE_VERSION";
    }
    case WIFI_REASON_INVALID_RSN_IE_CAP:
    {
        return "WIFI_REASON_INVALID_RSN_IE_CAP";
    }
    case WIFI_REASON_802_1X_AUTH_FAILED:
    {
        return "WIFI_REASON_802_1X_AUTH_FAILED";
    }
    case WIFI_REASON_CIPHER_SUITE_REJECTED:
    {
        return "WIFI_REASON_CIPHER_SUITE_REJECTED";
    }
    case WIFI_REASON_INVALID_PMKID:
    {
        return "WIFI_REASON_INVALID_PMKID";
    }
    case WIFI_REASON_BEACON_TIMEOUT:
    {
        return "WIFI_REASON_BEACON_TIMEOUT";
    }
    case WIFI_REASON_NO_AP_FOUND:
    {
        return "WIFI_REASON_NO_AP_FOUND";
    }
    case WIFI_REASON_AUTH_FAIL:
    {
        return "WIFI_REASON_AUTH_FAIL";
    }
    case WIFI_REASON_ASSOC_FAIL:
    {
        return "WIFI_REASON_ASSOC_FAIL";
    }
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
    {
        return "WIFI_REASON_HANDSHAKE_TIMEOUT";
    }
    case WIFI_REASON_CONNECTION_FAIL:
    {
        return "WIFI_REASON_CONNECTION_FAIL";
    }
    case WIFI_REASON_AP_TSF_RESET:
    {
        return "WIFI_REASON_AP_TSF_RESET";
    }
    case WIFI_REASON_ROAMING:
    {
        return "WIFI_REASON_ROAMING";
    }
    case WIFI_REASON_UNSPECIFIED:
    default:
    {
        return "WIFI_REASON_UNSPECIFIED";
    }
    }
}