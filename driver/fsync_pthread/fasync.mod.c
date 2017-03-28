#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xc0554ace, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xdfbe4044, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xa47fcfd8, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x938abae9, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xea0e3afd, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xff0ed215, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x972bca9, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xd3f9ab0a, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xd8e484f0, __VMLINUX_SYMBOL_STR(register_chrdev_region) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xaf302dbd, __VMLINUX_SYMBOL_STR(kill_fasync) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xc4f81cdd, __VMLINUX_SYMBOL_STR(fasync_helper) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "3F8E965A84B98E3DF387048");
