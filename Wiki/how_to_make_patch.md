1. create a new repo in github
2. upload buildroot/output/build/linux-custom/arch/arm/boot/dts/am335x-bone-common.dtsi into the repo, this will gives a copy of old am335x-bone-common.dtsi file
3. modify the local am335x-bone-common.dtsi file based on your need
4. git commit -m "any message"
5. git format-patch -1, this will create the patch file
6. !!! DON'T PUSH THE MODIFIED FILE TO GITHUB !!!
7. change the following line:
- buildroot/output/build/linux-custom/arch/arm/boot/dts/am335x-bone-common.dtsi  
-> arch/arm/boot/dts/am335x-bone-common.dtsi

- diff --git a/buildroot/output/build/linux-custom/arch/arm/boot/dts/am335x-bone-common.dtsi  
b/arch/arm/boot/dts/am335x-bone-common.dtsi  
-> diff --git a/arch/arm/boot/dts/am335x-bone-common.dtsi  
b/arch/arm/boot/dts/am335x-bone-common.dtsi

- --- a/buildroot/output/build/linux-custom/arch/arm/boot/dts/am335x-bone-common.dtsi  
+++ b/buildroot/output/build/linux-custom/arch/arm/boot/dts/am335x-bone-common.dtsi  
-> --- a/arch/arm/boot/dts/am335x-bone-common.dtsi  
+++ b/arch/arm/boot/dts/am335x-bone-common.dtsi

8. Following the steps here
https://github.com/cu-ecen-aeld/buildroot-assignments-base/wiki/Beagle-Bone-Black-Devicetree-Hardware-Support
9. check if buildroot/output/build/linux-custom/arch/arm/boot/dts/am335x-bone-common.dtsi get modified.

