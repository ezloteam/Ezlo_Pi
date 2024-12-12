

#ifndef _EZLOPI_SERVICE_URI_H_
#define _EZLOPI_SERVICE_URI_H_

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Function to start AP server service
     * @details Following operations are performed
     *  - Configure and start HTTP server
     *  - Register URI and error handlers
     *
     */
    void EZPI_begin_ap_server_service();
    /**
     * @brief function to end/stop AP server service
     *
     */
    void EZPI_end_ap_server_service();
    /**
     * @brief Function to return Wifi credentials
     *
     * @param wifi_cred
     * @return int
     */
    int EZPI_end_ap_server_serviceget_wifi_cred(char *wifi_cred);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_SERVICE_URI_H_
