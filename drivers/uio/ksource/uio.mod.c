#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xdc6a9dfc, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x503782f0, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0x918cc7b5, __VMLINUX_SYMBOL_STR(platform_device_unregister) },
	{ 0xa8a3bb08, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0xdbba58b7, __VMLINUX_SYMBOL_STR(platform_device_register) },
	{ 0xcbc3f1f, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x36628e6c, __VMLINUX_SYMBOL_STR(__uio_register_device) },
	{ 0xaf26042f, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xdb4b9e0b, __VMLINUX_SYMBOL_STR(uio_unregister_device) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

