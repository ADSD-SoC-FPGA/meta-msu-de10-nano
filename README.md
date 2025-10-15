# meta-msu-de10-nano
Meta Layers to support the EELE 467 and 468 FPGA Course at Montana State University

This repo's purpose is to help adapt the workflow found in the book [Advanced Digital System Design using SoC FPGAs](https://link.springer.com/book/10.1007/978-3-031-15416-4) by Dr. Snider, to a Yocto-style workflow when it comes to software development for SoC FPGAs.

Along with that, this repo will also be adapting the necessary software found in the [FPGA Open Speech Tools (Frost)](https://github.com/fpga-open-speech-tools/) GitHub organization.

The ultimate goal is to have a concise repository to build all necessary kernel drivers and software to run the examples in the book.

Please refer to the documentation in the `./documentation` folder of this repo for guidance.


Quartus Prime Lite:

Intel SoC EDS:
https://www.intel.com/content/www/us/en/software-kit/661080/intel-soc-fpga-embedded-development-suite-soc-eds-standard-edition-software-version-20-1-for-linux.html

./cv_bsp_generator.py -i /home/night1rider/university/de10-nano/DE10-Nano-Lab-Code/examples/passthrough/hps_isw_handoff/soc_system_passthrough_hps -o $(pwd)/temp