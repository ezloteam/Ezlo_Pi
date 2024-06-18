#include "ezlopi_test_prov.h"

const uint32_t test_version_num = 1;

const char * test_id_str = "105200033";
const char * test_uuid_str = "ac0ae169-2568-456d-bdee-c9276b05bcf0";
const char * test_hardware_type = "ezlopiesp32";
const char * test_cloud_uuid = "b9c0e91d-7614-48b7-b9cd-09eef78200b9";
const char * test_up_domain = "up.mios.com";
const char * test_cloud_domain = "cloud.ezlo.com";
const char * test_coordinator_url = "https://cloud.ezlo.com:7000/getserver";
const char * test_prov_token = "Ur89ye2qz4ST6ojOU9XwFo2HRu92nguloYVyKJkorRcl7wjJbtNf9P6daCtAYdi3PDmqL4qt0Zzo27Iq16NqLeengWyuqqjAuj1UF2wlf1UHgP1ihgXHXgWUmpDTmFQUeDfAiRkLtmDfZ08xBx2WCZv3kuA2mRNiXbxdd5orl1cPBtb2fcY9mWj4oLTF3EjgyYk7rZ0stJVf5q3wYys9Nq279A32X8DpDAU7gjJ1dd8Z46lVufXOCKjU586hmC32";
const char * test_ssl_pvt_key = "-----BEGIN PRIVATE KEY-----\nMIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDKUTAMt7SKN1kK\nLNKv0U4NAn9MwMuJJv6vxtlXoi4PBycT6C1aSDVe7e9nAc6FiIJaNnPzybU+iqO/\n2lDvPebF/zZrEf9BOMlplAWnrJ7MkkwqSjpEubd+KvbWPX/tW5pos1HYZxYvGJCy\nlPhlqkkfWaOQS+j58SDzB0M8czYwHSV5XgPeE5n1v12REBOFLUYm178z7nCdwF21\nXFDtBwmAChz50V+ISVgiMMCRtCkv1ug3m+1vyr/50eWpRNtlS7+xPs/mJq/hWDl1\nErD4hp0pUdFODZAwwBDQQFhHmWShy4riu5Deuq01SrpDSEzpxuBranuoUW39Cbb6\nzBKyyRyxAgMBAAECggEAHUDprd++4FYKGba1zMv6uVaD71EDPR92fRZyD8H4tJ3n\n+xtmZLhYgJOZOwTfsUmLRkhlsg7s65BNXBd7qL7ZHbD+KTOEJpCeTZX5zq8Jk8fl\nm8nO12geeET1xbBFIp4iW1hDvILaCiZG/zjGGqEpiznBX5hyT/hF4jjJx+Ba1KWS\n+p98Qp3m/YdOi+R1F8Gim6sAL7pv+mG9AbWZuPYVlB0bKZ6Ekh3yGaRcbSFRfutj\nWMu7vLPrVm/UPj3mHehPRrbwVFMXJngcpNhKy3nYSNWX9aMBcDSlDOy5FF4R1rao\nIPBjoljtTjgvnjhRqiSJvXSDqEULhsxPbANDy3DjjwKBgQDxlTx1iMZLXWzt80F4\ntNteJKRgF0fX81V1+LDdJ3KRe/vK0F04uEwRcJiaKyx14yX9jg9ireCWvTE+q9wC\ndifTbNNXY07KgNRoHWXopAJgNZ4dRnvQNU466SmbSwY2/Ks6IX/mpQyCMBYQzO7r\natwF0a9LDh1RzLk7SMV5f5F2QwKBgQDWZBI91r2bJz36oGC9MsChqp5/4AaDNigW\nFl7hMMI9imydRX2VceCNnXgXFnELd1CrAni/f0zVMlSM35JumUifXjV+SQUEN7p9\nJFbA6EEMhJnZzvuI4jSuC2Kh9cR/dQ2LT+LUAufOI2irpghWqepgE/js6IKvRby/\n2tqnjZUj+wKBgGpruXR1ShUNFN1FmYPY+AHi1l5WIxnoYa9UXGUv6XJZUbQoUciY\ntzm/hhn0BuPvql6l4/Uub483zKe1AXg8IJMsj3UhlCX01pz+xlPTXBCRzhNLZ2Ny\n+AFPkTqNtDEo4Sz8cOFjt7zp/e5suJsQOdh+ZGAF2bidHMQW7Jb3MVcNAoGAKobf\nN3UJV2WJXdXCWsVegLMfja4anJx5hygY2gKaBSsYMHpAVwynxm+eCrZg5i1cyw9K\ndHIEA6ZgLzGKEnZFeZdudQZ3AEctrvUuYEWx4ZHMEvCD5Ls6R+vqPMZLnBzT+vZ1\nwChzYuiTMrlUnhxGaZiMoDHe+nWS41u+U84571ECgYB4D1sCy1KdfCoxJZKrWT9t\nOxl5wMTJ+gCw1fWtYn0KwU/Q/9JSZcauHD2ZsBKbUgknLXU/gRWKFHk8wOsahYt7\nFPZMO5osgCX5a5EL0nGqHaPbLL3aeFXeYj6hMZLJVvltChBC5Tw3jPkZLqbJbqDo\ny06lyBP48KnpHBCy23dCsw==\n-----END PRIVATE KEY-----\n";
const char * test_ssl_pub_key = "-----BEGIN PUBLIC KEY-----\nMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAylEwDLe0ijdZCizSr9FO\nDQJ/TMDLiSb+r8bZV6IuDwcnE+gtWkg1Xu3vZwHOhYiCWjZz88m1Poqjv9pQ7z3m\nxf82axH/QTjJaZQFp6yezJJMKko6RLm3fir21j1/7VuaaLNR2GcWLxiQspT4ZapJ\nH1mjkEvo+fEg8wdDPHM2MB0leV4D3hOZ9b9dkRAThS1GJte/M+5wncBdtVxQ7QcJ\ngAoc+dFfiElYIjDAkbQpL9boN5vtb8q/+dHlqUTbZUu/sT7P5iav4Vg5dRKw+Iad\nKVHRTg2QMMAQ0EBYR5lkocuK4ruQ3rqtNUq6Q0hM6cbga2p7qFFt/Qm2+swSsskc\nsQIDAQAB\n-----END PUBLIC KEY-----";
const char * test_ssl_shared_key = "\n-----BEGIN CERTIFICATE-----\nMIIEhTCCAm0CCQDh/5jSCiKSIjANBgkqhkiG9w0BAQsFADCBgTELMAkGA1UEBhMC\nVVMxCzAJBgNVBAgMAk1BMQ8wDQYDVQQHDAZCb3N0b24xEzARBgNVBAoMCkV4YW1w\nbGUgQ28xEDAOBgNVBAsMB3RlY2hvcHMxCzAJBgNVBAMMAmNhMSAwHgYJKoZIhvcN\nAQkBFhFjZXJ0c0BleGFtcGxlLmNvbTAgFw0yNDA2MTgxMjI4MzZaGA8yMjk4MDQw\nMjEyMjgzNlowgYQxCzAJBgNVBAYTAlVTMQswCQYDVQQIDAJNQTEPMA0GA1UEBwwG\nQm9zdG9uMRQwEgYDVQQKDAtlemxvcGllc3AzMjEtMCsGA1UECwwkYWMwYWUxNjkt\nMjU2OC00NTZkLWJkZWUtYzkyNzZiMDViY2YwMRIwEAYDVQQDDAkxMDUyMDAwMzMw\nggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDKUTAMt7SKN1kKLNKv0U4N\nAn9MwMuJJv6vxtlXoi4PBycT6C1aSDVe7e9nAc6FiIJaNnPzybU+iqO/2lDvPebF\n/zZrEf9BOMlplAWnrJ7MkkwqSjpEubd+KvbWPX/tW5pos1HYZxYvGJCylPhlqkkf\nWaOQS+j58SDzB0M8czYwHSV5XgPeE5n1v12REBOFLUYm178z7nCdwF21XFDtBwmA\nChz50V+ISVgiMMCRtCkv1ug3m+1vyr/50eWpRNtlS7+xPs/mJq/hWDl1ErD4hp0p\nUdFODZAwwBDQQFhHmWShy4riu5Deuq01SrpDSEzpxuBranuoUW39Cbb6zBKyyRyx\nAgMBAAEwDQYJKoZIhvcNAQELBQADggIBACfE9Yja3qFJg4kfm7rUiv78YnIhGrFG\n2prvVHFRn2NHMXZuMChv2izAbSHO5wG5f73mtBO6cbnckPf3EWDIb1ZTZT3BJhjB\nRBgsAZ9LIBUosM9MCrbpwWWDwt/zEqmPHNHoeFSmNSK4KJWZFjiaxNfUaXC5YRPu\n85/QvuYwsB7NRXR4m3CXK4iYvgsYj+yTGWRQ8CfzJnObZPOodgWj8OYWo7e6c9C2\nyWhA7a+ELxPNfelwi55IoIEaQ6aQ5qgQVjdoRVDHPL8xhB67u55258Ufber1IgTh\nrWj+PcYqpqm2f5s69UEqJ/HQ4Rjo0aG5hxb981DWUiuF5ucsap7NfuK34WGxHyL0\nyrSpdkB5E9kONbFF5fqKXbvLafxkXy7ijSBNAiCRSZwPr98Xd8nTGq0+M1s8rdjI\nrCc4IO5BNFrY+PBykfWCUJwnKgJTvmie3vs24e9T3pZfz42s+Luc2QU+0FlLO41q\n3UtWycWZayUvMvp0UkffBeHgWHAsfCPMgUHBXgFWUVbOqCDYrUVQJQwCf8of+S0t\nnDPcb6Rvqi2/otPg3PjeYM2SHw9ZNSjYQDBVptzm96xH1k9lNdDagV9zBG2gxbWT\n6krNOGVnCIKyomtwAL7bGlT04YEW5c2byziVpMzn0zbBTXhtwTedm1CHdqq5yiNT\nWUX9svchqwk5\n-----END CERTIFICATE-----\n";
const char * test_ca_cert = "-----BEGIN CERTIFICATE-----\nMIIDnjCCAoYCCQDdqj/mAotZbDANBgkqhkiG9w0BAQsFADCBkDELMAkGA1UEBhMC\nVVMxEzARBgNVBAgMClNvbWUtU3RhdGUxEzARBgNVBAcMCk5ldyBKZXJzZXkxEzAR\nBgNVBAoMCkVaTE8gLCBsdGQxDDAKBgNVBAsMA0lPVDERMA8GA1UEAwwIZXpsby5j\nb20xITAfBgkqhkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTAeFw0xOTEwMDcx\nMjIxMTlaFw0yMTEwMDYxMjIxMTlaMIGQMQswCQYDVQQGEwJVUzETMBEGA1UECAwK\nU29tZS1TdGF0ZTETMBEGA1UEBwwKTmV3IEplcnNleTETMBEGA1UECgwKRVpMTyAs\nIGx0ZDEMMAoGA1UECwwDSU9UMREwDwYDVQQDDAhlemxvLmNvbTEhMB8GCSqGSIb3\nDQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\nMIIBCgKCAQEAvwfe5F1GIVrHTYfhtJrvTu1JsUaciuri3PcCrI5QsQkEp/tJdOdY\njeo4zBi6HtANCoV4+0X84MOiM9nghQUhv8fAgdSjpQhW6Wzom7EUzAZs017erC0a\nYPKe24lTbYnkoWpFBBJ4CYMZtU52iY3gwHpgKgqbepxwUik0BjXsave66edjdsin\n1cuSiyEMlZNrC+jgUa0rE7rrI26XMIOrTnE1BPXnWnBMn9bpEONL3SWC1Cs/tDJ7\nse0dGwvCr1IK7wqlOhIKx7BdJJoFxmUxtS+h/ZhDoYX+sAyKjWTNeH8WjJdQ+Elw\nJnYydGziM2tdYLFigYYdnNrpW7ibk5ZSVwIDAQABMA0GCSqGSIb3DQEBCwUAA4IB\nAQAS1yKVHY/GImMCfZPMIASoPpxcOFybbEBcfSxGDp0tDg7eA4A2v+RaNdMEC6Lx\nbZKtW54Va2wjIntYeMzcF1VQrdo/OzqlbdvMXJ4/ENmy9LTOOsYQBjjmIfWtMc5r\nQKQRQPUqmUrFjrCZWskEw+TIY19EoDAdmI9eWkFTn9N4JC8/aGPhDC3DSGAjq0sP\n6NXkwX5Kti9DgmaVWsMa9E8jdYKeazGZaCHp4qKgUREAZE2jQzmN9nYe5MWOCIZK\nQDb5gCNcLmMHP6o4HLcOABXqpzTu1kNEeXpbDwYru77pFikCHa/XP4pXojLy0QMP\nIv69lP5ZQwIobb+D/tlXGoBy\n-----END CERTIFICATE-----";