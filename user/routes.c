#include <esp8266.h>
#include <httpd.h>
#include <cgiwebsocket.h>
#include <httpdespfs.h>
#include <auth.h>

#include "routes.h"
#include "cgi_wifi.h"
#include "cgi_reset.h"
#include "cgi_ping.h"
#include "cgi_main.h"
#include "cgi_sockets.h"
#include "cgi_network.h"
#include "cgi_appcfg.h"
#include "cgi_persist.h"

#define WIFI_PROTECT 0
#define WIFI_AUTH_NAME "wifi"
#define WIFI_AUTH_PASS "nicitel"

static int wifiPassFn(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen);

/**
 * Application routes
 */
HttpdBuiltInUrl routes[] = {
	// redirect func for the captive portal
	ROUTE_CGI_ARG("*", cgiRedirectApClientToHostname, "esp-terminal.ap"),

	// --- Web pages ---
	ROUTE_TPL_FILE("/", tplScreen, "/term.tpl"),
	ROUTE_TPL_FILE("/about/?", tplAbout, "/about.tpl"),
	ROUTE_FILE("/help/?", "/help.tpl"),

	// --- Sockets ---
	ROUTE_WS(URL_WS_UPDATE, updateSockConnect),

	// --- System control ---
	ROUTE_CGI("/system/reset/?", cgiResetDevice),
	ROUTE_CGI("/system/ping/?", cgiPing),

	// --- WiFi config --- (TODO make this conditional and configurable)
#if WIFI_PROTECT
	ROUTE_AUTH("/wifi*", wifiPassFn),
#endif
	ROUTE_REDIRECT("/cfg/?", "/cfg/wifi"),

	ROUTE_TPL_FILE("/cfg/wifi/?", tplWlan, "/cfg_wifi.tpl"),
	ROUTE_FILE("/cfg/wifi/connecting/?", "/cfg_wifi_conn.tpl"),
	ROUTE_CGI("/cfg/wifi/scan", cgiWiFiScan),
	ROUTE_CGI("/cfg/wifi/connstatus", cgiWiFiConnStatus),
	ROUTE_CGI("/cfg/wifi/set", cgiWiFiSetParams),

	ROUTE_TPL_FILE("/cfg/network/?", tplNetwork, "/cfg_network.tpl"),
	ROUTE_CGI("/cfg/network/set", cgiNetworkSetParams),

	ROUTE_TPL_FILE("/cfg/app/?", tplAppCfg, "/cfg_app.tpl"),
	ROUTE_CGI("/cfg/app/set", cgiAppCfgSetParams),

	ROUTE_FILE("/cfg/admin/?", "/cfg_admin.tpl"),
	ROUTE_CGI("/cfg/admin/write_defaults", cgiPersistWriteDefaults),
	ROUTE_CGI("/cfg/admin/restore_defaults", cgiPersistRestoreDefaults),
	ROUTE_CGI("/cfg/admin/restore_hard", cgiPersistRestoreHard),

	ROUTE_FILESYSTEM(),
	ROUTE_END(),
};

// --- Wifi password protection ---

/**
 * Password for WiFi config
 */
static int ICACHE_FLASH_ATTR wifiPassFn(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen)
{
	if (no == 0) {
		os_strcpy(user, WIFI_AUTH_NAME);
		os_strcpy(pass, WIFI_AUTH_PASS);
		return 1;
	}
	return 0;
}
