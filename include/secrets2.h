#include <pgmspace.h>
#include <color.h>

#define THINGNAME "esp32_button_test_2"
#define ID 2
#define COLOR Yellow

const char WIFI_SSID[] = "DucksBeakTheBears";
const char WIFI_PASSWORD[] = "ChampKlein90";
const char AWS_IOT_ENDPOINT[] = "a23wplj2ed83c6-ats.iot.us-west-2.amazonaws.com";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVALkHYX68d2fc0OnFmZNUFTMoLReZMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMTExMTgwNDE4
MTFaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCyWycCrsjrGUuL77aT
e6PCK7cC3IDXZX+O8JsNkEYdH3ig3bdFLRoez/+LjH7FpRn4gQQnEFCjFRwTvoNP
IJ4gD1Ne3gkG3jvj09p65QSsFI4EfymeY9KQB5J8jZZMMzHk1WHxrgZzsSgu0huk
T1bEyJTK9rKwN/0wki3AzRVLnMcOgAot9Mhp2heV5+jw2qjHOc/qeRif4sP2AAIm
9QncH4VULDaT/huRjBP6jUMfMd8m13xMeKGeTYPIXaVZ7otbEO9uURRzrOsrOs2P
ILBOnu6+16mMsWEhRLH+g2q0s5PxR5rghRrQKW8U6tZ4oyp8xs1z6wqCZPmclQID
8nhrAgMBAAGjYDBeMB8GA1UdIwQYMBaAFEi/H0NaAvct7pXIQKCubLwiDjwHMB0G
A1UdDgQWBBQLnV9bFeVrZGeH9qNNRutqwZtgpTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAFtSGc9KJnqWr+icGnJoQdeFO
SRITaWAYaT0Vt4TOnUB5t0rn6cuUSoahIl8SeOKDSyWOsQYEwVuCEGDdCobzSXs/
pVn7GEvOyGRLQTB3F0o3FM0kszDsTIwh+DzGDTwCGad8o4hklr4NrDi5A+33Kpe4
0Sp6jYQJJLVGh2T3QM/u+3WvFilOBajiCvECnJw8zBNwsvcD8OmtkqAjPJYLa1Nn
t4J0V2VZy5R621lZvnrDLnPtVLAuKe5oB7V0EVEx4z16lBO0cizrJ1YMIWoe8CWZ
QuLfWVmDqMZp38kM46LpxMPxln0ETihQ46YZjJyJfMJ8il6uN6cz4jLtWZqwHw==
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAslsnAq7I6xlLi++2k3ujwiu3AtyA12V/jvCbDZBGHR94oN23
RS0aHs//i4x+xaUZ+IEEJxBQoxUcE76DTyCeIA9TXt4JBt4749PaeuUErBSOBH8p
nmPSkAeSfI2WTDMx5NVh8a4Gc7EoLtIbpE9WxMiUyvaysDf9MJItwM0VS5zHDoAK
LfTIadoXlefo8NqoxznP6nkYn+LD9gACJvUJ3B+FVCw2k/4bkYwT+o1DHzHfJtd8
THihnk2DyF2lWe6LWxDvblEUc6zrKzrNjyCwTp7uvtepjLFhIUSx/oNqtLOT8Uea
4IUa0ClvFOrWeKMqfMbNc+sKgmT5nJUCA/J4awIDAQABAoIBAEd/gwPwTVrxqPdQ
AUDeL6eHHrZm7r9qUNhcW8v0KidClnjmaLGyguYXO7oVeDyDEwYrsh52y9yUeczw
cnMmAD2Z5zIAkF3QePG727moVVcmZkgnMefs3FwgrPiBSd8CJt41LC5WNP/r5M/g
KnST2ZTJU9yJSNibmUP/VZgUnpTQeJS9KCYaqitQoUUVoJhhP1yExXUGzcTT2iV4
J7yvzQCPPE+XioQCLHof4ovcE5btZqLkgGBYrixvDticmBhdPNum5M1ryIGKZGVS
y3/aH/atNCaKfZLLgBfgl4movNHWQcWtMoAeyP6Viz6UXsA/9jsPrhV8X34LeqQQ
lLC0mgECgYEA6AtapftjUKCN0ugIgp0xzVcTHEV/vh/OJKPFvyJPClvYwNSHtxz8
SF2kEHUMlWoLgXshdV3sjtOAxuKSL28dIkaVNeu3ygWGy/5PNnn+OaRuYSNSDGHz
S3BUwIvu8qiMkQp7VWG08amzZde25OA6Q3w9RzaJ699GTmaE8YUlKFMCgYEAxMTi
Edi89qL+0MjHBdbqazTdt8IqR/1qEdKEWFSONKl5ABERESAy7l3hf4Jl3iph8j9n
GycNpZtuTdEaf7DspLS4NlmW5PXqgd4d8LQa+1WPPUdYXFX/EeqdyR+ib/4/JE4s
stAHQ15KSUbfd8lgk3v1Ur1u8mnzZdY56xCkDIkCgYBh8gD2VNFMBCk5IAYPk+ik
ox+G7HMKPoSWBKcAkavJVJF+e3c5vLtklFofNDj2jIeExQOPdfDXLMGLsbFA4mbq
w95Rn0i5tnF6g8LN3NqJXDXXPMByR5oZerXbfHWE6c5DnqgGlzJ4W0hFMbOakmcl
ezimaYf4DwuvJijxhWD0zwKBgQCbFWXslPuSFDNr0OLz2kiWSGt7emAirzubYmWG
zNXr8ap5f/oImm0p6/pfqiGpsZPS/1+/Sc96t/9++5C8FXDd+8W4qHHCcYPr6WmF
nh4p+2xS6tlcpTbNIzJLI1C+ng+cCLU4FbezxCwehup4GdKxSyC2nrFEBHeeyvKJ
L2tsqQKBgCA+VH5kNEOnT8czm23K+u3GFbCySmH6c+a7sBFz4yJqdMVLOyF9iVQD
fR2YurkYhTgTGxUFDn5q79CuB7xm6Am23MsMZb9Jtt49RuhaiY8UaVx8bDbgZKsd
Og4OSrnCjXayt2uO9eFmKoLUf5r+y5rw/EuIcff7Qq1TBMi2XYB1
-----END RSA PRIVATE KEY-----
)KEY";