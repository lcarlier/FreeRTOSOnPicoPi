# Free RTOS on Pico PI
This repository contains a minimalist project to run Free RTOS on a Pico Pi.

It contains a main function which starts 3 tasks as an example plus the Free RTOS kernel code.

# How to build

Following the instructions on the [Getting started page](https://www.raspberrypi.org/documentation/rp2040/getting-started/) of the Pico Pi Project.

In short, you can build this project like any other Pico Pi project.

```sh
mkdir build
cd build
cmake -DPICO_SDK_PATH=<path_to_pico_sdk>
make -j
```

