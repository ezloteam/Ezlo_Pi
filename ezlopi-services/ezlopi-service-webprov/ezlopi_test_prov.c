/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file    ezlopi_test_prov.c
 * @brief   perform some function on data
 * @author
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_test_prov.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/
#if 0
const uint32_t test_version_num = 1;
const char * test_id_str = "105200073";
const char * test_uuid_str = "c9816ce5-fc1b-415b-b673-50912aeab5fe";
const char * test_hardware_type = "ezlopiesp32";
const char * test_cloud_uuid = "b9c0e91d-7614-48b7-b9cd-09eef78200b9";
const char * test_up_domain = "up.mios.com";
const char * test_cloud_domain = "cloud.ezlo.com";
const char * test_coordinator_url = "https://cloud.ezlo.com:7000/getserver";
const char * test_prov_token = "A0S9AJl7VAbzdQ65qUIuORpTMOy8PhFEo8I1yJDY2wNi4eXWmahg7M53rFz0qGJI3tUxSK3QNisK28fbGUVesd0rDaiJwHEgpVNZcXJRgsFX5CWQEzRjao1HKHptVbMhYSVQOiCgHZKrgCxKioQy9aHLJdAXNCmnhjYIKXTiykIIWPpULS26tsTW9idUQtDYJ1nbe23fO7S7ptcBedyOiGrqwHbHt7LvZOqmZr8IdnBqGTAiKwaO8mQ8XAVZr3Jo";
const char * test_ssl_pvt_key = "-----BEGIN PRIVATE KEY-----\nMIGEAgEAMBAGByqGSM49AgEGBSuBBAAKBG0wawIBAQQgKEOiAyYfUugjNQ74G1+5\nj/qg3963yogjjrYcXdSeANmhRANCAARuRBnCMSEJXlP1KHi9VyrdK8EYsN+37cIj\nW3deH7goPVRjMCjUpQq/FqNROT26cN2Wm/ah56wRhyVREDv8towZ\n-----END PRIVATE KEY-----\n";
const char * test_ssl_pub_key = "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEbkQZwjEhCV5T9Sh4vVcq3SvBGLDft+3C\nI1t3Xh+4KD1UYzAo1KUKvxajUTk9unDdlpv2oeesEYclURA7/LaMGQ==\n-----END PUBLIC KEY-----";
const char * test_ssl_shared_key = "\n-----BEGIN CERTIFICATE-----\nMIICCjCCAbECAwTSdzAKBggqhkjOPQQDAjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\nBAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\ncHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\nhkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTAgFw0yNDA2MjUxNTUyMzhaGA8y\nMjk4MDQwOTE1NTIzOFowgZAxCzAJBgNVBAYTAlVTMRMwEQYDVQQIDApOZXcgSmVy\nc2V5MRQwEgYDVQQHDAtlemxvcGllc3AzMjETMBEGA1UECgwKY29udHJvbGxlcjEt\nMCsGA1UECwwkYzk4MTZjZTUtZmMxYi00MTViLWI2NzMtNTA5MTJhZWFiNWZlMRIw\nEAYDVQQDDAkxMDUyMDAwNzMwVjAQBgcqhkjOPQIBBgUrgQQACgNCAARuRBnCMSEJ\nXlP1KHi9VyrdK8EYsN+37cIjW3deH7goPVRjMCjUpQq/FqNROT26cN2Wm/ah56wR\nhyVREDv8towZMAoGCCqGSM49BAMCA0cAMEQCIBc3wYiPqA1elThngCwp3spdoMUy\nxIJD/5DiIfCmGyI2AiANkW5jll6/Y2A/Iq/TRRh68PYxYzAq/kcnPY5mod6ctg==\n-----END CERTIFICATE-----\n";
const char * test_ca_cert = "-----BEGIN CERTIFICATE-----\nMIICbDCCAhGgAwIBAgIJAOByzaI7aHY9MAoGCCqGSM49BAMDMIGQMQswCQYDVQQG\nEwJVUzEUMBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzAN\nBgNVBAoMBklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExU\nRCBDQTEhMB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTE5MDUz\nMTE3MDE0N1oYDzIxMTkwNTA3MTcwMTQ3WjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\nBAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\ncHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\nhkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTBWMBAGByqGSM49AgEGBSuBBAAK\nA0IABHLQdhLDYsafIFY8pZh96aDGqVm6E4r8nW9s4CfdpXaa/R4CnjaVpDQI7UmQ\n9vVDGZn8mcmm7VjKx+TSCS0MIKOjUzBRMB0GA1UdDgQWBBRiTl8Ez1l94jaqcxbi\nyxkVC0FkBTAfBgNVHSMEGDAWgBRiTl8Ez1l94jaqcxbiyxkVC0FkBTAPBgNVHRMB\nAf8EBTADAQH/MAoGCCqGSM49BAMDA0kAMEYCIQD7EUs8j50jKFd/46Zo95NbrPYQ\nPtLTHH9YjUkMEkYD5gIhAMP4y7E1aB78nQrmd3IX8MM32k9dM8xT0MztR16OtsuV\n-----END CERTIFICATE-----";
#endif

#if 0
const uint32_t test_version_num = 1;
const char * test_id_str = "105200122";
const char * test_uuid_str = "489b8299-3732-4d10-be41-0a87eeac0579";
const char * test_hardware_type = "ezlopiesp32";
const char * test_cloud_uuid = "b9c0e91d-7614-48b7-b9cd-09eef78200b9";
const char * test_up_domain = "up.mios.com";
const char * test_cloud_domain = "cloud.ezlo.com";
const char * test_coordinator_url = "https://cloud.ezlo.com:7000/getserver";
const char * test_prov_token = "RxHR3ZGakPAUtF4X1kMcE7Nh1fYIirybHFW4xHakVOPO6K5ququTpNhweCwCdTXYZgCtEdhaoWfjuNGqWMRQ0OdvTkNpHCUDoDNOOsLzgIae8Cj2Rc86DQfjmxSqpyzkmdgaS3NjSta0aItHmnerVmlGrNzaf5lIySCKle2xPRwnZCtfxbXx2jQcZCB2rQlVqzlzhkx9mjOooxbZOq8yDv5JG0jft5VAoBPodQqiYroP2boglcOrpSKqWHPqWFoX";
const char * test_ssl_pvt_key = "-----BEGIN PRIVATE KEY-----\nMIGEAgEAMBAGByqGSM49AgEGBSuBBAAKBG0wawIBAQQggy9tDxEIv7/Ivo3fdOR1\nCizjcEUv5INpcofnFjt1JTWhRANCAARaGjRCMlJMiPaa+yLQCEUS/3OlWFXbdFbO\nmLTlOZz02wCWY+gui2sSP+YFuM8fX2EEAlATJ5G0uNJBY9goIXvk\n-----END PRIVATE KEY-----\n";
const char * test_ssl_pub_key = "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEWho0QjJSTIj2mvsi0AhFEv9zpVhV23RW\nzpi05Tmc9NsAlmPoLotrEj/mBbjPH19hBAJQEyeRtLjSQWPYKCF75A==\n-----END PUBLIC KEY-----";
const char * test_ssl_shared_key = "\n-----BEGIN CERTIFICATE-----\nMIICDDCCAbECAwTSqDAKBggqhkjOPQQDAjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\nBAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\ncHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\nhkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTAgFw0yNDA3MTAwNzUzMTVaGA8y\nMjk4MDQyNDA3NTMxNVowgZAxCzAJBgNVBAYTAlVTMRMwEQYDVQQIDApOZXcgSmVy\nc2V5MRQwEgYDVQQHDAtlemxvcGllc3AzMjETMBEGA1UECgwKY29udHJvbGxlcjEt\nMCsGA1UECwwkNDg5YjgyOTktMzczMi00ZDEwLWJlNDEtMGE4N2VlYWMwNTc5MRIw\nEAYDVQQDDAkxMDUyMDAxMjIwVjAQBgcqhkjOPQIBBgUrgQQACgNCAARaGjRCMlJM\niPaa+yLQCEUS/3OlWFXbdFbOmLTlOZz02wCWY+gui2sSP+YFuM8fX2EEAlATJ5G0\nuNJBY9goIXvkMAoGCCqGSM49BAMCA0kAMEYCIQC34GH4R7trD1F4+v3jQT+cvpG3\n047g5SO5OdtdxOMKJAIhAKsQs+g1Xk3YLmzLKvWH9z1zLNIy+tuKPG80ixYi2nFm\n-----END CERTIFICATE-----\n";
const char * test_ca_cert = "-----BEGIN CERTIFICATE-----\nMIICbDCCAhGgAwIBAgIJAOByzaI7aHY9MAoGCCqGSM49BAMDMIGQMQswCQYDVQQG\nEwJVUzEUMBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzAN\nBgNVBAoMBklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExU\nRCBDQTEhMB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTE5MDUz\nMTE3MDE0N1oYDzIxMTkwNTA3MTcwMTQ3WjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\nBAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\ncHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\nhkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTBWMBAGByqGSM49AgEGBSuBBAAK\nA0IABHLQdhLDYsafIFY8pZh96aDGqVm6E4r8nW9s4CfdpXaa/R4CnjaVpDQI7UmQ\n9vVDGZn8mcmm7VjKx+TSCS0MIKOjUzBRMB0GA1UdDgQWBBRiTl8Ez1l94jaqcxbi\nyxkVC0FkBTAfBgNVHSMEGDAWgBRiTl8Ez1l94jaqcxbiyxkVC0FkBTAPBgNVHRMB\nAf8EBTADAQH/MAoGCCqGSM49BAMDA0kAMEYCIQD7EUs8j50jKFd/46Zo95NbrPYQ\nPtLTHH9YjUkMEkYD5gIhAMP4y7E1aB78nQrmd3IX8MM32k9dM8xT0MztR16OtsuV\n-----END CERTIFICATE-----";
#endif

#if 0
const uint32_t test_version_num = 1;
const char * test_id_str = "105200134";
const char * test_uuid_str = "268e1b6c-6067-469b-ac38-f5803c99d926";
const char * test_hardware_type = "ezlopiesp32";
const char * test_cloud_uuid = "b9c0e91d-7614-48b7-b9cd-09eef78200b9";
const char * test_up_domain = "up.mios.com";
const char * test_cloud_domain = "cloud.ezlo.com";
const char * test_coordinator_url = "https://cloud.ezlo.com:7000/getserver";
const char * test_prov_token = "IkX1kaNGvQpXFxBlKpyNsogfX6Xw1QC3Cez3ldqtxMRurRlFS2jwEFbyquh0QCTu1zMkmu1IqXphbGnMLNuLZDsATNzkAGc8reMHqQsYBONwvV46ragTeqmWxO0RYOaqcaOBxzMXhUFnK4BxDFGvxbdRgyRCqkQ6tu0nKILEcXTlHgyYEy7C8fdlFlyt5GvMQll4xnGtZQQBBGfAZZoKOIk7LF9FdR7U3HejaboHXAbecAe6etHUFjWgVtDc3sKg";
const char * test_ssl_pvt_key = "-----BEGIN PRIVATE KEY-----\nMIGEAgEAMBAGByqGSM49AgEGBSuBBAAKBG0wawIBAQQgOZMj8S5TVN+X2jHWZMFf\nzXufz2FpxCwT3boBoZ5j2jKhRANCAASmiS1zy2d//k+1SF7Ii2Nj3G573uXCJarD\n1dAusa2kCl3uoAvUaMsxAOUoSRRBuqyvVUfBKFV/dfbKdOHTCJ3j\n-----END PRIVATE KEY-----\n";
const char * test_ssl_pub_key = "-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEpoktc8tnf/5PtUheyItjY9xue97lwiWq\nw9XQLrGtpApd7qAL1GjLMQDlKEkUQbqsr1VHwShVf3X2ynTh0wid4w==\n-----END PUBLIC KEY-----";
const char * test_ssl_shared_key = "\n-----BEGIN CERTIFICATE-----\nMIICDDCCAbECAwTStDAKBggqhkjOPQQDAjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\nBAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\ncHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\nhkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTAgFw0yNDA3MTgwOTU4MzFaGA8y\nMjk4MDUwMjA5NTgzMVowgZAxCzAJBgNVBAYTAlVTMRMwEQYDVQQIDApOZXcgSmVy\nc2V5MRQwEgYDVQQHDAtlemxvcGllc3AzMjETMBEGA1UECgwKY29udHJvbGxlcjEt\nMCsGA1UECwwkMjY4ZTFiNmMtNjA2Ny00NjliLWFjMzgtZjU4MDNjOTlkOTI2MRIw\nEAYDVQQDDAkxMDUyMDAxMzQwVjAQBgcqhkjOPQIBBgUrgQQACgNCAASmiS1zy2d/\n/k+1SF7Ii2Nj3G573uXCJarD1dAusa2kCl3uoAvUaMsxAOUoSRRBuqyvVUfBKFV/\ndfbKdOHTCJ3jMAoGCCqGSM49BAMCA0kAMEYCIQCGvQYojLszCcYp0TlmgW3VblYs\nLmxjs5UkQlaqirYfjwIhAMZQRuzdj55YdNsqIyKjFq6xR/N5fA70OR5a7qfXds6p\n-----END CERTIFICATE-----\n";
const char * test_ca_cert = "-----BEGIN CERTIFICATE-----\nMIICbDCCAhGgAwIBAgIJAOByzaI7aHY9MAoGCCqGSM49BAMDMIGQMQswCQYDVQQG\nEwJVUzEUMBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzAN\nBgNVBAoMBklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExU\nRCBDQTEhMB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTE5MDUz\nMTE3MDE0N1oYDzIxMTkwNTA3MTcwMTQ3WjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\nBAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\ncHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\nhkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTBWMBAGByqGSM49AgEGBSuBBAAK\nA0IABHLQdhLDYsafIFY8pZh96aDGqVm6E4r8nW9s4CfdpXaa/R4CnjaVpDQI7UmQ\n9vVDGZn8mcmm7VjKx+TSCS0MIKOjUzBRMB0GA1UdDgQWBBRiTl8Ez1l94jaqcxbi\nyxkVC0FkBTAfBgNVHSMEGDAWgBRiTl8Ez1l94jaqcxbiyxkVC0FkBTAPBgNVHRMB\nAf8EBTADAQH/MAoGCCqGSM49BAMDA0kAMEYCIQD7EUs8j50jKFd/46Zo95NbrPYQ\nPtLTHH9YjUkMEkYD5gIhAMP4y7E1aB78nQrmd3IX8MM32k9dM8xT0MztR16OtsuV\n-----END CERTIFICATE-----";
#endif

// const uint32_t test_version_num = 0;
// const char * test_id_str = NULL;
// const char * test_uuid_str = NULL;
// const char * test_hardware_type = NULL;
// const char * test_cloud_uuid = NULL;
// const char * test_up_domain = NULL;
// const char * test_cloud_domain = NULL;
// const char * test_coordinator_url = NULL;
// const char * test_prov_token = NULL;
// const char * test_ssl_pvt_key = NULL;
// const char * test_ssl_pub_key = NULL;
// const char * test_ssl_shared_key = NULL;
// const char * test_ca_cert = NULL;

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
