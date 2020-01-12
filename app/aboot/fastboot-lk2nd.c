#include <string.h>
#include <debug.h>
#include <pm8x41_regulator.h>
#include <lk2nd-device.h>
#include <dev/fbcon.h>
#include "fastboot.h"

#if TARGET_MSM8916
static void cmd_oem_dump_regulators(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	struct spmi_regulator *vreg;
	for (vreg = target_get_regulators(); vreg->name; ++vreg) {
		snprintf(response, sizeof(response), "%s: enabled: %d, voltage: %d mV",
			 vreg->name, regulator_is_enabled(vreg),
			 regulator_get_voltage(vreg));
		fastboot_info(response);
	}
	fastboot_okay("");
}
#endif

#if WITH_DEBUG_LOG_BUF
static void cmd_oem_lk_log(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk_log_getbuf(), lk_log_getsize());
}
#endif

static void cmd_oem_cmdline(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk2nd_dev.cmdline, strlen(lk2nd_dev.cmdline));
}

static void cmd_oem_fb_clear(const char *arg, void *data, unsigned sz)
{
	fbcon_clear();
	fbcon_flush();
	fastboot_okay("");
}

static void cmd_oem_fb_pull(const char *arg, void *data, unsigned sz)
{
	struct fbcon_config *fbconfig = fbcon_display();
	if (!fbconfig)
		fastboot_fail("fbcon is not initialized");
	fastboot_stage(fbconfig->base,
			fbconfig->width * fbconfig->height * fbconfig->bpp / 8);
}

void fastboot_lk2nd_register_commands(void) {
#if TARGET_MSM8916
	fastboot_register("oem dump-regulators", cmd_oem_dump_regulators);
#endif
#if WITH_DEBUG_LOG_BUF
	fastboot_register("oem lk_log", cmd_oem_lk_log);
#endif
	fastboot_register("oem fb_clear", cmd_oem_fb_clear);
	fastboot_register("oem fb_pull", cmd_oem_fb_pull);

	if (lk2nd_dev.cmdline)
		fastboot_register("oem cmdline", cmd_oem_cmdline);
}
