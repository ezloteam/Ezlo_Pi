#include "certificates.h"
#include <string>

const char *ca_certificates =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIICbDCCAhGgAwIBAgIJAOByzaI7aHY9MAoGCCqGSM49BAMDMIGQMQswCQYDVQQG\r\n"
    "EwJVUzEUMBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzAN\r\n"
    "BgNVBAoMBklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExU\r\n"
    "RCBDQTEhMB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTE5MDUz\r\n"
    "MTE3MDE0N1oYDzIxMTkwNTA3MTcwMTQ3WjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\r\n"
    "BAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\r\n"
    "cHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\r\n"
    "hkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTBWMBAGByqGSM49AgEGBSuBBAAK\r\n"
    "A0IABHLQdhLDYsafIFY8pZh96aDGqVm6E4r8nW9s4CfdpXaa/R4CnjaVpDQI7UmQ\r\n"
    "9vVDGZn8mcmm7VjKx+TSCS0MIKOjUzBRMB0GA1UdDgQWBBRiTl8Ez1l94jaqcxbi\r\n"
    "yxkVC0FkBTAfBgNVHSMEGDAWgBRiTl8Ez1l94jaqcxbiyxkVC0FkBTAPBgNVHRMB\r\n"
    "Af8EBTADAQH/MAoGCCqGSM49BAMDA0kAMEYCIQD7EUs8j50jKFd/46Zo95NbrPYQ\r\n"
    "PtLTHH9YjUkMEkYD5gIhAMP4y7E1aB78nQrmd3IX8MM32k9dM8xT0MztR16OtsuV\r\n"
    "-----END CERTIFICATE-----";

const char *private_key =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIGEAgEAMBAGByqGSM49AgEGBSuBBAAKBG0wawIBAQQgoEsMUpfqBYKpUIMA8p9P\n"
    "VeuCFDJTcbUX7XWy3LuhkTWhRANCAARJcwBTmNicaSP5QaylgoOiFqzd2reZg0Wi\n"
    "x4/T69xWtkXSs3Urz+VZ7g4N5Z3K1GqEbYSwljzgzUw0hhAOrcKq\n"
    "-----END PRIVATE KEY-----\n";

const char *shared_key =
    "-----BEGIN CERTIFICATE-----\n"
    "MIICCzCCAbKgAwIBAgIDA7IrMAoGCCqGSM49BAMCMIGQMQswCQYDVQQGEwJVUzEU\n"
    "MBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzANBgNVBAoM\n"
    "BklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExURCBDQTEh\n"
    "MB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTIyMDYyMjEyMDEw\n"
    "NFoYDzIyOTYwNDA1MTIwMTA0WjCBjDELMAkGA1UEBhMCVVMxEzARBgNVBAgMCk5l\n"
    "dyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xEzARBgNVBAoMCmNvbnRyb2xsZXIx\n"
    "LTArBgNVBAsMJGZkYWM3MzcwLWYyMjItMTFlYy1iYTY0LWFkYjZlZWU1OGM2MDES\n"
    "MBAGA1UEAwwJMTAyMDAwMDgzMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAESXMAU5jY\n"
    "nGkj+UGspYKDohas3dq3mYNFoseP0+vcVrZF0rN1K8/lWe4ODeWdytRqhG2EsJY8\n"
    "4M1MNIYQDq3CqjAKBggqhkjOPQQDAgNHADBEAiAXVG/0Q0BSxPhaXrsaSD3jzOxB\n"
    "hgtvjOzPtHXIMrS1+QIgfMXJWQtlqq+aVmcFaY289k4UBa4Y2apC1R8VvPveV9I=\n"
    "-----END CERTIFICATE-----\n"
    "";

const char *public_key =
    "-----BEGIN PUBLIC KEY-----\n"
    "MFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAESXMAU5jYnGkj+UGspYKDohas3dq3mYNF\n"
    "oseP0+vcVrZF0rN1K8/lWe4ODeWdytRqhG2EsJY84M1MNIYQDq3Cqg==\n"
    "-----END PUBLIC KEY-----";

const char *certificates::get_ca_certificates(void) { return ca_certificates; }
const char *certificates::get_shared_key(void) { return shared_key; }
const char *certificates::get_private_key(void) { return private_key; }
const char *certificates::get_public_key(void) { return public_key; }