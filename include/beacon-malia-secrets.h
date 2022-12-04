#include <pgmspace.h>

#define THINGNAME "beacon-malia"
static const char ID[6] = "malia";
static const int RED = 255;
static const int BLUE = 125;
static const int GREEN = 0;

// The MQTT topics that this device should publish/subscribe
static const char *AWS_IOT_PUBLISH_TOPICS[] = {"beacon_beauchamp/pub"};
static const char *AWS_IOT_SUBSCRIBE_TOPICS[] = {"beacon_beauchamp/sub"};

// Device Certificate
static const char AWS_DEVICE_CERT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUFnKi9Yjr2nFi+8g607yO1ahqTuYwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIyMTIwNDIyNDkw
M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM0ZtebJ92JfSgC1SeTB
YNCbIVxXF2F5CwpDX+Jc/4IFZjnWKTsmthuATAA3iM8YGfN6wbxuj25pgBM7gnZJ
IOLtlVyEWC7PJfvWbvyItLR0l2aDTFygLrBuhzyP5wk7v8toaevTox54pN/kv373
RSiEFjnTjiO8Hm0JamehL4eIe7KoVEitgcSKo/shJOePMgheDhFajNdHy+xS/sfF
kkJR/vUsOcbPbHy+M2KzwPFRJBLoHT9Sv7Cn7cZqu99AFaOtqA2vnduJivldgWVb
JEsE5ffbrE04LQq2IoYJ7IKBRAmk1+on34bc8L7m/dTk9G+AauLLj54HOjvjSUIG
mncCAwEAAaNgMF4wHwYDVR0jBBgwFoAUELH7L42HkkvQqo/Gu4gYsxBFDtIwHQYD
VR0OBBYEFNyyhpLrzvgbsKvjBFGRWhE2L0dGMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBR1/yfn2PJQL3MZLMEMV4VF9uT
TqlvJKn//mBfgnEePvvqnvvxLpigBgAwyyxbe05AmT9bBzZjTzo67gwEbv/ub/NG
Qw9vMX6K1luodztnIYgq2E2kNWHyrLghWcwywIracPyjeg2e8EI/EWlT2ivhYPQX
o2jMj6xQ1ZfTnVp9+lWkymZVPMQD7rshjnIUUzlcX9jlmKQoxVEpJrMumEPH7RF2
QkgRs6yNkXtZFcum+bjNO7rmjTO2WYI2Hv1xWH+QIFxe5gmYI+PNG6lhwr8Syw4l
uu427+V4mH9KSVeRk1hP5d0i12dHkezHHWOveTuS+jIW/UnYHPOLpYaVWgkp
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAzRm15sn3Yl9KALVJ5MFg0JshXFcXYXkLCkNf4lz/ggVmOdYp
Oya2G4BMADeIzxgZ83rBvG6PbmmAEzuCdkkg4u2VXIRYLs8l+9Zu/Ii0tHSXZoNM
XKAusG6HPI/nCTu/y2hp69OjHnik3+S/fvdFKIQWOdOOI7webQlqZ6Evh4h7sqhU
SK2BxIqj+yEk548yCF4OEVqM10fL7FL+x8WSQlH+9Sw5xs9sfL4zYrPA8VEkEugd
P1K/sKftxmq730AVo62oDa+d24mK+V2BZVskSwTl99usTTgtCrYihgnsgoFECaTX
6iffhtzwvub91OT0b4Bq4suPngc6O+NJQgaadwIDAQABAoIBADhYG9SvHzrzGo1f
QFGLnmEwfNW1R0ML0bCcGbdB/R9nBFJXNzR8ygJ8GGTm3Lqn0FlHG/n8FYMrkHe4
HXsJU/ODeUjVQrR5S+IHYq4VkC8S6JTM9SltB+aINra4gavTsjjbEl4q4eD5G7I2
UTU8EM+0mbaBG8dNYr5pm8WkTj5ZDgDmhp2rMGHmXGh9ENt+MDF5LcrOdahO+1Wt
SuzW6A4SxX4h0hhRJ++v7BZqbsf9wnt/Dt7wPh4HOimVu/b47RJ3TXCx0LAJGgwF
vmKIgvBXps9UsEpKZcD7ALRMsD2rieWsPIef30Rp2Jk9mGpadTI7MJcfSNMnLdE/
Orc4peECgYEA9w26QyCMOpKMkrqea65g5Kglgl6pTYaErIGhLDfN651QOv2Gt4Ww
odWQLqJ8yHTT+9GpQa7sEwpIJ/VK2Wuhff2WEP8LgkZm1LZ98KRFEsgEGzppll1/
gWTaEMfnFAlIq7HxY/Dh+DIG8zY+4CxvaxS+bve6y1+evdQlde1Z1q0CgYEA1IcP
+N4qStHjs2wrrwjZgRTIkRn53ye+ziGZhx040YPr7mJbKxbtwrw52h4GUw5BSctD
JYgcGDo7teFHZDuyjii7R//Okxww9VItDbUZ27HVEPCVo4QSwwhqWYfMq58akhGn
7sWCjKqldeWeQLHNh88e/lS+S/kE4cCuWw887jMCgYEA0bjzj5yHrglOEBkf9uVX
F4SuItfc/RTed9qrSXAt/H/U5d7mh9CTuy4icZQ6Paj/c1vuRPYLLbaivlj6q0s1
ElE9HTyFNpgxHw20J54rjOACrGKP07eXT+6fN3ObdGTVLfumoWKRUTKWt8wq3/6Y
rtwzNNxT0+VeSmNXUZ6ljxkCgYBP3K0I1NtJoFOTUbucm5O4KMoxlHRMAhB2PzQF
DNsUCCG5Q6aEq6oOulJ2/z9bKIkqJMKHsPz1sbRMZWpBZRl0yCOQK7VBnJ9dIFMj
yoD8CHhLVEy21GOEfH4SRLCJcVE3wIC9feTP3TQt+GoeX3IfbZp08f1MaTgHaTNb
rn8z9wKBgAaZO70cTzNMoxD5fDEUpAPXxL/v7UIz7w5NczyM5u4y1v2TdkuhOC7u
34kv5V3azVPVMEAe44RF2dQ+A+Ns1IEQYTeZ4UongBfAH8FnfbRelICygw2yi/n7
Hp+wdP5BDkdQhv56aylwY9XkGIrrrzubfmrLwudbUWc4b9TVrKKS
-----END RSA PRIVATE KEY-----

)KEY";