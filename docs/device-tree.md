# Device Tree

## Observed Board Description

源资料明确引用 `tspi-rk3566-user-v10-linux.dts`，其路径语义指向 Rockchip ARM64 board description。资料同时覆盖 DTS/DTSI inclusion、node path、label、alias 和 DTC conversion concepts。

## Boundary

- 源目录没有可独立确认来源与许可证的 board DTS/DTSI tree。
- Public Candidate 不包含 DTB、DTBO 或 vendor include files。
- 本仓库不声明已经修改、编译或部署该 board Device Tree。

## Future Evidence Needed

未来如要加入 Device Tree 实现，应至少提供：

1. Upstream/vendor baseline and license.
2. A minimal patch or clearly authored overlay.
3. `compatible`、pinctrl、GPIO or bus binding rationale.
4. `dtc` build result or kernel build log.
5. Target-side binding evidence such as boot log or driver probe output.
